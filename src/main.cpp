#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <memory> // unique_ptr
#include "mqtt_client.h"
#include <thread>


void thread_test(MQTT_Client& client){
	int retval;
	while(client.get_connected()){
		retval = client.mqtt_recv(50);
		if(retval == -2){
			std::cerr << "MQTT timeout.\n";
			client.ping();
		}
		else if(retval) std::cerr << "Some error occured: " << retval << std::endl;
	}
}

int main(int argc, char *argv[]) {
	std::cout << "\nExpecting CONNACK(2)\n";
	MQTT_Client mqtt("127.0.0.1", 6969);
	if(mqtt.mqtt_recv(5)){
		std::cerr << "Connection failed.\n";
		return -1;
	}
	else std::cout << "Connection established.\n";

	std::thread recv_thrd(thread_test, std::ref(mqtt));


	std::cout << "Expecting PINGRESP(13)\n";
	int retval = mqtt.ping();
	if(retval) std::cerr << "Function ping returned with: "<< retval << std::endl;
	
	std::cout << "Expecting None(PUBLISH with 0 QoS)\n";
	retval = mqtt.publish("lol", "69");
	if(retval) std::cerr << "Function publish returned with: "<< retval << std::endl;
	

	std::cout << "Expecting SUBACK(9)&PUBLISH(3) because of retained value\n";
	retval = mqtt.subscribe("#");
	if(retval) std::cerr << "Function subscribe returned with: "<< retval << std::endl;
	
	std::cout << "Expecting UNSUBACK(11)\n";
	retval = mqtt.unsubscribe("lol");
	if(retval) std::cerr << "Function unsubscribe returned with: "<< retval << std::endl;

	std::cout << "Not expecting anything; just in case tere is one more\n";

	std::string lol;
	std::cin >> lol;

	usleep(500000);
	mqtt.broker_disconnect();
	recv_thrd.join();


	return 0;
}


//char c[] = {0x10, 38, 0, 4, 'M', 'Q', 'T', 'T', 4, 2, 0, 0x3c, 0, 26, 'P', 'y', 'y', '_', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};