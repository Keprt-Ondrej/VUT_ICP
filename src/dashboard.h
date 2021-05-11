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
     * @param parent 
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

signals:
    /**
     * @brief Signa
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void dataChanged();

private slots:
    void on_addWidget_released();
    void removeWidget();
    void updateTopic();

private:
    Ui::DashBoard *ui; 
    QVBoxLayout *layout;  
    QHash<QPushButton*,QHBoxLayout*> mapButtonToLayout; 
    QPixmap thermometherPNG;
    QPixmap humidityPNG;
    QPixmap wattPNG;
    QPixmap leverPNG;
    QPixmap detectorPNG;
    QPixmap relayPNG;
    MQTT_Client &mqtt;
    QHash<QHBoxLayout*,QModelIndex> mapDataToDisplay; 
    QHash<QPushButton*,QString> mapPublishButtonToTopicPath;   
};

#endif // DASHBOARD_H
