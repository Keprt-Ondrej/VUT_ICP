/**
 * @file topichistory.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Definition of class for displaying topic history in new window
 * 
*/
#include "topichistory.h"
#include "ui_topichistory.h"
#include "mainwindow.h"
#include "showbinarydata.h"

TopicHistory::TopicHistory(QModelIndex &dataHistory,QWidget *parent) :    
    QDialog(parent),
    ui(new Ui::TopicHistory),
    dataHistory(dataHistory)
{  
    ui->setupUi(this);   
    QStandardItemModel* model = nullptr;
    QStandardItem* item = nullptr;
    std::vector<QStandardItem*> pointers;
    try{
        model = new QStandardItemModel();     
        ui->path->setText(dataHistory.data(7).toString());

        QList<QVariant> dataType =  dataHistory.data(5).toList();
        QList<QVariant> data =  dataHistory.data(6).toList();    
        item = new QStandardItem("actual version"); 
        pointers.push_back(item);
        item->setData(dataType.at(0),5);
        item->setData(data.at(0),6);
        versions.push_back(item);    
        model->appendRow(item);
        int dataSize = data.size();
        for (int i = 1; i < dataSize;i++){
            item = new QStandardItem("version "+ QString::number(i));
            pointers.push_back(item);
            item->setData(dataType.at(i),5);
            item->setData(data.at(i),6);
            versions.push_back(item);
            model->appendRow(item);
        }
        
        ui->versions->setModel(model);
    }
    catch(...){
        delete model;        
        for(long unsigned int i = 0;i < pointers.size();i++){
            item = pointers[i];
            delete item;
        }
    }
}

TopicHistory::~TopicHistory()
{   
    qInfo() << "Destruktor Historie";
    int versionsSize = versions.size();  
    for (int i = 0; i < versionsSize;i++){
        delete versions[i];    
    }
    delete ui->versions;
    delete ui;
}

void TopicHistory::on_close_released()
{
    close();
}

void TopicHistory::on_versions_doubleClicked(const QModelIndex &index)
{   
    int dataType = index.data(5).toInt();
    if((dataType == STRING) || (dataType == JSON)){
        QString printData = index.data(6).toString();
        ui->topicTextView->setText(printData);
    }
    else{
        QByteArray data = index.data(6).toByteArray();
        ShowBinaryDataWindow(data);
        ui->topicTextView->setText("BINARY DATA");
    }    
}
