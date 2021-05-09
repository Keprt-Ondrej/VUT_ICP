/**
 * @file connectserver.h
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Declaration of class for connecting to new server
 * 
*/

#ifndef CONNECTSERVER_H
#define CONNECTSERVER_H
#include "ui_connectserver.h"
#include "mqtt_client.h"
#include <QDialog>

/**
 * @brief Declaration of class for Connect to server window
 * 
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
class ConnectServer : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Connect Server object
     * 
     * @param mqtt reference to shared data
     * @param parent 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit ConnectServer(MQTT_Client &mqtt, QWidget *parent = nullptr);

    /**
     * @brief Destroy the Connect Server object
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    ~ConnectServer();

private:
    Ui::ConnectServer *ui; ///< pointer on GUI
    MQTT_Client &mqtt;     ///< shared data

private slots:
    /**
     * @brief Connect app to new server
     * 
     * @sa call function broker_connect
     * @todo rename function
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void connectToServer();
};

#endif // CONNECTSERVER_H
