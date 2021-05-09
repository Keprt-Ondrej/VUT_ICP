/**
 * @file showbinarydata.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Definition of class for displaying binary data in new window
 * 
*/
#include "showbinarydata.h"

ShowBinaryData::ShowBinaryData(QByteArray &data,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowBinaryData),
    data(data)
{
    ui->setupUi(this);
    
    QPixmap pix(data.size(),1);
    pix.loadFromData(data);
    ui->binDataView->setPixmap(pix);
}

ShowBinaryData::~ShowBinaryData()
{
    delete ui;
}

void ShowBinaryDataWindow(QByteArray &data)
{
    ShowBinaryData ShowBinaryDataWindow(data);
    ShowBinaryDataWindow.setModal(true);
    ShowBinaryDataWindow.setWindowFlags(Qt::Window);
    ShowBinaryDataWindow.setWindowTitle("Binary Data");
    ShowBinaryDataWindow.exec();
}