#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <sys/socket.h>
#include <string>

typedef std::basic_string<unsigned char> ustring;
enum data_type_t{JSON,STRING,BIN};

class TCP_Client{
	public:
		TCP_Client();
		TCP_Client(const std::string& ip_addr, int port);
		~TCP_Client();
		
		int tcp_connect(const std::string& ip_addr, int port);
		int tcp_disconnect();
		int tcp_send(const std::string& msg);
		int tcp_send(const char* msg, int len);
		int tcp_receive(int timeout);

		ustring& pop_buffer();
		int get_socket();

	private:
		int open_socket;
		struct sockaddr server_address;
		ustring buffer;
		bool connected;
};


#endif // TCP_CLIENT_H