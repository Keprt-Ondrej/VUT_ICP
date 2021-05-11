/**
 * @file dashboard.h
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Declaration of class for displaying window with Dashboard
 * 
*/

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QDialog>
#include "ui_dashboard.h"
#include <QHash>
class MQTT_Client; // forward declaration for proper compiling


/**
 * @brief Declaration of class for displaying window with Dashboard
 * 
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
class DashBoard : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Dash Board object
     * 
     * @param mqtt reference to application data
     * @param parent parent window
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit DashBoard(MQTT_Client &mqtt,QWidget *parent = nullptr);
    ~DashBoard();

public slots:
    /**
     * @brief Update data displayed in GUI
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void updateGUI(); 

private slots:
    /**
     * @brief Create widget dinamically for topic
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_addWidget_released();
    
    /**
     * @brief Remove widget from dashboard
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void removeWidget();

    /**
     * @brief Open window for updating data for specified topic
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void publishTopic();

private:
    Ui::DashBoard *ui;                                          ///< Pointer to UI
    QVBoxLayout *layout;                                        ///< Pointer to layout, where widgets for topics are stored
    QHash<QPushButton*,QHBoxLayout*> mapButtonToLayout;         ///< Map button on widget layout for proper deleting all items in widget
    QPixmap thermometherPNG;                                    ///< Icon for widged thermomether                                    
    QPixmap humidityPNG;                                        ///< Icon for widged humidity meter   
    QPixmap wattPNG;                                            ///< Icon for widged wattmeter
    QPixmap leverPNG;                                           ///< Icon for widged lever/button
    QPixmap detectorPNG;                                        ///< Icon for widged detector   
    QPixmap relayPNG;                                           ///< Icon for widged relay
    MQTT_Client &mqtt;                                          ///< Reference on application shared memory 
    QHash<QHBoxLayout*,QModelIndex> mapDataToDisplay;           ///< Map index in memory, where are stored data for widget
    QHash<QPushButton*,QString> mapPublishButtonToTopicPath;    ///< Map widget button to path topic path    
};

#endif // DASHBOARD_H
