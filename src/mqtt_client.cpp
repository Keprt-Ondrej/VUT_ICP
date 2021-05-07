#include "mqtt_client.h"
#include <ctime>    // time
#include <unistd.h> // getpid, fcntl
#include <fcntl.h>  // fcntl
#include <iomanip>  // setw, internal, setfill; DELLETE LATER
#include <cstring>  // memcmp

// REWORK remaining lengths check size for errors
// REWORK check connected state
// REWORK fix when broker disconnects

#define INSERT_REM_LEN(len, packet)            \
	while(len != 0){                           \
		packet += static_cast<char>(len&0xFF); \
		len >>= 8;                             \
	}
#define INSERT_2B_LEN(len, packet)              \
	packet += static_cast<char>((len>>8)&0xFF); \
	packet += static_cast<char>(len&0xFF); 

// REWORK change to ClientInfo
MQTT_Client::MQTT_Client()
: connected(false){
	unique_client_id();
}

MQTT_Client::MQTT_Client(const std::string& custom_id)
: client_id(custom_id), connected(false){}

MQTT_Client::MQTT_Client(const std::string& ip_address, int port)
: connected(false){
	unique_client_id();
	int retval = broker_connect(ip_address, port);
	if(retval){
		std::cerr << "Unable to connect to " << ip_address << ":" << port << " broker.\n";
	}
	else connected = true;
}

MQTT_Client::MQTT_Client(const std::string& custom_id, const std::string& ip_address, int port)
: client_id(custom_id), connected(false){
	int retval = broker_connect(ip_address, port);
	if(retval){
		std::cerr << "Unable to connect to " << ip_address << ":" << port << " broker.\n";
	}
	else connected = true;
}

MQTT_Client::~MQTT_Client(){
	int retval = broker_disconnect();
	if(retval) std::cerr << "Broker already disconnected : " << retval << std::endl;
}

int MQTT_Client::broker_connect(const std::string& ip_address, int port){
	/// Connect to an MQTT broker
	int retval = tcp_connect(ip_address, port);
	if(retval) return retval;

	/// Create CONNECT packet
	uint32_t remaining_length = to_remaining_len(12 + client_id.length()); // 10 bytes are static + 2 bytes for payload length
	std::string connect_packet;

	/// Fixed header - Packet type and remaining length
	connect_packet = static_cast<char>(CONNECT << 4); // Connect control packet type
	INSERT_REM_LEN(remaining_length, connect_packet);

	/// Variable header
	connect_packet += static_cast<char>(0);    // Static 2 byte length of 'MQTT' - protocol name
	connect_packet += static_cast<char>(4);
	connect_packet += "MQTT";                  // Protocol name
	connect_packet += static_cast<char>(4);    // Protocol version (4 = version 3.11 for some reason)
	connect_packet += static_cast<char>(0x02); // Connect flags, only clean session is set
	connect_packet += static_cast<char>(0);    // 2 Byte keep alive value
	connect_packet += static_cast<char>(60);

	/// Payload - client ID
	INSERT_2B_LEN(client_id.length(), connect_packet);
	connect_packet += client_id;


	/// Send the packet
	retval = tcp_send(connect_packet.c_str(), connect_packet.length());
	if(retval) return -11;

	/// Expect CONNACK to arrive
	add_ack(CONNACK);

	connected = true;

	return 0;
}

int MQTT_Client::broker_disconnect(){
	/// Send disconnect nofification
	if(connected){
		char ping_packet[] = {static_cast<char>(DISCONNECT << 4), 0};
		int retval = tcp_send(ping_packet, 2);
		if(retval) return retval;
		connected = false;
	}

	return 0;
}

int MQTT_Client::publish(const std::string& topic, const std::string& value, int QoS, bool retain){
	// REWORK change to PublishFlags
	if(topic.length() == 0 || topic.length() > 0xFFFF || value.length() > 0xFFFF || QoS < 0 || QoS >= 3)
		return -1;

	/// Create PUBLISH packet
	uint32_t remaining_length = 0;
	std::string publish_packet;

	if(QoS != 0) remaining_length = to_remaining_len(6+topic.length()+value.length()); // 2+2+2 bytes for topic, packet identifier and payload lengths + the actual data
	else         remaining_length = to_remaining_len(4+topic.length()+value.length()); // Same as ^ , but no packet identifier

	/// Create fixed header
	// REWORK add DUP flag (if this packet has been already sent)
	if(QoS != 0) publish_packet = static_cast<char>((PUBLISH << 4) | (0 << 3) | (QoS << 1) | (retain&1));
	else         publish_packet = static_cast<char>((PUBLISH << 4) | (0 << 3) | (0 << 1) | (retain&1));
	INSERT_REM_LEN(remaining_length, publish_packet);

	/// Create variable header
	INSERT_2B_LEN(topic.length(), publish_packet);
	publish_packet += topic;

	if(QoS > 0){
		// REWORK add packet id
	}

	/// Create payload
	INSERT_2B_LEN(value.length(), publish_packet);
	publish_packet += value;

	/// Send the packet
	int retval = tcp_send(publish_packet.c_str(), publish_packet.length());
	if(retval) return retval-10;

	/// Expect publish acknowledgement to arrive
	if(QoS == 1)      add_ack(PUBACK);
	else if(QoS == 2) add_ack(PUBREC);

	return 0;
}

