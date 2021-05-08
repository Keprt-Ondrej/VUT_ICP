#include "showbinarydata.h"
#include "ui_showbinarydata.h"
#include <QDebug>
#include <QFile>    //TODO SMAZAT

ShowBinaryData::ShowBinaryData(QByteArray &data,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowBinaryData),
    data(data)
{
    ui->setupUi(this);
    QFile CurrentFile("examples/cats.jpg");
    if(!CurrentFile.open(QIODevice::ReadOnly)) return;
    QByteArray DataFile = CurrentFile.readAll();
    //QPixmap pix("examples/cats.jpg");
    QPixmap pix(DataFile.size(),1);
    pix.loadFromData(DataFile);     

    //ready for data from server
    //QPixmap pix(data.size(),1);
    //pix.loadFromData(data); 
      
    ui->binDataView->setPixmap(pix);
}

ShowBinaryData::~ShowBinaryData()
{
    delete ui;
}
