#include "mainwindow.h"
#include "hello.cpp"
#include <QApplication>

int main(int argc, char *argv[])
{
  hi();
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
