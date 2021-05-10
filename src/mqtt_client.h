#ifndef	MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "tcp_client.h"
#include <vector>
#include <cstdint>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QtCore>
#include <QtCore/QVariant>
#include <QtWidgets/QTreeView>
#include <QHBoxLayout>
#include "dashboard.h"

//enum data_type_t{JSON, STRING, BIN}

enum PacketType{
	RESERVED, // Forbidden
	CONNECT,
	CONNACK,
	PUBLISH,
	PUBACK,
	PUBREC,
	PUBREL,
	PUBCOMP,
	SUBSCRIBE,
	SUBACK,
	UNSUBSCRIBE, //10
	UNSUBACK,
	PINGREQ,
	PINGRESP,
	DISCONNECT,
	AUTH // Unused?
};

typedef struct ClientInfo{
	std::string hostname;
	int port;
	std::string username;
	std::string password;
	std::string client_id;
} client_t;

typedef struct PublishFlags{
	bool DUP;    // Duplicate
	uint8_t QoS; // Quality of Service
	bool retain; // Store on server store
} pubflg_t;

class MQTT_Client : private TCP_Client{
	public:
		MQTT_Client();
		MQTT_Client(client_t info);
		~MQTT_Client();
		
		int broker_connect(client_t info);
		int broker_disconnect();
		int publish(const std::string& topic, const std::string& value, pubflg_t opt={0,0,0});
		int subscribe(const std::string& topic);
		int unsubscribe(const std::string& topic);
		int ping();
		int mqtt_recv(int timeout);
		void set_tree_root(QStandardItemModel* root);
		bool get_connected();
		QModelIndex topic_find(std::string& topic);
        
        bool dashBoardOpened = false;
        DashBoard *dashBoardGUI;

	private:
		uint32_t to_remaining_len(uint32_t rem_len);
		uint32_t from_remaining_len(const unsigned char rem_len[4]);
		void unique_client_id();
		uint16_t available_packet_id();
		void add_ack(std::tuple<PacketType, uint16_t> ack);
		int rm_ack(std::tuple<PacketType, uint16_t> ack);
		int rm_packet_id(uint16_t packet_id);
		int received_data(ustring& received_packet);
		void update_tree(ustring& packet);

		client_t client;
		std::vector<std::tuple<PacketType, uint16_t>> pending_ack; // ack type, packet_id and ?timestamp(not yet)
		std::vector<uint16_t> unavailable_packet_id;
		bool connected;
		QStandardItemModel* tree_root;

};

#endif // MQTT_CLIENT_H