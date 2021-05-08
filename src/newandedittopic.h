#ifndef NEWANDEDITTOPIC_H
#define NEWANDEDITTOPIC_H
#include "ui_newandedittopic.h"

#include <QDialog>

namespace Ui {
class NewAndEditTopic;
}

class NewAndEditTopic : public QDialog
{
    Q_OBJECT

public:
    explicit NewAndEditTopic(QWidget *parent = nullptr);
    ~NewAndEditTopic();

private:
    Ui::NewAndEditTopic *ui;
};

#endif // NEWANDEDITTOPIC_H
