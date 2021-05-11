/**
 * @file fulltextwindow.h
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Declaration of class for displaying text data in new window
 * 
*/

#ifndef FULLTEXTWINDOW_H
#define FULLTEXTWINDOW_H

#include <QDialog>
#include "ui_fulltextwindow.h"

/**
 * @brief Declaration of class for displaying text data in new window 
 * 
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
class fullTextWindow : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new full Text Window object
     * 
     * @param data Reference to text data, which should be displayed
     * @param path Reference to Topic path on the server
     * @param parent parent window
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit fullTextWindow(QString &data,QString &path,QWidget *parent = nullptr);

    /**
     * @brief Destroy the full Text Window object
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    ~fullTextWindow();

private:
    Ui::fullTextWindow *ui; ///< Pointer on GUI    

private slots:
    /**
     * @brief Close window
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_Close_released();
};

#endif // FULLTEXTWINDOW_H
