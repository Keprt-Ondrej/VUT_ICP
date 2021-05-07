
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <memory> // unique_ptr
#include "mqtt_client.h"
#include <thread>
#include "shared.h"
#include "hello.cpp"
#include <QApplication>
#include "mainwindow.h"
#include "connectserver.h"


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

Shared *activeShared;
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

  
  	/* level00
	 *  | topic0
	 *  |
	 *  + level10
	 *  |  | topic2
	 *  |  | topic3
	 *  |
	 *  + level11
	 *  |  + level20
	 *  |  |  | topic4
	 *  |  |
	 *  |  | topic5
	 *  |
	 *  | topic1
	 *  | topic6
	 */
	std::string t0 = "10";
	std::string t1 = "True";
	std::string t2 = "420";
	std::string t3 = "69";
	std::string t4 = "dxfcgvbhjnmkl";
	std::string t5 = "fthn";
	TypedItem topic0("device", TOPIC, t0);
	TypedItem topic1("something_else", TOPIC, t1);
	TypedItem topic2("blazeit", TOPIC, t2);
	TypedItem topic3("nice", TOPIC, t3);
	TypedItem topic4("cghjk", TOPIC, t4);
	TypedItem topic5("jhgytr78uoikl", TOPIC, t5);
	TypedItem topic6("asdfg", TOPIC, t5);

	std::vector<Item*> main_v;
	std::vector<Item*> lvl10;
	std::vector<Item*> lvl11;
	std::vector<Item*> lvl20;
  
	Shared oof(level00);
	tree_print_recursive(oof.get_topics());
    std::cout << "==========================\n";

	Shared x;
	tree_print_recursive(x.get_topics());
    std::cout << "==========================\n";
	
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
        std::cout << x3.at(i).toDouble() << "\n";
	}
  
  Shared active(level00);
	activeShared = &active; //potrebuji nejaky shared navazat jinak SIGSEG
	hi();
		
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
//char c[] = {0x10, 38, 0, 4, 'M', 'Q', 'T', 'T', 4, 2, 0, 0x3c, 0, 26, 'P', 'y', 'y', '_', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};