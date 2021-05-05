#include "mainwindow.h"
#include "connectserver.h"


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
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
  connectServerNewWindow();
}
