/**
 * @file showbinarydata.h
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Declaration of class for displaying binary data in new window
 * 
*/
#ifndef SHOWBINARYDATA_H
#define SHOWBINARYDATA_H
#include "mainwindow.h"

#include <QDialog>
#include "ui_showbinarydata.h"

/**
 * @brief Declaration of class for displaying binary data in new window
 * 
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
class ShowBinaryData : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Show Binary Data object
     * 
     * @param data Reference to binary data
     * @param parent parent window
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit ShowBinaryData(QByteArray &data,QWidget *parent = nullptr);

    /**
     * @brief Destroy the Show Binary Data object
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    ~ShowBinaryData();

private:
    Ui::ShowBinaryData *ui; ///< Pointer to GUI
    QByteArray &data;       ///< Reference to binary data
};

/**
 * @brief Display binary data in new window
 * 
 * @sa Class for window ShowBinaryData
 * 
 * @param data reference to data, which should be displayed
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
void ShowBinaryDataWindow(QByteArray &data);

#endif // SHOWBINARYDATA_H
