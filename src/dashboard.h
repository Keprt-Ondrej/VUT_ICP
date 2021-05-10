#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QDialog>
#include "ui_dashboard.h"
#include <QHash>
class MQTT_Client; // forward declaration for proper compiling

class DashBoard : public QDialog
{
    Q_OBJECT

public:
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
    MQTT_Client &mqtt;
    QHash<QHBoxLayout*,QModelIndex> mapDataToDisplay; 
    QHash<QPushButton*,QString> mapPublishButtonToTopicPath;   
};

#endif // DASHBOARD_H
