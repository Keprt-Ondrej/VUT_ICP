#include "topichistory.h"
#include "ui_topichistory.h"
#include "shared.h"

TopicHistory::TopicHistory(QModelIndex &dataHistory,QWidget *parent) :
    dataHistory(dataHistory),
    QDialog(parent),
    ui(new Ui::TopicHistory)
{  
    ui->setupUi(this);
    qInfo() << dataHistory.data(0);    //TODO SMAZAT    
    QStandardItemModel* model = new QStandardItemModel();     
    ui->path->setText(dataHistory.data(7).toString());

    QList<QVariant> dataType =  dataHistory.data(5).toList();
    QList<QVariant> data =  dataHistory.data(6).toList();    
    QStandardItem* item = new QStandardItem("actual version"); 
    item->setData(dataType.at(0),5);
    item->setData(data.at(0),6);
    versions.push_back(item);    
    model->appendRow(item);
    int dataSize = data.size();
    for (int i = 1; i < dataSize;i++){
        item = new QStandardItem("version "+ QString::number(i));
        item->setData(dataType.at(i),5);
        item->setData(data.at(i),6);
        versions.push_back(item);
        model->appendRow(item);
    }
    
    ui->versions->setModel(model);
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
        ui->topicTextView->setText("DATA JSOU BINARNI");
    }    
}
