/**
 * @file fulltextwindow.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Definition of class for displaying text data in new window
 * 
*/

#include "fulltextwindow.h"

fullTextWindow::fullTextWindow(QString &data, QString &path, QWidget *parent):
    QDialog(parent),
    ui(new Ui::fullTextWindow)    
{
    ui->setupUi(this);
    ui->topic->setText(data);
    ui->topicPath->setText(path);
}

fullTextWindow::~fullTextWindow()
{
    delete ui;
}

void fullTextWindow::on_Close_released()
{
    close();
}
