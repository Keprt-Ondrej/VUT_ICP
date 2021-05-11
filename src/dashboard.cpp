/**
 * @file dashboard.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Definition of class for displaying window with Dashboard
 * 
*/

#include "dashboard.h"
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include <QFile>
#include <QTextBrowser>
#include "mqtt_client.h"
#include <QMessageBox>
#include "newandedittopic.h"

DashBoard::DashBoard(MQTT_Client &mqtt,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DashBoard),
    thermometherPNG("icons/thermomether.png"),
    humidityPNG("icons/humidity.png"),
    wattPNG("icons/watt.png"),
    leverPNG("icons/lever.png"),
    detectorPNG("icons/detector.png"),
    relayPNG("icons/relay.png"),
    mqtt(mqtt)
{    
    connect(this, SIGNAL(dataChanged()), this, SLOT(updateGUI()));
    ui->setupUi(this); 
    connect(ui->update, SIGNAL(clicked()), this, SLOT(updateGUI()));      
    layout = qobject_cast<QVBoxLayout*>(ui->widgets_frame->layout());    
}

DashBoard::~DashBoard()
{       
    QLayoutItem *removeItem;
    foreach(QHBoxLayout* widgetCluster,mapDataToDisplay.keys()){         
        while (widgetCluster->count() != 0){
            removeItem = widgetCluster->takeAt(0);
            delete removeItem->widget();
            delete removeItem;
        }
        delete widgetCluster;  
    }
    delete ui;
}

void DashBoard::on_addWidget_released()
{        
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

    QLabel *icon = new QLabel("",nullptr);    
    icon->setMaximumSize(100,100);
    if (widgetType == "temperature"){
        icon->setPixmap(thermometherPNG);
    }
    else if (widgetType == "wattmeter"){
        icon->setPixmap(wattPNG);       
    }
    else if(widgetType == "humidity meter"){
        icon->setPixmap(humidityPNG);        
    }
    else if (widgetType == "relay"){
        icon->setPixmap(relayPNG);        
    }
    else if (widgetType == "blank"){
        icon->setStyleSheet("QLabel{background : transparent}");
    }
    else if (widgetType == "lever"){
        icon->setPixmap(leverPNG);
    } 
    else{
        icon->setPixmap(detectorPNG);
    }    
    newWidget->addWidget(icon);   
   
    
    QTextBrowser *widgetDescription = new QTextBrowser(nullptr);
    widgetDescription->setText(ui->description->toPlainText());    
    widgetDescription->setStyleSheet("QTextBrowser{background : transparent;text-align:center;}");    //widgetDescription->setStyleSheet("QTextBrowser{background : transparent}");
    widgetDescription->setMaximumSize(400,100);
    newWidget->addWidget(widgetDescription);
    
    QTextBrowser *data = new QTextBrowser(nullptr);       
    data->setStyleSheet("QTextBrowser{background : transparent;text-align:center;}");
    data->setText(dataIndex.data(6).toList().at(0).toString());
    data->setMaximumSize(1500,100);    
    newWidget->addWidget(data);   

    QPushButton* publisButton = new QPushButton("Publish",nullptr);  
    publisButton->setStyleSheet("QPushButton:pressed{	background-color: #00FF00}");
    mapPublishButtonToTopicPath.insert(publisButton,QTopicPath);
    connect(publisButton,&QPushButton::released,this,&DashBoard::updateTopic);   //SLOT(updateTopic(topicPath))      mapPublishButtonToTopicPath.value(publishButton))
    publisButton->setMaximumSize(100,30);    
    newWidget->addWidget(publisButton);

    QPushButton* removeButton = new QPushButton("Remowe widget",nullptr);  
    removeButton->setStyleSheet("QPushButton:pressed{	background-color: #00FF00}");
    QObject::connect(removeButton,&QPushButton::released,this,&DashBoard::removeWidget);        
    removeButton->setMaximumSize(120,30);
    mapButtonToLayout.insert(removeButton,newWidget);     //for proper deleting all widgets
    newWidget->addWidget(removeButton);

    mapDataToDisplay.insert(newWidget,dataIndex);  
    layout->insertLayout(0,newWidget);
}

void DashBoard::removeWidget()
{    
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QHBoxLayout* removeLayout = mapButtonToLayout.take(button);    
    QPushButton* publishButton = qobject_cast<QPushButton*>(removeLayout->itemAt(3)->widget());
    mapPublishButtonToTopicPath.take(publishButton);
    mapDataToDisplay.take(removeLayout);
    QLayoutItem *removeItem; 

    while (removeLayout->count() != 0){
        removeItem = removeLayout->takeAt(0);
        delete removeItem->widget();
        delete removeItem;
    }
    delete removeLayout;
}

void DashBoard::updateGUI(){
    qInfo() << "updating";
    foreach (QHBoxLayout* widgetCluster,mapDataToDisplay.keys()){
        QModelIndex dataIndex = mapDataToDisplay.value(widgetCluster);
        QTextBrowser* dataDisplay = qobject_cast<QTextBrowser*>(widgetCluster->itemAt(2)->widget());
        int type = dataIndex.data(5).toList().at(0).toInt();
        if (type == BIN){
            dataDisplay->setText("Cannot display binary data");
        }
        else{            
            dataDisplay->setText(dataIndex.data(6).toList().at(0).toString());            
        }
    }
}

void DashBoard::updateTopic(){
    //std::string path = topicPath.toUtf8().constData();
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString path = mapPublishButtonToTopicPath.value(button);
    QString data = "";
    NewAndEditTopic EditTopicWindow(mqtt,path,data, nullptr);
    EditTopicWindow.setModal(false);
    EditTopicWindow.setWindowFlags(Qt::Window);
    EditTopicWindow.setWindowTitle("Publish: "+path);
    EditTopicWindow.exec();
}