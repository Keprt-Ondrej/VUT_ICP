#include "mqtt_client.h"
#include <ctime>    // time
#include <unistd.h> // getpid, fcntl
#include <fcntl.h>  // fcntl
#include <iomanip>  // setw, internal, setfill; DELLETE LATER
#include <iostream>
#include <QHash>
#include <cerrno>

#include <QTextBrowser>


// REWORK timeout on pending acks & resend

#define INSERT_REM_LEN(len, packet)            \
	while(len != 0){                           \
		packet += static_cast<char>(len&0xFF); \
		len >>= 8;                             \
	}
#define INSERT_2B_LEN(len, packet)              \
	packet += static_cast<char>((len>>8)&0xFF); \
	packet += static_cast<char>(len&0xFF); 

MQTT_Client::MQTT_Client()
: connected(false), tree_root(NULL){}

MQTT_Client::MQTT_Client(client_t info)
: connected(false){
	int retval = broker_connect(info);
	if(retval){
		std::cerr << "Unable to connect to " << client.hostname << ":" << client.port << " broker.\n";
	}
}

MQTT_Client::~MQTT_Client(){
	int retval = broker_disconnect();
	if(retval) std::cerr << "Broker already disconnected : " << retval << std::endl;
}

void _print_packet(const uint8_t* received_packet, int size){
	std::string tmp_buf = "";
			std::cout << tmp_buf << std::endl;
	for(int i = 0; i < size; i++){
		if(i%8 == 0){
			std::cout << " "<< tmp_buf << std::endl;
			tmp_buf.clear();
		}
		std::cout << std::internal << std::setfill('0');
		std::cout << std::hex << std::setw(2) << static_cast<uint16_t>(received_packet[i]) << " ";
		if(received_packet[i] >= 32) tmp_buf += received_packet[i];
		else tmp_buf += '.';
	}

	for(int i = 0; i < size%8; i++){
		std::cout << "   ";
	}
	std::cout << " "<< tmp_buf << std::endl;

	std::cout << std::dec << std::endl;
}
template<typename T>
void print_packet(std::basic_string<T>& tmp){
	_print_packet(reinterpret_cast<const uint8_t*>(tmp.c_str()), tmp.size());
}

int MQTT_Client::broker_connect(client_t info){
	/// Disconnect from prevoius session if there has been one
	broker_disconnect();
	bool unique_id = false;

	/// Store the data
	client = info;
	if(client.client_id == ""){
		unique_client_id();
		unique_id = true;
	}

	/// Connect to an MQTT broker
	int retval = tcp_connect(client.hostname, client.port);
	if(retval) return retval;

	/// Create CONNECT packet
	uint32_t packet_length = 12 + client.client_id.length(); // 10 bytes are static + 2 bytes for client id + the client id
	if(client.username != ""){
		packet_length += 2 + client.username.length(); // 2B length + data
		if(client.password != "")
			packet_length += 2 + client.password.length(); // 2B length + data
	}
	uint32_t remaining_length = to_remaining_len(packet_length);
	std::string connect_packet;

	/// Fixed header - Packet type and remaining length
	connect_packet = static_cast<char>(CONNECT << 4); // Connect control packet type
	INSERT_REM_LEN(remaining_length, connect_packet);

	/// Variable header
	connect_packet += static_cast<char>(0);    // Static 2 byte length of 'MQTT' - protocol name
	connect_packet += static_cast<char>(4);
	connect_packet += "MQTT";                  // Protocol name
	connect_packet += static_cast<char>(4);    // Protocol version (4 = version 3.11 for some reason)

	uint8_t connect_flags = 0;
	if(client.username != ""){
		connect_flags |= 0b10000000;
		if(client.password != "")
			connect_flags |= 0b01000000;
	}
	if(unique_id) connect_flags |= 0b00000010;
	connect_packet += static_cast<char>(connect_flags); // Connect flags, only clean session is set
	INSERT_2B_LEN(60, connect_packet); // 2 Byte keep alive value

	/// Payload - client ID, username, password (This order explicitly)
	INSERT_2B_LEN(client.client_id.length(), connect_packet);
	connect_packet += client.client_id;

	if(connect_flags&0x80){ // Username
		INSERT_2B_LEN(client.username.length(), connect_packet);
		connect_packet += client.username;
	}

	if(connect_flags&0x40){ // Password
		INSERT_2B_LEN(client.password.length(), connect_packet);
		connect_packet += client.password;
	}

	/// Send the packet
	retval = tcp_send(connect_packet.c_str(), connect_packet.length());
	if(retval) return retval-10;

	/// Expect CONNACK to arrive
	add_ack(std::make_tuple(CONNACK, 0));

	connected = true;

	return 0;
}

