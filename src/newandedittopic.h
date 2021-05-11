/**
 * @file newandedittopic.h
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Declaration of class for editing or publishing new topic
 * 
*/

#ifndef NEWANDEDITTOPIC_H
#define NEWANDEDITTOPIC_H

#include <QDialog>
#include "ui_newandedittopic.h"

class MQTT_Client;  //forward declaration for proper compailing

/**
 * @brief Declaration of class for editing or publishing new topic
 * 
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
class NewAndEditTopic : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new NewAndEditTopic window when someone want publish edited data
     * 
     * @param mqtt Application shared data
     * @param path Topic path on server
     * @param data Topic data to edit
     * @param parent parent window
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit NewAndEditTopic(MQTT_Client &mqtt,QString &path,QString &data,QWidget *parent);

    /**
     * @brief Construct a new NewAndEditTopic window when someone want publish new data
     * 
     * @param mqtt Application shared data
     * @param parent parent window
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit NewAndEditTopic(MQTT_Client &mqtt,QWidget *parent);

    /**
     * @brief Destroy the NewAndEditTopic window
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    ~NewAndEditTopic();   

private:
    MQTT_Client &mqtt;          ///< Application shared data
    Ui::NewAndEditTopic *ui;    ///< Pointer to GUI

    QString fileName;           ///< Path to file selecteed from PC
    std::string fileMemblock;   ///< Data, which would be published

private slots:
    /**
     * @brief Open new window for selecting file from PC
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_selectFile_released();

    /**
     * @brief Send data to server
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_publish_released();
};

#endif // NEWANDEDITTOPIC_H
