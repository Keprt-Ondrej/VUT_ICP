#include "newandedittopic.h"
#include "ui_newandedittopic.h"

NewAndEditTopic::NewAndEditTopic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAndEditTopic)
{
    ui->setupUi(this);
}

NewAndEditTopic::~NewAndEditTopic()
{
    delete ui;
}