int MQTT_Client::broker_disconnect(){
	/// Send disconnect nofification
	if(connected){
		connected = false;
		char ping_packet[] = {static_cast<char>(DISCONNECT << 4), 0};
		int retval = tcp_send(ping_packet, 2);
		if(retval) return retval;
		if(tree_root != NULL){
			delete_tree(NULL);
		}
	}

	return 0;
}

int MQTT_Client::publish(const std::string& topic, const std::string& value, pubflg_t opt){
	if(!connected || topic.length() == 0 || topic.length() > 0xFFFF || value.length() > 0xFFFF || opt.QoS >= 3)
		return -1;

	/// Create PUBLISH packet
	uint32_t remaining_length = 0;
	std::string publish_packet;

	if(opt.QoS != 0) remaining_length = to_remaining_len(4+topic.length()+value.length()); // 2+2 bytes for topic and packet identifier lengths + the actual data
	else             remaining_length = to_remaining_len(2+topic.length()+value.length()); // Same as ^ , but no packet identifier

	/// Create fixed header
	publish_packet = static_cast<char>((PUBLISH << 4) | (opt.DUP << 3) | ((opt.QoS&0b11) << 1) | (opt.retain&1));
	INSERT_REM_LEN(remaining_length, publish_packet);

	/// Create variable header
	INSERT_2B_LEN(topic.length(), publish_packet);
	publish_packet += topic;

	uint16_t packet_id = 0;
	if(opt.QoS > 0){
		packet_id = available_packet_id();
		INSERT_2B_LEN(packet_id, publish_packet);
	}

	/// Create payload
	publish_packet += value;

	/// Send the packet
	int retval = tcp_send(publish_packet.c_str(), publish_packet.length());
	if(retval) return retval-10;

	/// Expect publish acknowledgement to arrive
	if(opt.QoS == 1)      add_ack(std::make_tuple(PUBACK, packet_id));
	else if(opt.QoS == 2) add_ack(std::make_tuple(PUBREC, packet_id));

	return 0;
}

// REWORK (un)subscribe multiple topics in one packet
int MQTT_Client::subscribe(const std::string& topic){
	if(!connected) return -1;
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
	if(retval) return retval-10;

	/// Expect SUBACK to arrive
	add_ack(std::make_tuple(SUBACK, packet_id));

	return 0;
}

int MQTT_Client::unsubscribe(const std::string& topic){
	if(!connected) return -1;
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
	if(retval) return retval-10;

	/// Expect UNSUBACK to arrive
	add_ack(std::make_tuple(UNSUBACK, packet_id));

	return 0;
}

int MQTT_Client::ping(){
	if(!connected) return -1;
	/// Send ping request
	char ping_packet[] = {static_cast<char>(PINGREQ << 4), 0};
	int retval = tcp_send(ping_packet, 2);
	if(retval) return retval-10;

	/// Expect PINGRESP to arrive
	add_ack(std::make_tuple(PINGRESP, 0));

	return 0;
}

int MQTT_Client::mqtt_recv(int timeout){
	/// Acquire incoming packet
	int tcp_socket = get_socket();
	if(tcp_socket < 0) return -1;
	fd_set read_socket;
	struct timeval timer = {timeout, 0};
	unsigned char c;
	ustring packet;
	uint32_t remaining_length = 0;
	
	FD_ZERO(&read_socket);
	FD_SET(tcp_socket, &read_socket);


	/// Set up timeout in case the server doesnt respond
	int retval = select(tcp_socket+1, &read_socket, NULL, NULL, &timer);
	if(retval < 0) return retval-10;       // Error
	else if(retval == 0) return -2; // Timeout

	/// Load an MQTT packet
	/// Control header
	retval = recv(tcp_socket, &c, 1, 0);
	if(retval == 0) return -5;
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
		if(retval == 0) return -5;
		if(retval < 0) return -4;
		packet += c;
		remaining_length |= (c&0x7F) << (index*7);
		index++;
	}while(c&0x80);
	
	/// Rest of the packet
	for(index = 0; index < remaining_length; index++){
		retval = recv(tcp_socket, &c, 1, 0);
		if(retval == 0) return -5;
		if(retval < 0){
			return retval-10;
		}
		packet += c;
	}

	/// Do something with the data
	received_data(packet);

	//usleep(1000);	
	return 0;
}

