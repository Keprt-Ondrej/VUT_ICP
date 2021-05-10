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

private:
    QStandardItemModel* tree_model;

private slots:
    /**
     * @brief Open new window, where user can publish topic
     * @sa Class NewAndEditTopic for window class
     * @note Same window for edit topic
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_actionNew_Topic_triggered();

    /**
     * @brief Open displayed topic concetent in new window
     * @sa Class for window  fullTextWindow
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_TopicShowInNewWindow_released();

    /**
     * @brief Open history of displayed topic in new window
     * 
     * @sa Class for window ShowBinaryData
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_TopicHistory_released();

    /**
     * @brief Open displayed topic in edit window
     * 
     * @sa class NewAndEditTopic for window class
     * @note Same window for new topic, with filled data from topic
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

#endif // MAINWINDOW_H
