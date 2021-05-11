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
     * @param parent parent window
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
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
     * @author Matúš Fabo (xfabom01@stud.fit.vutbr.cz)
    */
    void connectToServer();
};

/**
 * @brief Open new window to select new server
 * 
 * @param mqtt Reference on application shared data
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
void connectServerNewWindow(MQTT_Client &mqtt);

#endif // CONNECTSERVER_H
