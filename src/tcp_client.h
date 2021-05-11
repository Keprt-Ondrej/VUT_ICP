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
 * 
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 */
class TCP_Client{
	public:

		/**
		 * @brief	Class constructor
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		TCP_Client();

		/**
		 * @brief	Class constructor
		 * @param	hostname	Hostname or IP address of the server
		 * @param	port		Port of the server
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		TCP_Client(const std::string& hostname, int port);

		/**
		 * @brief	Class destructor
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 */
		~TCP_Client();
		

		/**
		 * @brief	Connects to a server
		 * @param	hostname	Hostname or IP address of the server
		 * @param	port		Port of the server
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
		 * @return 
		 *		- 0		-> OK
		 *		- -1	-> DNS lookup failed
		 *		- -3	-> Socket opening failed
		 *		- -4	-> TCP server connect failed
		 */
		int tcp_connect(const std::string& hostname, int port);

		/**
		 * @brief	Closes ongoing TCP connection
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0		-> OK
		 *		- -1	-> TCP shutdown failed
		 *		- -2	-> Socket closing failed
		 */
		int tcp_disconnect();

		/**
		 * @brief	Sends a packet over TCP connection
		 * @param	msg	String containing the message
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0		-> OK
		 *		- -1	-> Bad arguments or TCP server not connected
		 *		- -2	-> TCP packet sending failed
		 */
		int tcp_send(const std::string& msg);
		
		/**
		 * @brief	Sends a packet over TCP connection
		 * @param	msg	Array containing the message
		 * @param	len	Length of the message
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0		-> OK
		 *		- -1	-> Bad arguments or TCP server not connected
		 *		- -2	-> TCP packet sending failed
		 */
		int tcp_send(const char* msg, int len);

		/**
		 * @brief	Receives one packet
		 * @param	timeout	Time duration to wait for incoming packet
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- 0		-> OK
		 *		- -1	-> Socket data wait (syscall select) error
		 *		- -2	-> Incoming packet timeout
		 *		- -3	-> Set nonblocking read error
		 */
		int tcp_receive(int timeout);

		/**
		 * @brief	Returns packet buffer
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
		 * @return
		 *		- String
		 */
		ustring& pop_buffer();

		/**
		 * @brief	Returns socket descriptor
         * 
         * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
         * 
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