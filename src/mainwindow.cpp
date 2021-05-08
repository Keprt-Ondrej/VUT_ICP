#include "mainwindow.h"
#include "connectserver.h"
#include "topichistory.h"
#include "showbinarydata.h"

#include <iostream>

QModelIndex dEfAuLTgLOBAlInDeX = QModelIndex();

MainWindow::MainWindow(MQTT_Client &mqtt,QWidget *parent) :
  QMainWindow(parent),    
  ui(new Ui::MainWindow),  
  displayedData(dEfAuLTgLOBAlInDeX),
  sharedMqtt(mqtt) 
{
  
  ui->setupUi(this);
  this->setWindowTitle("MQTT EXPLORER");  
  //connectServerNewWindow();

  //https://stackoverflow.com/questions/1985936/creating-qt-models-for-tree-views
 

  QStandardItemModel* model = new QStandardItemModel(); 

  QStandardItem* item0 = new QStandardItem("1 first item");
  QStandardItem* item1 = new QStandardItem( "2 second item");
  QStandardItem* item3 = new QStandardItem("TESTOVACI DATA HERE");
  QStandardItem* item4 = new QStandardItem("4 forth item");

  QList<QVariant> my_list;
  QList<QVariant> my_list_types;  //BOOL nejde, protoze pak pi nesel ulozit do QVariant
  for (int i = 0;i < 1000;i++){
    my_list.push_front((double)i);
    my_list_types.push_front(BIN);
  }  

  item3->setData((bool)true,3);   
  item3->setData((bool)true,4);
  item3->setData(my_list_types,5);
  item3->setData(my_list,6);  
  item3->setData("/karel/sel/na/pomoc/do/bytu",7);

  item3->setForeground(QBrush(receivedColor));
  item4->setForeground(QBrush(sendColor));
 

  model->appendRow(item0);
  item0->appendRow(item3);
  //item0->appendRow(item9);
  item0->appendRow(item4);
  model->appendRow(item1);  
  

  QStandardItem* vytahnutej = model->item(0); 
  vytahnutej = vytahnutej->child(0);
  
/*
  QVariant strQ = vytahnutej->data(0);
  std::cout << "rowCount: "<<vytahnutej->rowCount() << std::endl;
  std::string karel = strQ.toString().toStdString();
  std::cout << "vytahnutej: " << karel << std::endl;
*/

  ui->TreeView->setModel(model); 
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

void ShowBinaryDataWindow(QByteArray &data){
  ShowBinaryData ShowBinaryDataWindow(data);
  ShowBinaryDataWindow.setModal(true);
  ShowBinaryDataWindow.setWindowFlags(Qt::Window);
  ShowBinaryDataWindow.setWindowTitle("Binary Data");
  ShowBinaryDataWindow.exec();  
}

void MainWindow::on_TopicShowInNewWindow_released()
{  
  if (!displayedData.isValid()){
    ui->TopicTextView->setText("Choose Topic!");
    return;
  }

  int type = displayedData.data(5).toList().at(0).toInt();
  if ((type == STRING) || (type == JSON)){
    //display text
    displayedData.data(6).toList().at(0);
  }
  else{
    QByteArray data = displayedData.data(6).toList().at(0).toByteArray();
    ShowBinaryDataWindow(data); 
  }  
}

void MainWindow::on_TopicHistory_released(){
  if (!displayedData.isValid()){
    ui->TopicTextView->setText("Choose Topic!");
    return;
  }
  TopicHistory TopicHistoryWindow(displayedData);
  TopicHistoryWindow.setModal(false);
  TopicHistoryWindow.setWindowFlags(Qt::Window);
  TopicHistoryWindow.setWindowTitle("Topic History");
  TopicHistoryWindow.exec();  
}

void MainWindow::on_TopicEdit_released()
{
    std::cout << "edit topic" << std::endl;
}

//TODO NAKONCI SMAZAT
/*
void MainWindow::AddRoot(QString name, QString Description){
  QTreeWidgetItem *item = new QTreeWidgetItem(ui->TreeWidget);
  item->setText(0,name);
  qInfo() << Description;
  ui->TreeWidget->addTopLevelItem(item);
}

void MainWindow::AddChild(QTreeWidgetItem *parent, QString name, QString Description){
  QTreeWidgetItem *item = new QTreeWidgetItem();
  item->setText(0,name);
  qInfo() << Description;
  parent->addChild(item);
}
*/

void MainWindow::on_TreeView_doubleClicked(const QModelIndex &index){   
  if (index.data(3).toBool()){ //je topic
    displayedData = const_cast<QModelIndex&>(index);
    QString stringData = ""; 
    /*data_type_t*/ int type = index.data(5).toList().at(0).toInt();     
    if ((type == STRING) || (type == JSON)){       
      ui->TopicTextView->setText(index.data(6).toList().at(0).toString());
      /*   
      or this way:
      QVariant raw = index.data(6);
      QList<QVariant> data = raw.toList();      
      stringData = stringData + data.at(0).toString(); 
      ui->TopicTextView->setText(stringData);
      */   
    } 
    else{      
      ui->TopicTextView->setText("Data are BINARY, use Show in new window button");
      QByteArray data = index.data(6).toList().at(0).toByteArray(); 
      ShowBinaryDataWindow(data); 
    }
  }
}
//https://www.youtube.com/watch?v=M0PZDrDwdHM  tree directory
