/**
 * @file connectserver.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Definition of class for connecting to new server
 * 
*/

#include "connectserver.h"
#include "mqtt_client.h"
#include <QMessageBox>
#include <iostream>

ConnectServer::ConnectServer(MQTT_Client &mqtt, QWidget *parent):
    QDialog(parent),
    ui(new Ui::ConnectServer),
    mqtt(mqtt)
{
    //this->setAttribute( Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    this->setWindowTitle("Connect server");
    connect(ui->Connect, SIGNAL(released()), this, SLOT(connectToServer()));
}

ConnectServer::~ConnectServer()
{
    delete ui;
}

void ConnectServer::connectToServer()
{
    // get data from GUI
    QString protocolQS = ui->Protocol->currentText();
    std::string protocol = protocolQS.toUtf8().constData();
    QString hostQS = ui->Host->text();
    std::string host = hostQS.toUtf8().constData();
    int port = ui->Port->text().toDouble();
    QString usernameQS = ui->Username->text();
    std::string username = usernameQS.toUtf8().constData();
    QString passwordQS = ui->Password->text();
    std::string password = passwordQS.toUtf8().constData();
    QString QTopics = ui->subscribeTopic->toPlainText();
    std::string topics = QTopics.toUtf8().constData();
    //std::cout << topics << std::endl;

    client_t info{
        host,
        port,
        username,
        password,
        "" //client_id
    };

    int result = mqtt.broker_connect(info); //connect to new server

    if (result == 0)
    {
        result = mqtt.mqtt_recv(10); //wait 10 seconds for connection acknowledge
        if(result == 0){
            bool success = true;
            result = mqtt.start_receiving();
            if(result != 0)
            {
                success = false;
            }

            int index = 0;
            for(unsigned int i = 0; i < topics.length(); i++) //subscribe to all given topics 
            {
                if(topics[i] == '\n')
                {
                    topics[i] = '\0';
                    result = mqtt.subscribe(&(topics.c_str()[index]));
                    if(result != 0)
                    {
                        success = false;
                        break;
                    }
                    index = i+1;
                }
            }
            if(index < topics.length())
            {
                result = mqtt.subscribe(&(topics.c_str()[index]));
                if(result != 0)
                {
                    success = false;
                }
            }

            if(success){
                close(); //success, close window
                return;
            }
        }
    }

    //fail to connect
    QMessageBox messageBox;
    messageBox.setFixedSize(500, 200);
    ///< \todo dodelat switch na ruzne error msg
    messageBox.critical(0, "Error", "Broker connection failed!");
}

void connectServerNewWindow(MQTT_Client &mqtt)
{
    ConnectServer connectServerWindow(mqtt);
    connectServerWindow.setModal(true);
    connectServerWindow.exec();
}