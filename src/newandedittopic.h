#ifndef NEWANDEDITTOPIC_H
#define NEWANDEDITTOPIC_H
#include "ui_newandedittopic.h"

#include <QDialog>

class MQTT_Client;

namespace Ui {
class NewAndEditTopic;
}

class NewAndEditTopic : public QDialog
{
    Q_OBJECT

public:
    explicit NewAndEditTopic(MQTT_Client &mqtt,QString &path,QString &data,QWidget *parent);    //for edit
    explicit NewAndEditTopic(MQTT_Client &mqtt,QWidget *parent);  //for new
    ~NewAndEditTopic();   

private:
    MQTT_Client &mqtt;    
    Ui::NewAndEditTopic *ui;

    QString fileName;
    std::string fileMemblock;

private slots:
    void on_selectFile_released();
    void on_publish_released();
};

#endif // NEWANDEDITTOPIC_H
