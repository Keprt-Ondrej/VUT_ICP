#ifndef TOPICHISTORY_H
#define TOPICHISTORY_H

#include "ui_topichistory.h"
#include <QDialog>
#include <QtCore>
#include <QtGui>

namespace Ui {
class TopicHistory;
}

class TopicHistory : public QDialog
{
    Q_OBJECT

public:
    explicit TopicHistory(QModelIndex &dataHistory,QWidget *parent = nullptr);    
    ~TopicHistory();

private slots:
    void on_close_released();

    void on_versions_doubleClicked(const QModelIndex &index);

private:
    Ui::TopicHistory *ui;    
    QModelIndex &dataHistory;
    QVector<QStandardItem*> versions;   
};

#endif // TOPICHISTORY_H