void MQTT_Client::set_tree_root(QStandardItemModel* root){
	tree_root = root;
}

bool MQTT_Client::get_connected(){
	return connected;
}

std::pair<std::string,std::string> getPath(std::string path){
	std::string delimiter = "/";
	std::string name = path.substr(0, path.find(delimiter));
	path.erase(0, path.find(delimiter) + delimiter.length());
	return std::make_pair(name, path);
}

QModelIndex _topic_find(QStandardItem* item, std::string& topic){
	QModelIndex blank = QModelIndex();
	if(item == NULL) return blank;

	std::pair<std::string,std::string> path = getPath(topic);
	if(path.first != item->data(0).toString().toStdString())
		return blank;

	static std::string current_path = "";

	if(item->rowCount() == 0){
		//std::cout << current_path << item->data(0).toString().toStdString() << std::endl;
		return item->index();
	}

	current_path += path.first + '/';

	QModelIndex tmp;
	for(int i = 0; i < item->rowCount(); i++){
		tmp = _topic_find(item->child(i), path.second);
		if(tmp != blank){
			current_path.clear();
			return tmp;
		}
	}
	current_path.erase(current_path.end()-item->data(0).toString().size()-1, current_path.end());
	return blank;
}

QModelIndex MQTT_Client::topic_find(std::string& topic){
	QModelIndex blank = QModelIndex();
	QModelIndex tmp;
	for(int i = 0; i < tree_root->rowCount(); i++){
		tmp = _topic_find(tree_root->item(i), topic);
		if(tmp != blank){
			return tmp;
		}
	}
	return blank;
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
	client.client_id = "ICP_mqtt_explorer_";
	client.client_id += hex_id;

	//std::cout << client_id << " " << client_id.length() << std::endl;
}

