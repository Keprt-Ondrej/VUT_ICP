
#include "connectserver.h"
#include "shared.h"

ConnectServer::ConnectServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectServer)
{
    //this->setAttribute( Qt::WA_DeleteOnClose);
    ui->setupUi(this);    
    this->setWindowTitle("Connect server");
    connect(ui->Connect,SIGNAL(released()),this,SLOT(storeConnectionInfo()));
}

ConnectServer::~ConnectServer()
{
    delete ui;
}

void ConnectServer::storeConnectionInfo(){
    QString protocolQS = ui->Protocol->currentText();
    std::string protocol = protocolQS.toUtf8().constData();
    QString hostQS = ui->Host->text();    
    std::string host = hostQS.toUtf8().constData();
    int port = ui->Port->text().toDouble();
    QString usernameQS = ui->Username->text();
    std::string username = usernameQS.toUtf8().constData();
    QString passwordQS = ui->Password->text();
    std::string password = passwordQS.toUtf8().constData();
    activeShared->setConnectionInfo(protocol,host,port,username,password);
    close();
}
