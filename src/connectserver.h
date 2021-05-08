#ifndef CONNECTSERVER_H
#define CONNECTSERVER_H
#include "ui_connectserver.h"
#include "mqtt_client.h"
#include <QDialog>

namespace Ui {
class ConnectServer;
}

class ConnectServer : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectServer(MQTT_Client &mqtt,QWidget *parent = nullptr);
    ~ConnectServer();

private:
    Ui::ConnectServer *ui;
    MQTT_Client &mqtt;

private slots:
    void storeConnectionInfo();
};

#endif // CONNECTSERVER_H
