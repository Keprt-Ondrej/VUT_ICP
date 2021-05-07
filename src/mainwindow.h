#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtGui>

//https://csfieldguide.org.nz/en/interactives/rgb-mixer/ TODO color setup

const QColor receivedColor = QColor(250,0,0);
const QColor sendColor = QColor(0,200,0);

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(std::vector<int> &karel,QWidget *parent = 0);
  ~MainWindow();

public:  
  Ui::MainWindow *ui;
  QModelIndex &displayedData;
  std::vector<int> & my_karel;  //TODO change to shared

private slots: 
  void connectServerNewWindow();
  void on_actionConnect_server_triggered();
  void on_actionNew_Topic_triggered();
  void on_TopicShowInNewWindow_released();
  void on_TopicHistory_released();
  void on_TopicEdit_released();
  void on_TreeView_doubleClicked(const QModelIndex &index);
};


#endif // MAINWINDOW_H
