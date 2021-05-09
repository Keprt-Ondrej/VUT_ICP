/**
 * @file mainwindow.h
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Declaration of application main window
 * 
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "mqtt_client.h"

class MQTT_Client; // forward declaration for proper compiling

///\todo https://csfieldguide.org.nz/en/interactives/rgb-mixer/ color setup
const QColor receivedColor = QColor(250, 0, 0);
const QColor sendColor = QColor(0, 200, 0);

/**
 * @brief Main functionality of application GUI
 * 
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Main Window object
     * 
     * @param mqtt shared data
     * @param parent 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit MainWindow(MQTT_Client &mqtt, QWidget *parent = 0);

    /**
     * @brief Destroy the Main Window object
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    ~MainWindow();

public:
    Ui::MainWindow *ui;         ///< pointer on UI
    QModelIndex &displayedData; ///< brief Index to data storage, that are now displayed
    MQTT_Client &sharedMqtt;    ///< reference to application data

private slots:
    /**
     * @brief Open new window, where user can publish topic
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_actionNew_Topic_triggered();

    /**
     * @brief Open displayed topic concetent in new window
     * @note Same window for edit topic
     * @sa class NewAndEditTopic for window class
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_TopicShowInNewWindow_released();

    /**
     * @brief Open history of displayed topic in new window
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_TopicHistory_released();

    /**
     * @brief Open displayed topic in edit window
     * 
     * @note Same window for new topic, with filled data from topic
     * @sa class NewAndEditTopic for window class
     *
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_TopicEdit_released();

    /**
     * @brief Displays data content of topic
     * 
     * @param index position in data structure, on which we clicked 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_TreeView_doubleClicked(const QModelIndex &index);

    /**
     * @brief Open new window to select new server 
     * 
     * @sa class ConnectServer for window class
     * @sa Call function connectServerNewWindow
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_actionConnect_server_triggered();

    /**
     * @brief Open dashboard
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_actionDashboard_triggered();
};

/**
 * @brief Display binary data in new window
 * 
 * @param data reference to data, which should be displayed
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
void ShowBinaryDataWindow(QByteArray &data);

/**
 * @brief Open new window to select new server
 * 
 * @param mqtt shared data
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
void connectServerNewWindow(MQTT_Client &mqtt);

#endif // MAINWINDOW_H
