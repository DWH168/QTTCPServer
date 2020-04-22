#ifndef TCPSERVER_H
#define TCPSERVER_H
#pragma execution_character_set("utf-8")
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QMessageBox>
namespace Ui {
class TcpServer;
}

class TcpServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = 0);
    ~TcpServer();

private slots:
    void on_btn_connect_clicked();

    void on_clear_btn_clicked();

    void on_send_btn_clicked();

private:
    Ui::TcpServer *ui;
    void TcpServer::sendData(QTcpSocket *client,QString data);
    QTcpServer *tcpServer;
    QList<QTcpSocket*> tcpClient;
    QTcpSocket *currentClient;
};

#endif // TCPSERVER_H
