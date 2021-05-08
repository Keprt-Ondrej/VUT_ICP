#ifndef FULLTEXTWINDOW_H
#define FULLTEXTWINDOW_H

#include <QDialog>

namespace Ui {
class fullTextWindow;
}

class fullTextWindow : public QDialog
{
    Q_OBJECT

public:
    explicit fullTextWindow(QString &data,QString &path,QWidget *parent = nullptr);
    ~fullTextWindow();

private slots:
    void on_Close_released();

private:
    Ui::fullTextWindow *ui;
    QString &data;
    QString &path;
};

#endif // FULLTEXTWINDOW_H
