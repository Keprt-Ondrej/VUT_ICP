/**
 * @file	tcp_client.cpp
 * @author	Matus Fabo (xfabom01@stud.fit.vutbr.cz)
 *
 * @brief	Simple general TCP client
 */

#include "tcp_client.h"

#include <iostream>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>

TCP_Client::TCP_Client()
: open_socket(-1), connected(false){}

TCP_Client::TCP_Client(const std::string& ip_addr, int port)
: open_socket(-1), connected(false){
	tcp_connect(ip_addr, port);
}

TCP_Client::~TCP_Client(){
	tcp_disconnect();
}

int TCP_Client::tcp_connect(const std::string& hostname, int port){
	if(connected)
		tcp_disconnect();

	/// Get IP through dns
	struct	addrinfo* ai_ret;

	int retval = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), NULL, &ai_ret);
	if(retval) return -1;
	server_address = *ai_ret->ai_addr;

	/// Connect to the server
	open_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(open_socket <= 0) return -3;

	retval = connect(open_socket, &server_address, sizeof(server_address));
	freeaddrinfo(ai_ret);
	if(retval) return -4;
	connected = true;
	return 0;
}

int TCP_Client::tcp_disconnect(){
	if(!connected)
		return 0;

	int retval = shutdown(open_socket, SHUT_WR);
	if(retval) return -1;

	retval = close(open_socket);
	if(retval) return -2;

	connected = false;

	return 0;
}

int TCP_Client::tcp_send(const std::string& msg){
	return tcp_send(msg.c_str(), msg.length());
}

int TCP_Client::tcp_send(const char* msg, int len){
	if(msg == NULL || len <= 0 || !connected) return -1;

	int retval = send(open_socket, msg, len, MSG_NOSIGNAL);
	if(retval < 0){
		connected = false;
		return -2;
	}
	return 0;
}

int TCP_Client::tcp_receive(int timeout){
	fd_set read_socket;
	struct timeval timer = {timeout, 0};
	unsigned char c;
	
	FD_ZERO(&read_socket);
	FD_SET(open_socket, &read_socket);

	/// Set up timeout in case the server doesnt respond
	int retval = select(open_socket+1, &read_socket, NULL, NULL, &timer);
	if(retval < 0) return -1;
	else if(retval == 0){
		std::cerr << "TCP read timeout.\n\n";
		return -2;
	}

	buffer.clear();

	retval = fcntl(open_socket, F_SETFL, O_NONBLOCK);
	if(retval < 0) return -3;

	do{ // Load all data into the buffer
		retval = recv(open_socket, &c, 1, 0);
		if(retval!= 0) buffer += c;
	}while(retval > 0);

	return 0;
}

ustring& TCP_Client::pop_buffer(){
	return buffer;
}

int TCP_Client::get_socket(){
	return open_socket;
}