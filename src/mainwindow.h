#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

private slots:
  void connectServerNewWindow();    
  void on_actionConnect_server_triggered();
  void on_actionNew_Topic_triggered();
  void on_TopicShowInNewWindow_released();
  void on_TopicHistory_released();
  void on_TopicEdit_released();
};

#endif // MAINWINDOW_H
