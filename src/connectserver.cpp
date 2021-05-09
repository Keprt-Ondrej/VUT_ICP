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
    QString protocolQS = ui->Protocol->currentText();
    std::string protocol = protocolQS.toUtf8().constData();
    QString hostQS = ui->Host->text();
    std::string host = hostQS.toUtf8().constData();
    int port = ui->Port->text().toDouble();
    QString usernameQS = ui->Username->text();
    std::string username = usernameQS.toUtf8().constData();
    QString passwordQS = ui->Password->text();
    std::string password = passwordQS.toUtf8().constData();
    client_t info{
        host,
        port,
        username,
        password,
        "" //client_id
    };
    int result = mqtt.broker_connect(info);

    if (result == 0)
    {
        close(); //uspech, zavrit okno
    }

    //neuspech:
    QMessageBox messageBox;
    messageBox.setFixedSize(500, 200);
    //TODO dodelat switch na ruzne error msg
    messageBox.critical(0, "Error", "An error has occured!");
}
