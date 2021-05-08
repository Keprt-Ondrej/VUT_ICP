#ifndef NEWTOPIC_H
#define NEWTOPIC_H

#include <QDialog>

namespace Ui {
class NewTopic;
}

class NewTopic : public QDialog
{
    Q_OBJECT

public:
    explicit NewTopic(QWidget *parent = nullptr);
    ~NewTopic();

private:
    Ui::NewTopic *ui;
};

#endif // NEWTOPIC_H
