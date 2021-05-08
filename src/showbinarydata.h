#ifndef SHOWBINARYDATA_H
#define SHOWBINARYDATA_H
#include "mainwindow.h"

#include <QDialog>
#include "ui_showbinarydata.h"

namespace Ui {
class ShowBinaryData;
}

class ShowBinaryData : public QDialog
{
    Q_OBJECT

public:
    explicit ShowBinaryData(QByteArray &data,QWidget *parent = nullptr);
    ~ShowBinaryData();

private:
    Ui::ShowBinaryData *ui;
    QByteArray &data;
};

#endif // SHOWBINARYDATA_H
