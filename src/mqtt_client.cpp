/**
 * @file	mqtt_client.cpp
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 *
 * @brief	MQTT client to utilize MQTT protocol
 */

#include "mqtt_client.h"
#include <ctime>    // time
#include <unistd.h> // getpid, fcntl
#include <fcntl.h>  // fcntl
#include <iostream>
#include <random>
#include <chrono>
#include <sstream>

/**
 * @def 	INSERT_REM_LEN
 * @brief	Inserts remaining length according to MQTT protocol into a string
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 *
 * @def 	INSERT_2B_LEN
 * @brief	Inserts 2 byte value into a string
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 *
 */
#define INSERT_REM_LEN(len, packet)            \
	while(len != 0){                           \
		packet += static_cast<char>(len&0xFF); \
		len >>= 8;                             \
	}
#define INSERT_2B_LEN(len, packet)              \
	packet += static_cast<char>((len>>8)&0xFF); \
	packet += static_cast<char>(len&0xFF); 

#define COLOR_RECEIVED	QColor(180,0,0)
#define COLOR_PUBLISHED	QColor(0,180,0)

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
		char disconnect_packet[] = {static_cast<char>(DISCONNECT << 4), 0};
		int retval = tcp_send(disconnect_packet, 2);
		if(retval) return retval;
		if(tree_root != NULL){ 
			delete_tree(NULL);
		}
	}

	stop_simulation();
	stop_receiving();
	return 0;
}

int MQTT_Client::publish(const std::string& topic, const std::string& value, pubflg_t opt){
	if(!connected || topic.length() == 0 || topic.length() > 0xFFFF || value.length() > 0x7FFFFFF || opt.QoS >= 3)
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

	/// Expect this packet to arrive
	published_queue.push_back(topic);

	return 0;
}

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
	if(retval < 0) return retval-10; // Error
	else if(retval == 0) return -2;  // Timeout

	/// Load an MQTT packet
	/// Control header
	retval = recv(tcp_socket, &c, 1, 0);
	if(retval == 0) return -5;
	if(retval < 0) return -4;
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
		if(retval < 0) return -4;
		packet += c;
	}

	/// Do something with the data
	received_data(packet);
	
	return 0;
}

void MQTT_Client::set_tree_root(QStandardItemModel* root){
	tree_root = root;
}

bool MQTT_Client::get_connected(){
	return connected;
}

bool MQTT_Client::get_simulation_state(){
	return simulate;
}

/**
 * @brief	Splits a string into a pair divided by '/'
 * @param	path	String containing path to split
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz) 
 * 
 * @return
 *		- Pair of strings
 */
std::pair<std::string,std::string> getPath(std::string path){
	std::string delimiter = "/";
	std::string name = path.substr(0, path.find(delimiter));
	path.erase(0, path.find(delimiter) + delimiter.length());
	return std::make_pair(name, path);
}

/**
 * @brief	Recursive helper function that finds the QModelIndex
 * @param	item	Element of traversed tree model
 * @param	topic	Path to the element
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 *
 * @return
 *		- QModelIndex
 */
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

/**
 * @brief	Mainloop of receiving thread
 * @param	client	Reference to receiving client
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
void continuous_receive(MQTT_Client& client){
	int retval;
	time_t last_ping = time(0);
	while(client.get_connected()){
		retval = client.mqtt_recv(50);
		if(retval == -2 || last_ping+45 <= time(0)){
			std::cerr << "MQTT timeout.\n";
			client.ping();
			last_ping = time(0);
		}
		else if(retval){
			if(retval == -5) std::cout << "Broker disconnected\n";
			else             std::cerr << "Some error occured: " << retval << std::endl;
			break;
		}
	}
}

int MQTT_Client::start_receiving(){
	if(receiving_thread.joinable())
		return -1;

	receiving_thread = std::thread(continuous_receive, std::ref(*this));
	return 0;
}

int MQTT_Client::stop_receiving(){
	if(receiving_thread.joinable() == false)
		return -1;

	receiving_thread.join();
	return 0;
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

void MQTT_Client::rm_packet_id(uint16_t packet_id){
	for(unsigned int i = 0; i < unavailable_packet_id.size(); i++){
		if(unavailable_packet_id[i] == packet_id){
			unavailable_packet_id.erase(unavailable_packet_id.begin() + i);
			break;
		}
	}
}

int MQTT_Client::received_data(ustring& received_packet){
	/// Parse & print acquired packet
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

/**
 * @brief	Recognises .jpg and .png files
 * @param	data	File to recognize
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 * @return
 *		- STRING
 *		- BIN
 */
