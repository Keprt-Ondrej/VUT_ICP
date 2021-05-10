#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QDialog>
#include "ui_dashboard.h"
#include <QHash>
class MQTT_Client; // forward declaration for proper compiling

namespace Ui {
class DashBoard;
}

class DashBoard : public QDialog
{
    Q_OBJECT

public:
    explicit DashBoard(MQTT_Client &mqtt,QWidget *parent = nullptr);
    ~DashBoard();

private slots:
    void on_addWidget_released();
    void removeWidget();

private:
    Ui::DashBoard *ui; 
    QVBoxLayout *layout;  
    QHash<QPushButton*,QHBoxLayout*> mapButtonToLayout; 
    QPixmap thermometherPNG;
    MQTT_Client &mqtt;
};

#endif // DASHBOARD_H
