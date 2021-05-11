/**
 * @file	mqtt_client.h
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 *
 * @brief	MQTT client to utilize MQTT protocol
 */

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

/**
 * @brief	Constants of all packet types for MQTT protocol
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
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
	AUTH
};


/**
 * @brief	Encapsulated data for MQTT client
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
typedef struct ClientInfo{
	std::string hostname;  ///< Server hostname or IP
	int port;              ///< Server port
	std::string username;  ///< Client username
	std::string password;  ///< Client password
	std::string client_id; ///< Client specific session ID
} client_t;


/**
 * @brief	Encapsulated flags for publishing data
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
typedef struct PublishFlags{
	bool DUP;    ///< Duplicate flag
	uint8_t QoS; ///< Quality of Service value
	bool retain; ///< Server retain flag
} pubflg_t;

/**
 * @brief	Received data type
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
enum data_type_t{JSON,STRING,BIN};

/**
 * @brief	TCP extension class for MQTT protocol
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
class MQTT_Client : private TCP_Client{
	public:
		/**
		 * @brief	Class constructor
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		MQTT_Client();
		/**
		 * @brief	Class constructor
		 * @param	info	Encapsulated client information
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		MQTT_Client(client_t info);
		/**
		 * @brief	Class destructor
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		~MQTT_Client();
		

		/**
		 * @brief	Connects to an MQTT broker over TCP communication
		 * @param	info	Encapsulated client information
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 *		- -3
		 *		- -4
		 *		- -11
		 *		- -12
		 */
		int broker_connect(client_t info);

		/**
		 * @brief	Closes current session with connected MQTT broker
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 *		- -2
		 */
		int broker_disconnect();

		/**
		 * @brief	Sends data to MQTT broker over MQTT protocol
		 * @param	topic	String containing topic path
		 * @param	value	String containing topic payload
		 * @param	opt 	Publish flags
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 *		- -11
		 *		- -12
		 */
		int publish(const std::string& topic, const std::string& value, pubflg_t opt={0,0,0});

		/**
		 * @brief	Tells the broker to send over this given topic
		 * @param	topic	String containing topic path for subscription
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 *		- -11
		 *		- -12
		 */
		int subscribe(const std::string& topic);

		/**
		 * @brief	Tells the broker to stop sending over this given topic
		 * @param	topic	String containing topic path to unsubscribe
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 *		- -11
		 *		- -12
		 */
		int unsubscribe(const std::string& topic);

		/**
		 * @brief	Pings broker to let it know the client is still responding
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 */
		int ping();

		/**
		 * @brief	Receives one MQTT packet
		 * @param	timeout	Time duration to wait for the packet
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 *		- -2
		 *		- -4
		 *		- -5
		 *		- -11
		 *		- -21
		 */
		int mqtt_recv(int timeout);

		/**
		 * @brief	Sets a pointer to shown tree structure
		 * @param	root	Pointer to shown tree model
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		void set_tree_root(QStandardItemModel* root);

		/**
		 * @brief	Returns connection state
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- false
		 *		- true
		 */
		bool get_connected();

		/**
		 * @brief	Finds a QModelIndex index in shown tree model
		 * @param	topic	String containig topic path
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- QModelIndex
		 */
		QModelIndex topic_find(std::string& topic);

		/**
		 * @brief	Starts a thread that receives every packet
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 */
		int start_receiving();

		/**
		 * @brief	Waits and joins a running thread
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 */
		int stop_receiving();
		
		/**
		 * @brief	Updates shown tree model or inserts a new element
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @param	packet	Raw PUBLISH packet
		 */
		void update_tree(ustring& packet);
		
		/**
		 * @brief	Updates shown tree model or inserts a new element
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @param	topic		Path to topic element
		 * @param	value		Topic data
		 * @param	received	Flag to signalize if the topic was received or published
		 */
		void update_tree(const std::string& topic, const std::string& value, bool received);

	private:

		/**
		 * @brief	Encodes 4 byte integer to an MQTT protocol length
		 * @param	rem_len	Integer to encode
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- Encoded length
		 */
		uint32_t to_remaining_len(uint32_t rem_len);

		/**
		 * @brief	Decodes 1 to 4 bytes of encoded MQTT protocol length
		 * @param	rem_len	Array containing 1 to 4 bytes of encoded MQTT protocol length
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- Decoded length
		 */
		uint32_t from_remaining_len(const unsigned char rem_len[4]);

		/**
		 * @brief	Creates an unique client ID
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		void unique_client_id();

		/**
		 * @brief	Creates and stores an unique ID used for packet identification
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- Integer
		 */
		uint16_t available_packet_id();

		/**
		 * @brief	Removes stored integer value
		 * @param	packet_id	Integer value to remove
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		void rm_packet_id(uint16_t packet_id);

		/**
		 * @brief	Adds a pair of expected packets to a queue
		 * @param	ack	A pair containing a packet type and a packet ID
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		void add_ack(std::tuple<PacketType, uint16_t> ack);

		/**
		 * @brief	Removes received packet pair from queue
		 * @param	ack	A pair containing a packet type and a packet ID
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 */
		int rm_ack(std::tuple<PacketType, uint16_t> ack);

		/**
		 * @brief	Program logic that decides what to do with the received packet
		 * @param	received_packet	Received packet from TCP communication
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0
		 *		- -1
		 */
		int received_data(ustring& received_packet);

		/**
		 * @brief	Recursively removes and frees all elements from shown three model
		 * @param	item	Pointer to tree model elements
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		void delete_tree(QStandardItem* item);

		client_t client;               ///< Encapsulated client information
		bool connected;                ///< Flag that holds connection state
		std::thread receiving_thread;  ///< Thread that receives all incoming packets
		QStandardItemModel* tree_root; ///< Pointer to shown tree model
		std::vector<std::tuple<PacketType, uint16_t>> pending_ack; ///< Queue of pairs containting packet type and packet ID
		std::vector<uint16_t> unavailable_packet_id;               ///< List of packet IDs in use
		std::vector<std::string> published_queue;                  ///< Block updating topics in published queue
};
#endif // MQTT_CLIENT_H