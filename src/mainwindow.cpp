/**
 * @file mainwindow.cpp
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Definition of application main window
 * 
 */

#include "mainwindow.h"
#include "connectserver.h"
#include "topichistory.h"
#include "showbinarydata.h"
#include "fulltextwindow.h"
#include "newandedittopic.h"
#include <QMessageBox>
#include <iostream>

QModelIndex dEfAuLTgLOBAlInDeX = QModelIndex();

MainWindow::MainWindow(MQTT_Client &mqtt, QWidget *parent) : QMainWindow(parent),
                                                             ui(new Ui::MainWindow),
                                                             displayedData(dEfAuLTgLOBAlInDeX),
                                                             sharedMqtt(mqtt)
{

    ui->setupUi(this);
    this->setWindowTitle("MQTT EXPLORER");
    //connectServerNewWindow();

    //https://stackoverflow.com/questions/1985936/creating-qt-models-for-tree-views

    QStandardItemModel *model = new QStandardItemModel();
    sharedMqtt.set_tree_root(model);

    QStandardItem *item0 = new QStandardItem("1 first item");
    QStandardItem *item1 = new QStandardItem("2 second item");
    QStandardItem *item3 = new QStandardItem("TESTOVACI DATA HERE");
    QStandardItem *item4 = new QStandardItem("Cute kittens");

    QList<QVariant> my_list;
    QList<QVariant> my_list_types; //BOOL nejde, protoze pak by nesel ulozit do QVariant
    for (int i = 0; i < 1000; i++)
    {
        my_list.push_front((double)i);
        my_list_types.push_front(STRING);
    }

    item3->setData((bool)true, 3);
    item3->setData((bool)true, 4);
    item3->setData(my_list_types, 5);
    item3->setData(my_list, 6);
    item3->setData("/karel/sel/na/pomoc/do/bytu", 7);
    item3->setForeground(QBrush(receivedColor));

    QList<QVariant> my_list1;
    QList<QVariant> my_list_types1;
    my_list_types1.push_front(BIN);

    QFile CurrentFile("examples/cats.jpg");
    if (!CurrentFile.open(QIODevice::ReadOnly))
        return;
    QByteArray DataFile = CurrentFile.readAll();
    QVariant binaryImage = DataFile;
    my_list1.push_front(binaryImage);
    item4->setData((bool)true, 3);
    item4->setData((bool)true, 4);
    item4->setData(my_list_types1, 5);
    item4->setData(my_list1, 6);
    item4->setForeground(QBrush(sendColor));
    item4->setData("/karel/sel/na/pomoc/do/bytu", 7);

    model->appendRow(item0);
    item0->appendRow(item3);
    //item0->appendRow(item9);
    item0->appendRow(item4);
    model->appendRow(item1);

    QStandardItem *vytahnutej = model->item(0);
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

void connectServerNewWindow(MQTT_Client &mqtt)
{
    ConnectServer connectServerWindow(mqtt);
    connectServerWindow.setModal(true);
    connectServerWindow.exec();
}

void MainWindow::on_actionConnect_server_triggered()
{
    connectServerNewWindow(sharedMqtt);
}

void MainWindow::on_actionNew_Topic_triggered()
{
    NewAndEditTopic NT(sharedMqtt, nullptr);
    //qInfo() << NT;
    NT.setModal(false);
    NT.setWindowFlags(Qt::Window);
    NT.setWindowTitle("New topic");
    NT.exec();
}

void ShowBinaryDataWindow(QByteArray &data)
{
    ShowBinaryData ShowBinaryDataWindow(data);
    ShowBinaryDataWindow.setModal(true);
    ShowBinaryDataWindow.setWindowFlags(Qt::Window);
    ShowBinaryDataWindow.setWindowTitle("Binary Data");
    ShowBinaryDataWindow.exec();
}

void MainWindow::on_TopicShowInNewWindow_released()
{
    if (!displayedData.isValid())
    {
        ui->TopicTextView->setText("Choose Topic!");
        return;
    }

    int type = displayedData.data(5).toList().at(0).toInt();
    if ((type == STRING) || (type == JSON))
    {
        QString data = displayedData.data(6).toList().at(0).toString();
        QString path = displayedData.data(7).toString();

        fullTextWindow ft(data, path);
        ft.setModal(true);
        ft.setWindowFlags(Qt::Window);
        ft.setWindowTitle("Topic");
        ft.exec();
    }
    else
    {
        QByteArray data = displayedData.data(6).toList().at(0).toByteArray();
        ShowBinaryDataWindow(data);
    }
}

void MainWindow::on_TopicHistory_released()
{
    if (!displayedData.isValid())
    {
        ui->TopicTextView->setText("Choose Topic!");
        return;
    }
    TopicHistory TopicHistoryWindow(displayedData, this);
    TopicHistoryWindow.setModal(false);
    TopicHistoryWindow.setWindowFlags(Qt::Window);
    TopicHistoryWindow.setWindowTitle("Topic History");
    TopicHistoryWindow.exec();
}

void MainWindow::on_TopicEdit_released()
{
    if (!displayedData.isValid())
    {
        ui->TopicTextView->setText("Choose Topic!");
        return;
    }
    QString path = displayedData.data(7).toString();
    int type = displayedData.data(5).toList().at(0).toInt();
    QString tmpData = ""; //for error handler and if data were binary
    QString &data = tmpData;
    if ((type == STRING) || (type == JSON))
    {
        data = displayedData.data(6).toList().at(0).toString();
    }
    else
    {
        data = tmpData;
    }
    NewAndEditTopic ET(sharedMqtt, path, data, nullptr);
    ET.setModal(false);
    ET.setWindowFlags(Qt::Window);
    ET.setWindowTitle("Edit topic");
    ET.exec();
}

void MainWindow::on_TreeView_doubleClicked(const QModelIndex &index)
{
    if (index.data(3).toBool())
    { //je topic
        displayedData = const_cast<QModelIndex &>(index);
        QString stringData = "";
        /*data_type_t*/ int type = index.data(5).toList().at(0).toInt();
        if ((type == STRING) || (type == JSON))
        {
            ui->TopicTextView->setText(index.data(6).toList().at(0).toString());
            /*   
      or this way:
      QVariant raw = index.data(6);
      QList<QVariant> data = raw.toList();      
      stringData = stringData + data.at(0).toString(); 
      ui->TopicTextView->setText(stringData);
      */
        }
        else
        {
            ui->TopicTextView->setText("BINARY DATA, use Show in new window button");
            QByteArray data = index.data(6).toList().at(0).toByteArray();
            ShowBinaryDataWindow(data);
        }
    }
}
//https://www.youtube.com/watch?v=M0PZDrDwdHM  tree directory

void MainWindow::on_actionDashboard_triggered()
{
    //DASHBOARD
    QMessageBox messageBox;
    messageBox.setFixedSize(500, 200);
    messageBox.critical(0, "Error", "Not implemented");
}