// REWORK (un)subscribe multiple topics in one packet
int MQTT_Client::subscribe(const std::string& topic){
	/// Create SUBSCRIBE packet
	int packet_id = available_packet_id();
	int remaining_length = to_remaining_len(5+topic.length()); // 2+2+1 bytes - packet id, topic and requested QoS lengths + topic itself
	std::string sub_packet;

	/// Create fixed header
	sub_packet = static_cast<char>((SUBSCRIBE << 4) | 0x2); // That 0x2 has to be there for some reason
	INSERT_REM_LEN(remaining_length, sub_packet);

	/// Create variable header
	INSERT_2B_LEN(packet_id, sub_packet);

	/// Create payload
	INSERT_2B_LEN(topic.length(), sub_packet);
	sub_packet += topic;
	sub_packet += static_cast<char>(0); // Request QoS of 0

	/// Send the packet
	int retval = tcp_send(sub_packet.c_str(), sub_packet.length());
	if(retval) return retval;

	/// Expect SUBACK to arrive
	add_ack(SUBACK);

	return 0;
}

int MQTT_Client::unsubscribe(const std::string& topic){
	/// Create UNSUBSCRIBE packet
	int packet_id = available_packet_id();
	int remaining_length = to_remaining_len(4+topic.length()); // 2+2 bytes - packet id and topic lengths + topic itself
	std::string unsub_packet;

	/// Create fixed header
	unsub_packet = static_cast<char>((UNSUBSCRIBE << 4) | 0x2); // That 0x2 has to be there for some reason
	INSERT_REM_LEN(remaining_length, unsub_packet);

	/// Create variable header
	INSERT_2B_LEN(packet_id, unsub_packet);

	/// Create payload
	INSERT_2B_LEN(topic.length(), unsub_packet);
	unsub_packet += topic;

	/// Send the packet
	int retval = tcp_send(unsub_packet.c_str(), unsub_packet.length());
	if(retval) return retval;

	/// Expect UNSUBACK to arrive
	add_ack(UNSUBACK);

	return 0;
}

int MQTT_Client::ping(){
	/// Send ping request
	char ping_packet[] = {static_cast<char>(PINGREQ << 4), 0};
	int retval = tcp_send(ping_packet, 2);
	if(retval) return retval;

	/// Expect PINGRESP to arrive
	add_ack(PINGRESP);

	return 0;
}

int MQTT_Client::mqtt_recv(int timeout){
	/// Acquire incoming packet
	int tcp_socket = get_socket();
	fd_set read_socket;
	struct timeval timer = {timeout, 0};
	unsigned char c;
	ustring packet;
	uint32_t remaining_length = 0;
	
	FD_ZERO(&read_socket);
	FD_SET(tcp_socket, &read_socket);

	/// Set up timeout in case the server doesnt respond
	int retval = select(tcp_socket+1, &read_socket, NULL, NULL, &timer);
	if(retval < 0) return -1;       // Error
	else if(retval == 0) return -2; // Timeout

	/// Set nonblocking reading
	retval = fcntl(tcp_socket, F_SETFL, O_NONBLOCK);
	if(retval < 0) return -3;

	/// Load an MQTT packet
	/// Control header
	retval = recv(tcp_socket, &c, 1, 0);
	if(retval < 0) return -4;
	if(c >> 4 == 0){
		broker_disconnect();
		tcp_receive(10);
		return -5;
	}
	packet += c;

	/// Remaining length
	uint32_t index = 0;
	do{
		retval = recv(tcp_socket, &c, 1, 0);
		if(retval < 0) return -4;
		packet += c;
		remaining_length |= (c&0x7F) << (index*7);
	}while(c&0x80);

	/// Rest of the packet
	for(index = 0; index < remaining_length; index++){
		retval = recv(tcp_socket, &c, 1, 0);
		if(retval < 0) return -4;
		packet += c;
	}

	/// Do something with the data
	received_data(packet);

	return 0;
}

bool MQTT_Client::get_connected(){
	return connected;
}

uint32_t MQTT_Client::to_remaining_len(uint32_t rem_len){
	if(rem_len == 0 || rem_len&0xF0000000){ // Maximal length is 28 bits
		std::cerr << "Invalid length of " << rem_len << std::endl;
		return 0xFFFFFFFF;
	}
	uint32_t encoded = 0;
	uint8_t enc_byte = 0;
	uint32_t remaining = rem_len;
	int index = 0;

	while(remaining > 0){
		enc_byte = (remaining&0x7F) | 0x80;
		remaining >>= 7;
		encoded |= enc_byte << (index++ * 8);
	}
	encoded &= ~(1 << ((index * 7)+index-1));

	return encoded;
}

