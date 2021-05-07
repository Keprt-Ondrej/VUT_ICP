#ifndef	MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "tcp_client.h"
#include "shared.h"
#include <vector>
#include <cstdint>

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
	UNSUBSCRIBE,
	UNSUBACK,
	PINGREQ,
	PINGRESP,
	DISCONNECT,
	AUTH // Unused?
};

typedef struct ClientInfo{
	std::string& hostname;
	int port;
	std::string& username;
	std::string& password;
	std::string& client_id;
} client_t;

typedef struct PublishFlags{
	bool DUP;    // Duplicate
	uint8_t QoS; // Still got no idea
	bool retain; // Server store
} pubflg_t;

class MQTT_Client : private TCP_Client{
	public:
		MQTT_Client();
		MQTT_Client(const std::string& ip_address, int port);
		MQTT_Client(const std::string& custom_id);
		MQTT_Client(const std::string& custom_id, const std::string& ip_address, int port);
		~MQTT_Client();
		
		int broker_connect(const std::string& ip_address, int port);
		int broker_disconnect();
		int publish(const std::string& topic, const std::string& value, int QoS=0, bool retain=false);
		int subscribe(const std::string& topic);
		int unsubscribe(const std::string& topic);
		int ping();
		int mqtt_recv(int timeout);
		bool get_connected();

	private:
		uint32_t to_remaining_len(uint32_t rem_len);
		uint32_t from_remaining_len(const unsigned char rem_len[4]);
		void unique_client_id();
		int available_packet_id();
		void add_ack(PacketType ack);
		int rm_ack(PacketType ack);
		int received_data(ustring& received_packet);

		std::string client_id;
		std::vector<PacketType> pending_ack;
		std::vector<int> unavailable_packet_id;
		bool connected;

};

#endif // MQTT_CLIENT_H