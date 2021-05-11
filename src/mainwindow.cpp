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
#include "dashboard.h"
#include <QMessageBox>
#include <iostream>


QModelIndex dEfAuLTgLOBAlInDeX = QModelIndex();

MainWindow::MainWindow(MQTT_Client &mqtt, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    displayedData(dEfAuLTgLOBAlInDeX),
    sharedMqtt(mqtt)
{
    ui->setupUi(this);
    this->setWindowTitle("MQTT EXPLORER");
    tree_model = new QStandardItemModel();
    sharedMqtt.set_tree_root(tree_model);
    ui->TreeView->setModel(tree_model);
    simulate = ui->toolBar->actions().at(8);    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionConnect_server_triggered()
{
    connectServerNewWindow(sharedMqtt);
    
    tree_model = new QStandardItemModel();
    sharedMqtt.set_tree_root(tree_model);
    ui->TreeView->setModel(tree_model);
}

void MainWindow::on_actionNew_Topic_triggered()
{
    NewAndEditTopic NT(sharedMqtt, nullptr);
    NT.setModal(false);
    NT.setWindowFlags(Qt::Window);
    NT.setWindowTitle("New topic");
    NT.exec();
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

/*
 * data position in QStandardItem = :
 * all data posions are QVariant
 * data[0] = topic name
 * data[3] = bool topic/level  topic = true
 * data[4] = zdroj received = true
 * data[5] = QList<"data_type_t">
 * data[6] = QList<QVariant> data
 * data[7] = fullpath
*/
void MainWindow::on_TreeView_doubleClicked(const QModelIndex &index)
{
    if (index.data(3).toBool())
    { //is topic        
        //index.setForeground(QBrush(QColor(0,0,0)));
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

void MainWindow::on_actionDashboard_triggered()
{
    //DASHBOARD
    DashBoard dashBoard(sharedMqtt,nullptr);
    dashBoard.setModal(true);
    dashBoard.setWindowFlags(Qt::Window);
    dashBoard.setWindowTitle("Dash Board");  
    dashBoard.exec();   
}

void MainWindow::on_actionDisconnect_triggered()
{
    sharedMqtt.broker_disconnect();
    
}

void MainWindow::on_actionSimulate_triggered()
{
    sharedMqtt.start_stop_simulation();
    if(sharedMqtt.get_simulation_state())
        simulate->setText("Stop simulation");
    else simulate->setText("Start simulation");
}