data_type_t data_type(const std::string& data){
	if(static_cast<uint8_t>(data[0]) == 0xFF && // JPEG
	   static_cast<uint8_t>(data[1]) == 0xD8 &&
	   static_cast<uint8_t>(data[2]) == 0xFF)
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

/**
 * @brief	Recursive helper function that updates an element or inserts a new one
 * @param	item		Element of traversed tree model
 * @param	name		Path to topic element
 * @param	value		Topic data
 * @param	depth		Current depth of traversed branch
 * @param	received	Flag to signalize if the topic was received or published
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 * @return
 *		- 0		-> OK
 *		- -1	-> Topic not found
 *		- -10	-> Allocation failed
 */
int update_topic(QStandardItem* item, const std::string& name, const std::string value, int depth, bool received){
	if(item == NULL || depth < 0) return -1;
	usleep(10);
	std::pair<std::string,std::string> path = getPath(name);
	static std::string full_path = "";
	full_path += item->data(0).toString().toStdString() + "/";
	if(item->data(0).toString().toStdString() == path.first){
		if(received) item->setForeground(QBrush(COLOR_RECEIVED));
		else		 item->setForeground(QBrush(COLOR_PUBLISHED));

		if(item->hasChildren()){
			int retval = -1;
			depth--;
			for(int i = 0; i < item->rowCount(); i++){
				retval = update_topic(item->child(i), path.second, value, depth, received);
				if(retval == 0) return 0;
				else if(retval == -10) return -10;
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
					if(new_level == NULL){
						std::cerr << "Topic allocation failed\n";
						return -10;
					}
					new_level->setData(false, 3);
					new_level->setData(true, 4);
					new_level->setData(QString::fromStdString(full_path), 7);
  					item->appendRow(new_level);
  					item = new_level;
					if(received) item->setForeground(QBrush(COLOR_RECEIVED));
					else		 item->setForeground(QBrush(COLOR_PUBLISHED));
				}
				QList<QVariant> my_list;
				QList<QVariant> my_list_types;

				data_type_t tmp = data_type(value);
				if(tmp == BIN){
					my_list.push_front(QByteArray(value.c_str(), value.length()));
					my_list_types.push_front(tmp);
				}
				else{
					my_list.push_front(QString::fromStdString(value));
					my_list_types.push_front(data_type(value));
				}


				item->setData(true, 3);
				item->setData(true, 4);
				item->setData(my_list_types, 5);
				item->setData(my_list, 6);
				item->setData(QString::fromStdString(full_path), 7);
				full_path.clear();

				return 0;
			}
		}
		else{
			QList<QVariant> my_list_types = item->data(5).toList();
			QList<QVariant> my_list = item->data(6).toList();

			data_type_t tmp = data_type(value);
			if(tmp == BIN){
				my_list.push_front(QByteArray(value.c_str(), value.length()));
				my_list_types.push_front(tmp);
			}
			else{
				my_list.push_front(QString::fromStdString(value));
				my_list_types.push_front(data_type(value));
			}

			item->setData(my_list_types, 5);
			item->setData(my_list, 6);
			if(received) item->setForeground(QBrush(COLOR_RECEIVED));
			else		 item->setForeground(QBrush(COLOR_PUBLISHED));

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
	//uint32_t remaining_length = from_remaining_len(&(packet.c_str()[1]));
	int t_index = 0;
	if(packet[1]&0x80){
		if(packet[2]&0x80){
			if(packet[3]&0x80)
				t_index = 5;
			else t_index = 4;
		}
		else t_index = 3;
	}
	else t_index = 2;

	uint16_t topic_len = (packet[t_index] << 8) | packet[t_index+1];
	for(uint16_t i = 0; i < topic_len; i++){
		topic += packet[t_index+2+i];
	}

	if(not published_queue.empty() && topic == *published_queue.begin()){
		published_queue.erase(published_queue.begin());
		return;
	}

	//std::cout << topic << std::endl;
	
	std::string value = "";
	for(unsigned int i = 2+t_index+topic_len; i < packet.length(); i++){
		value += packet[i];
	}

	update_tree(topic, value, true);
}

void MQTT_Client::update_tree(const std::string& topic, const std::string& value, bool received){
	int depth = 0;
	for(uint16_t i = 0; i < topic.length(); i++){
		if(topic[i] == '/') depth++;
	}

	int retval = -1;
	for(int i = 0; i < tree_root->rowCount(); i++){
		retval = update_topic(tree_root->item(i), topic, value, depth, received);
		if(retval == 0) break;
		else if(retval == -10){
			return;
		}
	}
	if(retval == -1){
		std::pair<std::string,std::string> path = getPath(topic);
		std::string full_path = "";
		QStandardItem* item = new QStandardItem(path.first.c_str());
		if(item == NULL){
			std::cerr << "Allocation failed\n";
			return;
		}
		if(depth == 0)
			full_path += item->data(0).toString().toStdString();
		else
			full_path += item->data(0).toString().toStdString() + "/";
		item->setData(false, 3);
		item->setData(true, 4);
		item->setData(QString::fromStdString(full_path), 7);
		tree_root->appendRow(item);

		for(int i = 1; i <= depth; i++){
			if(received) item->setForeground(QBrush(COLOR_RECEIVED));
			else		 item->setForeground(QBrush(COLOR_PUBLISHED));
			path = getPath(path.second);
			if(i == depth)
				full_path += item->data(0).toString().toStdString();
			else
				full_path += item->data(0).toString().toStdString() + "/";
			QStandardItem* new_level = new QStandardItem(path.first.c_str());
			if(new_level == NULL){
				std::cerr << "Allocation failed\n";
				return;
			}
			new_level->setData(false, 3);
			new_level->setData(true, 4);
			new_level->setData(QString::fromStdString(full_path), 7);
			item->appendRow(new_level);
			item = new_level;
		}

		QList<QVariant> my_list;
		QList<QVariant> my_list_types;

		data_type_t tmp = data_type(value);
		if(tmp == BIN){
			my_list.push_front(QByteArray(value.c_str(), value.length()));
			my_list_types.push_front(tmp);
		}
		else{
			my_list.push_front(QString::fromStdString(value));
			my_list_types.push_front(data_type(value));
		}

		item->setData(true, 3);
		item->setData(true, 4);
		item->setData(my_list_types, 5);
		item->setData(my_list, 6);
		item->setData(QString::fromStdString(full_path), 7);
		if(received) item->setForeground(QBrush(COLOR_RECEIVED));
		else		 item->setForeground(QBrush(COLOR_PUBLISHED));
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

/**
 * @brief	Conversion from integer to string
 * @param	value	Integer value to convert
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 * @return
 *		- Converted string
 */
std::string itostr(int value){
	std::stringstream converted;
	std::string tmp;
	converted << value;
	converted >> tmp;
	return tmp;
}

/**
 * @brief	Mainloop of simulating thread
 * @param	client	Reference to an MQTT client
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
void traffic_simulation(MQTT_Client& client){
	/// Set random time generator between 0.5 and 2 seconds
    std::mt19937_64 sleep_time(std::random_device{}());
    std::uniform_int_distribution<> time{200, 2000};

    /// Set random number generator for topic generation
    std::mt19937_64 topic_generator(std::random_device{}());
    std::uniform_int_distribution<> topic_path{0, 4};
    std::uniform_int_distribution<> topic_name{0, 11};
    std::uniform_int_distribution<> topic_depth{0, 4};
    std::uniform_int_distribution<> id{0, 20};
    std::uniform_int_distribution<> choice{0, 1};
    std::uniform_int_distribution<> digits{0, 3};
    std::uniform_int_distribution<> to_100{0, 100};
    std::uniform_int_distribution<> to_10k{0, 10000};
    std::uniform_int_distribution<> to_4{0, 4};
    std::uniform_int_distribution<> to_99{0, 99};
    std::uniform_int_distribution<> coords{-90, 90};
    std::uniform_int_distribution<> coords_dec{0, 999999};
    std::uniform_int_distribution<> angle{0, 359};

    std::string topic = "";
    std::string value = "";

	while(client.get_simulation_state() && client.get_connected()){
		/// Generate topic path
		int depth = topic_depth(topic_generator);
		for(int i = 0; i < depth; i++){
			switch(topic_path(topic_generator)){
				case 0: topic += "simulated/"; break;
				case 1: topic += "device/"; break;
				case 2: topic += "sensor/"; break;
				case 3: topic += "data/"; break;
				case 4: topic += "control/"; break;
				default: topic += "test/"; break;
			}
		}

		/// Generate topic name and value
		switch(topic_name(topic_generator)){
			case 0:
				topic += "door sensor " + itostr(id(topic_generator));
				if(choice(topic_generator))
					value = "Door opened";
				else value = "Door closed";
				break;
			case 1:
				topic += "thermal sensor " + itostr(id(topic_generator));
				value = itostr(to_100(topic_generator));
				value += "°C";
				break;
			case 2:
				topic += "wattmeter " + itostr(id(topic_generator));
				value = itostr(to_10k(topic_generator));
				value += "W";
				break;
			case 3:
				topic += "motor " + itostr(id(topic_generator));
				value = itostr(to_10k(topic_generator));
				value += "RPM";
				break;
			case 4:
				topic += "electric valve " + itostr(id(topic_generator));
				value = "Opened " + itostr(to_100(topic_generator)) + "%";
				break;
			case 5:
				topic += "humidity sensor " + itostr(id(topic_generator));
				value = itostr(to_100(topic_generator)) + "%";
				break;
			case 6:
				topic += "cpu " + itostr(id(topic_generator));
				value = "{\"Clock_speed\":" + itostr(to_4(topic_generator)) + "." + itostr(to_99(topic_generator)) + ",";
				value += "\"Temperature\":" + itostr(to_100(topic_generator)) + ",";
				value += "\"CPU_Usage\":" + itostr(to_100(topic_generator)) + "}";
				break;
			case 7:
				topic += "thermostat " + itostr(id(topic_generator));
				value = itostr(to_100(topic_generator));
				value += "°C";
				break;
			case 8:
				topic += "power relay " + itostr(id(topic_generator));
				if(choice(topic_generator))
					value = "Relay switched on";
				else value = "Relay switched off";
				break;
			case 9:
				topic += "gps " + itostr(id(topic_generator));
				value = "{\"lat\": "+ itostr(coords(topic_generator)) + "." + itostr(coords_dec(topic_generator))+",";
				value += "\"long\": "+ itostr(coords(topic_generator)) + "." + itostr(coords_dec(topic_generator))+"}";
				break;
			case 10:
				topic += "pressure sensor " + itostr(id(topic_generator));
				value = itostr(to_10k(topic_generator));
				break;
			case 11:
				topic += "stepper motor " + itostr(id(topic_generator));
				value = "{\"angle\": " + itostr(angle(topic_generator)) + ",";
				value += "\"step\": ";
				if(choice(topic_generator)){
					if(choice(topic_generator))
						value += "\"full\"";
					else value += "\"half\"";
				}
				else value += "\"quarter\"";
				value += "}";
				break;
			default:
				topic += "test";
				value = "Simulated value ";
				value += itostr(id(topic_generator));
				break;
		}

		std::cout << topic << std::endl;

		client.publish(topic, value, {0,0,0});
		client.update_tree(topic, value, false);
		topic.clear();
		value.clear();

    	std::this_thread::sleep_for(std::chrono::milliseconds{time(sleep_time)});
	}
}

void MQTT_Client::start_stop_simulation(){
	if(simulation_thread.joinable()){
		if(simulate) simulate = false;
		simulation_thread.join();
	}
	else{
		if(simulate == false) simulate = true;
		simulation_thread = std::thread(traffic_simulation, std::ref(*this));
	}
}

void MQTT_Client::stop_simulation(){
	simulate = false;
	if(simulation_thread.joinable()){
		simulation_thread.join();
	}
}
