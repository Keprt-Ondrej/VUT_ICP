#include "dashboard.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>
#include <QLabel>
#include <QFile>
#include <QTextBrowser>
#include "mqtt_client.h"

DashBoard::DashBoard(MQTT_Client &mqtt,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DashBoard),
    thermometherPNG("examples/thermomether.png"),
    mqtt(mqtt)
{
    ui->setupUi(this);   
    layout = qobject_cast<QVBoxLayout*>(ui->widgets_frame->layout());
}

DashBoard::~DashBoard()
{
    delete ui;
}


void DashBoard::on_addWidget_released()
{    
    QHBoxLayout *newWidget = new QHBoxLayout(nullptr);

    QPushButton* button = new QPushButton("Remowe widget",nullptr);  
    button->setStyleSheet("QPushButton:pressed{	background-color: #00FF00; min-width: 80px; max-widith: 80px;min-height: 80px; max-height: 80px}");
    QObject::connect(button,&QPushButton::released,this,&DashBoard::removeWidget);
    mapButtonToLayout.insert(button,newWidget);
    button->resize(800,800);    

    QLabel *icon = new QLabel("",nullptr);
    icon->resize(800,800);
    icon->setPixmap(thermometherPNG);

    QTextBrowser *data = new QTextBrowser(nullptr);

    
    newWidget->addWidget(icon);
    newWidget->addWidget(data); 
    newWidget->addWidget(button);
    
    layout->insertLayout(0,newWidget);  

    foreach (QPushButton* item,mapButtonToLayout.keys()){
        qInfo() << item << mapButtonToLayout.value(button);
    } 


}

void DashBoard::removeWidget()
{    
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QHBoxLayout* removeLayout = mapButtonToLayout.take(button);    
    QLayoutItem *removeItem;
    while (removeLayout->count() != 0){
        removeItem = removeLayout->takeAt(0);
        delete removeItem->widget();
        delete removeItem;
    }

    delete removeLayout;   
}