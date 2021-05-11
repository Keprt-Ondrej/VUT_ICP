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

/**
 * @brief Data inicialization and connecting to broker
 * 
 * @param argc Argument counter
 * @param argv Program arguments 
 *
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * @author Matúš Fabo (xfabom01@stud.fit.vutbr.cz)
 * 
 * @return int exit code
*/
int main(int argc, char *argv[]) {  
	QApplication a(argc, argv);

	MQTT_Client mqtt;

	connectServerNewWindow(mqtt);
	MainWindow w(mqtt);
	w.show();

	return a.exec();
}
//char c[] = {0x10, 38, 0, 4, 'M', 'Q', 'T', 'T', 4, 2, 0, 0x3c, 0, 26, 'P', 'y', 'y', '_', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y', '_', 'y', 'y', 'y', 'y', 'y', 'y', 'y', 'y'};
