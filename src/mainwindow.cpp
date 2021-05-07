#include "mainwindow.h"
#include "connectserver.h"

#include "shared.h"
#include <iostream>


QTreeView* displayedTree;
QStandardItem* globalitem;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->setWindowTitle("MQTT EXPLORER");
  connectServerNewWindow();

  //https://stackoverflow.com/questions/1985936/creating-qt-models-for-tree-views
 

  QStandardItemModel* model = new QStandardItemModel();  

  QStandardItem* item0 = new QStandardItem(QIcon("test.png"), "1 first item");
  QStandardItem* item1 = new QStandardItem(QIcon("test.png"), "2 second item");
  QStandardItem* item3 = new QStandardItem(QIcon("test.png"), "3 third item");
  QStandardItem* item4 = new QStandardItem("4 forth item");

  QList<QVariant> my_list;
  for (int i = 0;i < 1000;i++){
    my_list.push_front((double)i);
  }

  globalitem = item3;

  item3->setData(my_list,3);
  item3->setData((bool)false,4);
  item3->setData((bool)true,5);
  item3->setData(STRING,6);
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
  

  displayedTree = ui->TreeView;
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
/*
  QStandardItem* item0 = new QStandardItem(QIcon("test.png"), "Prosim funguj");
  QStandardItem* item1 = new QStandardItem(QIcon("test.png"), "Zaplatim ti");
  QStandardItem* item3 = new QStandardItem(QIcon("test.png"), "zlatem");
  QStandardItem* item4 = new QStandardItem("4 forth item"); 


  QStandardItemModel* model = new QStandardItemModel();


  model->appendRow(item0);
  item0->appendRow(item3);
  //item0->appendRow(item9);
  item0->appendRow(item4);
  model->appendRow(item1);  
  
  displayedTree->setModel(model);
  */

  globalitem->setData("NOVE JMENO",0);
  std::cout << "was here\n";
 

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

void MainWindow::on_TreeView_expanded(const QModelIndex &index)
{
    qInfo() << index.data(0);
    std::cout << "Here we COlapsed\n";
}

void MainWindow::on_TreeView_doubleClicked(const QModelIndex &index)
{
  QString displayData = "";
  if (index.data(6).toInt() == STRING){
    QList<QVariant> data = index.data(3).toList();
    for(int i = 0;i< data.size();i++){
      displayData = displayData + data.at(i).toString() + "\n";
    }
    ui->TopicTextView->setText(displayData);
  }
  else{
    displayData = "Data nejsou string";
    ui->TopicTextView->setText(displayData);
  }
  
  std::cout << "double click\n";
  qInfo() << "======================";
  qInfo() << index.data(1); //vraci jmeno
}


//https://www.youtube.com/watch?v=M0PZDrDwdHM  tree directory