uint32_t MQTT_Client::from_remaining_len(const unsigned char* rem_len){
	if(rem_len == NULL) return 0xFFFFFFFF;
	int index = 0;
	uint32_t tmp = 0;
	do{
		tmp |= (rem_len[index]&0x7F) << (index*7);
	}while(rem_len[index++]&0x80);

	return tmp;
}

void MQTT_Client::unique_client_id(){
	/// Set pseudo random generator seed
	std::srand(static_cast<int>(getpid()) + std::time(NULL));
	char hex_id[11];
	sprintf(hex_id, "%lx", std::rand()&0xFFFFFFFFFF);


	/// Set client ID
	client_id = "ICP_mqtt_explorer_";
	client_id += hex_id;

	//std::cout << client_id << " " << client_id.length() << std::endl;
}

int MQTT_Client::available_packet_id(){
	if(unavailable_packet_id.empty()){
		unavailable_packet_id.push_back(1);
		return 1;
	}
	else{
		int tmp = unavailable_packet_id.back()+1;
		unavailable_packet_id.push_back(tmp);
		return tmp;
	}
}

void MQTT_Client::add_ack(PacketType ack){
	pending_ack.push_back(ack);
	std::cout << pending_ack.size() << std::endl;
}

int MQTT_Client::rm_ack(PacketType ack){
	if(*pending_ack.begin() == ack){
		pending_ack.erase(pending_ack.begin());
		std::cout << pending_ack.size();
	}
	else return -1;
	return 0;
}

int MQTT_Client::received_data(ustring& received_packet){
	// REWORK packet id return & proper acks
	/// Parse & print acquired packet
	int qos = 0;
	switch(received_packet[0]>>4){
		case CONNACK:
			std::cout << "CONNACK" << " arrived.\n";
			if(received_packet.length() != 4 || 
			   received_packet[2] != 0)
				std::cerr << "Bad CONNACK packet\n";
			if(received_packet[3] != 0){
				switch(received_packet[3]){
					case 1: std::cerr << "Bad protocol version.\n"; break;
					case 2: std::cerr << "Bad client ID.\n"; break;
					case 3: std::cerr << "Server unavailable.\n"; break;
					case 4: std::cerr << "Bad username or password.\n"; break;
					case 5: std::cerr << "Not authorized.\n"; break;
				}
			}

			if(rm_ack(CONNACK)){
				std::cerr << "^^Unexpected packet.\n";
			}
			break;

		case PUBLISH:
			std::cout << "PUBLISH" << " arrived.\n";
			qos = (received_packet[0]&0b0110) >> 1;
			if(qos == 1){
				std::cout << "Sending PUBACK.\n";
			}
			break;

		case PUBACK:
			std::cout << "PUBACK" << " arrived.\n";
			break;

		case PUBREC:
			std::cout << "PUBREC" << " arrived.\n";
			break;

		case PUBREL:
			std::cout << "PUBREL" << " arrived.\n";
			break;

		case PUBCOMP:
			std::cout << "PUBCOMP" << " arrived.\n";
			break;

		case SUBACK:
			std::cout << "SUBACK" << " arrived.\n";
			if(rm_ack(SUBACK)){
				std::cerr << "^^Unexpected packet.\n";
			}
			break;

		case UNSUBACK:
			std::cout << "UNSUBACK" << " arrived.\n";
			if(rm_ack(UNSUBACK)){
				std::cerr << "^^Unexpected packet.\n";
			}
			break;

		case PINGRESP:
			std::cout << "PINGRESP" << " arrived.\n";
			if(rm_ack(PINGRESP)){
				std::cerr << "^^Unexpected packet.\n";
			}
			break;

		case DISCONNECT:
			std::cout << "DISCONNECT" << " arrived.\n";
			break;

		default:{
				static int count = 0;
				std::cout << "Oh no, anyway...(" << (received_packet[0]>>4) << "): " << count++ << " times.\n";
				for(int i = 0; i < received_packet.size(); i++){
					std::cout << std::internal << std::setfill('0');
					std::cout << std::hex << std::setw(2) << static_cast<unsigned int>(received_packet[i]) << " ";
					if(i%8 == 0) std::cout << std::endl;
				}
				std::cout << std::dec << std::endl;
				char temp[] = {2,2,2};
				if(std::memcmp(received_packet.c_str(), temp, 3) == 0){
					tcp_receive(10);
					ustring& tmp_str = pop_buffer();
					for(int i = 0; i < tmp_str.size(); i++){
						std::cout << std::internal << std::setfill('0');
						std::cout << std::hex << std::setw(2) << static_cast<unsigned int>(tmp_str[i]) << " ";
						if(i%8 == 0) std::cout << std::endl;
					}
				}
			}
	}
	std::cout << std::endl;

	return 0;
}
