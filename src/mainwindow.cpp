#include "mainwindow.h"
#include "connectserver.h"
#include <iostream>


//https://www.youtube.com/watch?v=M0PZDrDwdHM  tree directory
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->setWindowTitle("MQTT EXPLORER");
  connectServerNewWindow();
  }

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::connectServerNewWindow(){
  ConnectServer connectServerWindow;
  connectServerWindow.setModal(true);
  connectServerWindow.exec();
}

void MainWindow::on_actionConnect_server_triggered()
{
    //CONNECT SERVER
    connectServerNewWindow();
}

void MainWindow::on_actionNew_Topic_triggered()
{
    //NEW TOPIC
    std::cout << "new topic button" << std::endl;

}

void MainWindow::on_TopicShowInNewWindow_released()
{
    std::cout << "new window button" << std::endl;
}

void MainWindow::on_TopicHistory_released()
{
   std::cout << "history topic" << std::endl;
}

void MainWindow::on_TopicEdit_released()
{
    std::cout << "edit topic" << std::endl;
}
