/**
 * @file newandedittopic.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Definition of class for editing or publishing new topic
 * 
*/

#include "newandedittopic.h"
#include <QFileDialog>  ///< \todo delete
#include <QDebug>
#include <iostream>
#include <QMessageBox>  
#include "mqtt_client.h"
#include <fstream>

NewAndEditTopic::NewAndEditTopic(MQTT_Client &mqtt,QWidget *parent) :    
    QDialog(parent),
    mqtt(mqtt),
    ui(new Ui::NewAndEditTopic)    
{
    ui->setupUi(this);
}

NewAndEditTopic::NewAndEditTopic(MQTT_Client &mqtt,QString &path,QString &data,QWidget *parent) :    
    QDialog(parent),
    mqtt(mqtt),    
    ui(new Ui::NewAndEditTopic)    
{
    ui->setupUi(this);
    ui->path->setText(path);
    ui->textData->setText(data);
}

NewAndEditTopic::~NewAndEditTopic()
{
    delete ui;
}

void NewAndEditTopic::on_publish_released()
{
    using namespace std;
    if (fileMemblock.size() == 0){
        QString tmpData = ui->textData->toPlainText();
        fileMemblock.resize(tmpData.size());
        fileMemblock = tmpData.toUtf8().constData();            
    }
    //cout << fileMemblock << endl;
    string path = ui->path->toPlainText().toUtf8().constData();
    bool retainOnServer = ui->retainOnServer->isChecked();
    uint8_t qualityOfService;
    QString qOs = ui->qos->currentText();
    if ("At most Once" == qOs){
        qualityOfService = 0;
    }
    else if ("At least Once" == qOs){
        qualityOfService = 1;
    }
    else{
        qualityOfService = 2;
    } 
    pubflg_t flags{
        false,              // bool DUP;    Duplicate
        qualityOfService,   //uint8_t QoS;  Quality of Service
        retainOnServer      //bool retain;  Store on server store
    };
    
    int result = mqtt.publish(path, fileMemblock, flags);
    if(result == 0){ 
        QMessageBox messageBox;
        messageBox.setFixedSize(700,200);
        messageBox.critical(0,"Succes","Data published");       
        close();        
    }
    else{
        QMessageBox messageBox;
        messageBox.setFixedSize(700,200);
        messageBox.critical(0,"Error","Data were not published"); 
    }
}

void NewAndEditTopic::on_selectFile_released()
{
    fileName = QFileDialog::getOpenFileName(this,"Publish file","");
    //qInfo() << fileName.size();   

    using namespace std;
    string fileDataInput = fileName.toUtf8().constData();    
    streampos size;

    ifstream file(fileDataInput);
    if (!file.is_open()) {
        QMessageBox messageBox;
        messageBox.setFixedSize(700,200);
        messageBox.critical(0,"Error","Could not open file:\n"+fileName);
    }
    else{
        size = file.tellg();
        fileMemblock.resize(size);
        fileMemblock = string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close(); 
        ui->textData->setText("Data loaded from file:\n"+fileName);
        ui->textData->setReadOnly(true);        
    }
}