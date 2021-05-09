/**
 * @file main.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * @author Fabo Matúš (xfabom01@stud.fit.vutbr.cz)
 * 
 * @brief 
 * 
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>

#include <thread>

#include <QApplication>
#include "mainwindow.h"
#include "connectserver.h"
#include <iostream>
#include <ctime>

/*! \mainpage My Personal Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

void thread_test(MQTT_Client& client){
	int retval;
	time_t last_ping = time(0);
	while(client.get_connected()){
		retval = client.mqtt_recv(50);
		if(retval == -2 || last_ping+50 <= time(0)){
			std::cerr << "MQTT timeout.\n";
			client.ping();
			last_ping = time(0);
		}
		else if(retval){
			std::cerr << "Some error occured: " << retval << std::endl;
			break;
		}
	}
}


int main(int argc, char *argv[]) {  
	QApplication a(argc, argv);
	/// Set up client info
	MQTT_Client mqtt;	
	client_t data = {
		"test.mosquitto.org",
        1883,
        "",
        "",
        ""
	};

	//connectServerNewWindow(mqtt); // funkce si zavola int MQTT_Client::broker_connect(client_t info)


	/// Connect to MQTT broker

	std::cout << "\nExpecting CONNACK(2)\n";
	mqtt.broker_connect(data);
	if(mqtt.mqtt_recv(5)){
		std::cerr << "Connection failed.\n";
		return -1;
	}
	else std::cout << "Connection established.\n";

	/// Start receiving packets
	std::thread recv_thrd(thread_test, std::ref(mqtt));


	std::cout << "Expecting PINGRESP(13)\n";
	int retval = mqtt.ping();
	if(retval) std::cerr << "Function ping returned with: "<< retval << std::endl;
	
	std::cout << "Expecting PUBACK(PUBLISH with 1 QoS)\n";
	retval = mqtt.publish("lol", "69", {0,1,0});
	if(retval) std::cerr << "Function publish returned with: "<< retval << std::endl;
	

	std::cout << "Expecting SUBACK(9)&PUBLISH(3) because of retained value\n";
	retval = mqtt.subscribe("#");
	if(retval) std::cerr << "Function subscribe returned with: "<< retval << std::endl;
	retval = mqtt.subscribe("$SYS/#");
	if(retval) std::cerr << "Function subscribe returned with: "<< retval << std::endl;
	
	std::cout << "Expecting UNSUBACK(11)\n";
	retval = mqtt.unsubscribe("lol");
	if(retval) std::cerr << "Function unsubscribe returned with: "<< retval << std::endl;

	/// Leave it running until user closes it
	//std::string lol;
	//std::cin >> lol;

	usleep(500000);

	std::cout << "Error of -5 is ok, it means that broker is not available anymore\n";


	QList<QVariant> x1;
    for(int i=0; i<5; i++){
		x1.append((double)i);
	}

    QVariant x2 = x1;

	
	QList<QVariant> karel = x2.toList();
	karel.push_front((double)73);
	x2 = karel;
	
     
    QList<QVariant> x3 = x2.toList();
    for(int i=0; i<x3.size(); i++){
        //std::cout << x3.at(i).toDouble() << "\n";
	}

	std::vector<int> fiskus;
	fiskus.push_back(5);
	fiskus.push_back(42);

	
	//connectServerNewWindow();
	MainWindow w(mqtt);
	w.show();

	retval = a.exec();

	mqtt.broker_disconnect();
	recv_thrd.join();

	return retval;
}
//char c[] = {0x10, 38, 0, 4, 'M', 'Q', 'T', 'T', 4, 2, 0, 0x3c, 0, 26, 'P', 'y', 'y', '_', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};
