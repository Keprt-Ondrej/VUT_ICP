#include "fulltextwindow.h"
#include "ui_fulltextwindow.h"

fullTextWindow::fullTextWindow(QString &data,QString &path,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fullTextWindow),
    data(data),
    path(path)
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
