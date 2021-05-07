#include "mainwindow.h"
#include "connectserver.h"
#include "topichistory.h"

#include "shared.h"
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
    my_list_types.push_front(STRING);
  }  

  item3->setData((bool)true,3);   
  item3->setData((bool)true,4);
  item3->setData(my_list_types,5);
  item3->setData(my_list,6);  
  item3->setData("/karel/sel/na/pomoc/do/bytu",7);

  item3->setForeground(QBrush(receivedColor));
  item4->setForeground(QBrush(sendColor));

  if (item3->data(4).toBool()){
    std::cout << "Hell YEAH\n";
  }
  else {
    std::cout << "Hell NO\n";
  }

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

  //std::cout << "KAREEEEEEEEL" << my_karel[1] << std::endl;

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

void MainWindow::on_TopicHistory_released(){
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
      stringData = "Data nejsou string";
      ui->TopicTextView->setText(stringData);
    }
  }
  qInfo() << "Tree value: " << index.data(0); //vraci jmeno TODO SMAZAT
}
//https://www.youtube.com/watch?v=M0PZDrDwdHM  tree directory