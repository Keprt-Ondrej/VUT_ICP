/**
 * @file	tcp_client.h
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 *
 * @brief	Simple general TCP client
 */
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <sys/socket.h>
#include <string>

typedef std::basic_string<unsigned char> ustring;

/**
 * @brief	Simple TCP communication client
 */
class TCP_Client{
	public:

		/**
		 * @brief	Class constructor
		 */
		TCP_Client();

		/**
		 * @brief	Class constructor
		 * @param	hostname	Hostname or IP address of the server
		 * @param	port		Port of the server
		 */
		TCP_Client(const std::string& hostname, int port);

		/**
		 * @brief	Class destructor
		 */
		~TCP_Client();
		

		/**
		 * @brief	Connects to a server
		 * @param	hostname	Hostname or IP address of the server
		 * @param	port		Port of the server
		 * @return 
		 *		- 0
		 *		- -1
		 *		- -2
		 */
		int tcp_connect(const std::string& hostname, int port);

		/**
		 * @brief	Closes ongoing TCP connection
		 * @return
		 *		- 0
		 *		- -1
		 *		- -2
		 */
		int tcp_disconnect();

		/**
		 * @brief	Sends a packet over TCP connection
		 * @param	msg	String containing the message
		 * @return
		 *		- 0
		 *		- -1
		 *		- -2
		 */
		int tcp_send(const std::string& msg);
		
		/**
		 * @brief	Sends a packet over TCP connection
		 * @param	msg	Array containing the message
		 * @param	len	Length of the message
		 * @return
		 *		- 0
		 *		- -1
		 *		- -2
		 */
		int tcp_send(const char* msg, int len);

		/**
		 * @brief	Receives one packet
		 * @param	timeout	Time duration to wait for incoming packet
		 * @return
		 *		- 0
		 *		- -1
		 *		- -2
		 *		- -3
		 */
		int tcp_receive(int timeout);

		/**
		 * @brief	Returns packet buffer
		 * @return
		 *		- String
		 */
		ustring& pop_buffer();

		/**
		 * @brief	Returns socket descriptor
		 * @return
		 *		- Integer
		 */
		int get_socket();

	private:
		struct sockaddr server_address; ///< Structure containig server information
		int open_socket; ///< Open socket descriptor
		ustring buffer;  ///< Packet buffer
		bool connected;  ///< Flag representing connecton state
};


#endif // TCP_CLIENT_H