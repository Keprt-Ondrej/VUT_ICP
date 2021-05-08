#include "newtopic.h"
#include "ui_newtopic.h"

NewTopic::NewTopic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTopic)
{
    ui->setupUi(this);
}

NewTopic::~NewTopic()
{
    delete ui;
}
