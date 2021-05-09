/**
 * @file topichistory.h
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
 * 
 * @brief Declaration of class for displaying topic history in new window
 * 
*/

#ifndef TOPICHISTORY_H
#define TOPICHISTORY_H

#include "ui_topichistory.h"
#include <QDialog>
#include <QtCore>
#include <QtGui>
#include "tcp_client.h"

/**
 * @brief Declaration of class for displaying topic history in new window
 * 
 * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
*/
class TopicHistory : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Topic History object
     * 
     * @param dataHistory Reference to data for displaying
     * @param parent 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    explicit TopicHistory(QModelIndex &dataHistory,QWidget *parent = nullptr);   

    /**
     * @brief Destroy the Topic History object
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    ~TopicHistory();
    
private:
    Ui::TopicHistory *ui;               ///< Pointer to GUI
    QModelIndex &dataHistory;           ///< Reference to data for displaying
    QVector<QStandardItem*> versions;   ///< Vector on dynamiclly allocated items for every version

private slots:

    /**
     * @brief Close window
     * 
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_close_released();

    /**
     * @brief Display data of selected version
     * 
     * @param index Reference to data to display
     * @author Keprt Ondřej (xkeprt03@stud.fit.vutbr.cz)
    */
    void on_versions_doubleClicked(const QModelIndex &index);
};

#endif // TOPICHISTORY_H
