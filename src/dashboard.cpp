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
#include <QSpacerItem>
#include <QMessageBox>
#include <string>   
#include <QLineEdit> 

DashBoard::DashBoard(MQTT_Client &mqtt,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DashBoard),
    thermometherPNG("examples/thermomether.png"),
    mqtt(mqtt)
{
    ui->setupUi(this);   
    layout = qobject_cast<QVBoxLayout*>(ui->widgets_frame->layout());

    mqtt.dashBoardGUI = this;

    mojelajna = new QLineEdit("Konec",nullptr);
    mojelajna->setObjectName("mojelajna");
    layout->addWidget(mojelajna);

   
}

DashBoard::~DashBoard()
{
    delete ui;
}


void DashBoard::on_addWidget_released()
{    
    //QModelIndex topic_find(std::string& topic);
    QString QTopicPath = ui->topicPath->text();
    std::string topicPath = QTopicPath.toUtf8().constData();
    QModelIndex dataIndex = mqtt.topic_find(topicPath);    
    if (!dataIndex.data(3).toBool())    //errors
    {
        QMessageBox messageBox;
        messageBox.setFixedSize(700,200);
        messageBox.critical(0,"Error","Topic not found");
        return; 
    }

    int type = dataIndex.data(5).toList().at(0).toInt();
    if (type == BIN){
        QMessageBox messageBox;
        messageBox.setFixedSize(700,200);
        messageBox.critical(0,"Error","Cannot display binary data");
        return;
    }

    QString widgetType = ui->widgetType->currentText();
    QHBoxLayout *newWidget = new QHBoxLayout(nullptr); 

    QLabel *icon = new QLabel("Teplomeros",nullptr);    
    icon->setMaximumSize(150,150);
    if (widgetType == "temperature"){
        icon->setPixmap(thermometherPNG);
    }
    else if (widgetType == "wattmeter"){
        icon->setPixmap(thermometherPNG);
        icon->setStyleSheet("QLabel{background-color: #00FF50}");
    }
    else if(widgetType == "humidity meter"){
        icon->setPixmap(thermometherPNG);
        icon->setStyleSheet("QLabel{background-color: #50FF00}");
    }
    else if (widgetType == "relay"){
        icon->setPixmap(thermometherPNG);
        icon->setStyleSheet("QLabel{background-color: #00FF50}");
    }
    else if (widgetType == "blank"){
        icon->setStyleSheet("QLabel{background : transparent}");
    }
    newWidget->addWidget(icon);   
   
    
    QTextBrowser *widgetDescription = new QTextBrowser(nullptr);
    widgetDescription->setText(ui->widgetDescription->text());    
    widgetDescription->setStyleSheet("QTextBrowser{background-color: #00FF00}");    //widgetDescription->setStyleSheet("QTextBrowser{background : transparent}");
    widgetDescription->setMaximumSize(150,100);
    newWidget->addWidget(widgetDescription);

    QTextBrowser *data = new QTextBrowser(nullptr);    
    data->setStyleSheet("QTextBrowser{	background-color: #00FF00}");
    data->setText(dataIndex.data(6).toList().at(0).toString());
    qInfo() << data; 
    newWidget->addWidget(data);    

    QPushButton* button = new QPushButton("Remowe widget",nullptr);  
    button->setStyleSheet("QPushButton:pressed{	background-color: #00FF00}");
    QObject::connect(button,&QPushButton::released,this,&DashBoard::removeWidget);        
    button->setMaximumSize(120,30);
    mapButtonToLayout.insert(button,newWidget);     //for proper deleting all widgets
    newWidget->addWidget(button);
    mapDataToDisplay.insert(dataIndex,newWidget);    

    layout->insertLayout(0,newWidget);  
    
    QVariant num = layout->count();
    mojelajna->setText(num.toString());
    foreach (QPushButton* item,mapButtonToLayout.keys()){
        qInfo() << item << mapButtonToLayout.value(button);
    } 
    
}

void DashBoard::removeWidget()
{    
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QHBoxLayout* removeLayout = mapButtonToLayout.take(button);    
    QLayoutItem *removeItem; 

    QTextBrowser* pls = qobject_cast<QTextBrowser*>(removeLayout->itemAt(2)->widget());
    pls->setText("koncim tady");
    
    while (removeLayout->count() != 0){
        removeItem = removeLayout->takeAt(0);
        delete removeItem->widget();
        delete removeItem;
    }

    delete removeLayout;  
    
}

void DashBoard::updateGUI(){
    foreach (QModelIndex dataIndex,mapDataToDisplay.keys()){
        QHBoxLayout* widgetCluster = mapDataToDisplay.value(dataIndex);
        QTextBrowser* dataDisplay = qobject_cast<QTextBrowser*>(widgetCluster->itemAt(2)->widget());
        int type = dataIndex.data(5).toList().at(0).toInt();
        if (type == BIN){
            dataDisplay->setText("Cannot display binnary data");
        }
        else{
            dataDisplay->setText(dataIndex.data(6).toList().at(0).toString());
        }
    }
}  