uint16_t MQTT_Client::available_packet_id(){
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

void MQTT_Client::add_ack(std::tuple<PacketType, uint16_t> ack){
	pending_ack.push_back(ack);
}

int MQTT_Client::rm_ack(std::tuple<PacketType, uint16_t> ack){
	if(*pending_ack.begin() == ack){
		pending_ack.erase(pending_ack.begin());
		rm_packet_id(std::get<1>(ack));
	}
	else{
		std::cerr << "Expecting: " << std::get<0>(*pending_ack.begin()) << ", " << std::get<1>(*pending_ack.begin()) << std::endl;
		std::cerr << "Got:       " << std::get<0>(ack) << ", " << std::get<1>(ack) << std::endl;
		return -1;
	}
	return 0;
}

int MQTT_Client::rm_packet_id(uint16_t packet_id){
	for(unsigned int i = 0; i < unavailable_packet_id.size(); i++){
		if(unavailable_packet_id[i] == packet_id){
			unavailable_packet_id.erase(unavailable_packet_id.begin() + i);
			break;
		}
	}
	return 0;
}

int MQTT_Client::received_data(ustring& received_packet){
	/// Parse & print acquired packet
	//print_packet(received_packet);
	int qos = 0;
	uint16_t packet_id = 0;
	switch(received_packet[0]>>4){
		case CONNACK:
			std::cout << "CONNACK arrived.\n";
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
			else connected = true;

			break;

		case PUBLISH:
			std::cout << "PUBLISH arrived.\n";
			qos = (received_packet[0]&0b0110) >> 1;
			update_tree(received_packet);

            if (dashBoardOpened){
                dashBoardGUI->updateGUI();
            }

			if(qos != 0){
				uint16_t tmp_len = (received_packet[2] << 8) | received_packet[3];
				packet_id = (received_packet[2+tmp_len] << 8) | received_packet[3+tmp_len];
				if(qos == 1){
					std::cout << "Sending PUBACK.\n";
					char puback_packet[] = {static_cast<char>(PUBACK<<4), 2, static_cast<char>((packet_id>>8)&0x0F), static_cast<char>(packet_id&0x0F)};
					int retval = tcp_send(puback_packet, 4);
					if(retval) return retval-10;
				}
				else if(qos == 2){
					std::cout << "Sending PUBREC.\n";
					char pubrec_packet[] = {static_cast<char>(PUBREC<<4), 2, static_cast<char>((packet_id>>8)&0x0F), static_cast<char>(packet_id&0x0F)};
					int retval = tcp_send(pubrec_packet, 4);
					if(retval) return retval-10;

					/// Expect PUBREL packet
					add_ack(std::make_tuple(PUBREL, packet_id));
				}
			}
			break;

		case PUBACK:
			std::cout << "PUBACK arrived.\n";
			packet_id = (received_packet[2] << 8) | received_packet[3];
			break;

		case PUBREC:{
				std::cout << "PUBREC arrived.\n";
				/// Send PUBREL packet
				packet_id = (received_packet[2] << 8) | received_packet[3];
				char pubrel_packet[] = {static_cast<char>((PUBREL<<4)|2), 2, static_cast<char>(received_packet[2]), static_cast<char>(received_packet[3])};
				int retval = tcp_send(pubrel_packet, 4);
				if(retval) return retval-10;

				/// Expect PUBCOMP packet
				add_ack(std::make_tuple(PUBCOMP, packet_id));
			}
			break;

		case PUBREL:{
				std::cout << "PUBREL arrived.\n";
				/// Send PUBCOMP packet
				packet_id = (received_packet[2] << 8) | received_packet[3];
				char pubcomp_packet[] = {static_cast<char>(PUBCOMP<<4), 2, static_cast<char>((packet_id>>8)&0x0F), static_cast<char>(packet_id&0x0F)};
				int retval = tcp_send(pubcomp_packet, 4);
				if(retval) return retval-10;
			}
			break;

		case PUBCOMP:
			std::cout << "PUBCOMP arrived.\n";
			packet_id = (received_packet[2] << 8) | received_packet[3];
			break;

		case 
			SUBACK: std::cout << "SUBACK arrived.\n"; 
			packet_id = (received_packet[2] << 8) | received_packet[3];
			break;
		case 
			UNSUBACK: std::cout << "UNSUBACK arrived.\n"; 
			packet_id = (received_packet[2] << 8) | received_packet[3];
			break;
		case PINGRESP: std::cout << "PINGRESP arrived.\n"; break;

		default:{
			static int count = 0;
			std::cout << "Oh no, anyway...(" << (received_packet[0]>>4) << "): " << count++ << " times.\n";
			//print_packet<uint8_t>(received_packet);
		}
	}

	if(((received_packet[0]>>4)&0x0F) != PUBLISH &&
	   rm_ack(std::make_tuple(static_cast<PacketType>(received_packet[0]>>4), packet_id))){
		std::cerr << "^^Unexpected packet.\n";
	}

	std::cout << std::endl;

	return 0;
}

data_type_t data_type(std::string& data){
	if(static_cast<uint8_t>(data[0]) == 0xFF &&
	   static_cast<uint8_t>(data[1]) == 0xD8 &&
	   static_cast<uint8_t>(data[2]) == 0xFF) // JPEG
		return BIN;
	else if(static_cast<uint8_t>(data[0]) == 137 && // PNG
			static_cast<uint8_t>(data[1]) == 80 &&
			static_cast<uint8_t>(data[2]) == 78 &&
			static_cast<uint8_t>(data[3]) == 71 &&
			static_cast<uint8_t>(data[4]) == 13 &&
			static_cast<uint8_t>(data[5]) == 10 &&
			static_cast<uint8_t>(data[6]) == 26 &&
			static_cast<uint8_t>(data[7]) == 10)
		return BIN;
	return STRING;
}

int update_topic(QStandardItem* item, std::string& name, std::string value, int depth){
	if(item == NULL || depth < 0) return -1;
	std::pair<std::string,std::string> path = getPath(name);
	static std::string full_path = "";
	full_path += item->data(0).toString().toStdString() + "/";
	if(item->data(0).toString().toStdString() == path.first){
		if(item->hasChildren()){
			int retval = -1;
			depth--;
			for(int i = 0; i < item->rowCount(); i++){
				retval = update_topic(item->child(i), path.second, value, depth);
				if(retval == 0) return 0;
			}
			depth++;
			if(retval == -1){
				for(int i = 0; i < depth; i++){
  					path = getPath(path.second);
  					if(i == depth-1)
  						full_path += item->data(0).toString().toStdString();
					else
						full_path += item->data(0).toString().toStdString() + "/";
					QStandardItem* new_level = new QStandardItem(path.first.c_str());
					new_level->setData(false, 3);
					new_level->setData(true, 4);
					new_level->setData(QString::fromStdString(full_path), 7);
  					item->appendRow(new_level);
  					item = new_level;
				}
				QList<QVariant> my_list;
				QList<QVariant> my_list_types;

				my_list.push_front(QString::fromStdString(value));
				my_list_types.push_front(data_type(value));

				item->setData(true, 3);
				item->setData(true, 4);
				item->setData(my_list_types, 5);
				item->setData(my_list, 6);
				item->setData(QString::fromStdString(full_path), 7);
				item->setForeground(QBrush(QColor(250,0,0)));
				full_path.clear();
				return 0;
			}
		}
		else{
			QList<QVariant> my_list_types = item->data(5).toList();
			QList<QVariant> my_list = item->data(6).toList();

			my_list_types.push_front(data_type(value));
			my_list.push_front(QString::fromStdString(value));
			item->setData(my_list_types, 5);
			item->setData(my_list, 6);
			item->setForeground(QBrush(QColor(250,0,0)));

			return 0;
		}
	}
	full_path.erase(full_path.end()-item->data(0).toString().size()-1, full_path.end());
	return -1;
}

void MQTT_Client::update_tree(ustring& packet){
	if(tree_root == NULL) return;

	/// Find/create topic
	std::string topic;
	int depth = 0;
	uint32_t remaining_length = from_remaining_len(&(packet.c_str()[1]));
	int t_index = 0;
	if(remaining_length >= 0x80){
		if(remaining_length >= 0x8000){
			if(remaining_length >= 0x800000)
				t_index = 5;
			else t_index = 4;
		}
		else t_index = 3;
	}
	else t_index = 2;

	uint16_t topic_len = (packet[t_index] << 8) | packet[t_index+1];
	for(uint16_t i = 0; i < topic_len; i++){
		topic += packet[4+i];
		if(packet[4+i] == '/') depth++;
	}

	//std::cout << ((topic_find(topic) != QModelIndex())?("Topic found"):("Go to hell")) << std::endl;

	std::string value = "";
	for(unsigned int i = 2+t_index+topic_len; i < packet.length(); i++){
		value += packet[i];
	}

	int retval = -1;
	for(int i = 0; i < tree_root->rowCount(); i++){
		retval = update_topic(tree_root->item(i), topic, value, depth);
		if(retval == 0) break;
	}
	if(retval == -1){
		std::pair<std::string,std::string> path = getPath(topic);
		std::string full_path = "";
		QStandardItem* item = new QStandardItem(path.first.c_str());
		if(depth == 0)
			full_path += item->data(0).toString().toStdString();
		else
			full_path += item->data(0).toString().toStdString() + "/";
		item->setData(false, 3);
		item->setData(true, 4);
		item->setData(QString::fromStdString(full_path), 7);
		tree_root->appendRow(item);

		for(int i = 1; i <= depth; i++){
			path = getPath(path.second);
			if(i == depth)
				full_path += item->data(0).toString().toStdString();
			else
				full_path += item->data(0).toString().toStdString() + "/";
			QStandardItem* new_level = new QStandardItem(path.first.c_str());
			new_level->setData(false, 3);
			new_level->setData(true, 4);
			new_level->setData(QString::fromStdString(full_path), 7);
			item->appendRow(new_level);
			item = new_level;
		}

		QList<QVariant> my_list;
		QList<QVariant> my_list_types;

		my_list.push_front(QString::fromStdString(value));
		my_list_types.push_front(data_type(value));

		item->setData(true, 3);
		item->setData(true, 4);
		item->setData(my_list_types, 5);
		item->setData(my_list, 6);
		item->setData(QString::fromStdString(full_path), 7);
		item->setForeground(QBrush(QColor(250,0,0)));
	}
}

void MQTT_Client::delete_tree(QStandardItem* item){
	if(item == NULL){
		if(tree_root == NULL) return;
		for(int i = 0; i < tree_root->rowCount(); i++){
			delete_tree(tree_root->item(i));
		}
		delete tree_root;
		tree_root = NULL;
	}
	else{
		if(item->rowCount() == 0){
			delete item;
			return;
		}
		else{
			for(int i = 0; i < item->rowCount(); i++){
				delete_tree(item->child(i));
			}
			delete item;
			return;
		}
	}
}