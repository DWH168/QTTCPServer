#include "tcpserver.h"
#include "ui_tcpserver.h"
#include<QDebug>
/**
  *微信公众号：智慧小巷
  *可获取更多资料哦
  *
  */
TcpServer::TcpServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    tcpServer = new QTcpServer(this);
    //本地IP
    ui->ip_edit->setText("127.0.0.1");
    //此处设置默认端口
    ui->prot_edit->setText("777");
    //把发送按钮禁用
    ui->send_btn->setEnabled(false);

    //新连接
    connect(tcpServer,&QTcpServer::newConnection,[=](){

        currentClient = tcpServer->nextPendingConnection();
        tcpClient.append(currentClient);
        //获取IP地址与端口
        QString str=tr("%1:%2").arg(currentClient->peerAddress().toString().split("::ffff:")[1])\
                .arg(currentClient->peerPort());
        //下拉框添加元素
        ui->cbx_connection->addItem(str);
        //数据接收框返回连接成功
        ui->re_edit->append(QString("TcpClient:%1 已连接").arg(str));
        //当有数新据时
        connect(currentClient,&QTcpSocket::readyRead,[=](){
            //遍历所有客户端寻找有新数据的客户端
            for(int i=0; i<tcpClient.length(); i++)
            {
                //读数据
                QByteArray buffer = tcpClient[i]->readAll();
                //如果遍历到当前的客户端没有数据就直接跳到下一个
                if(buffer.isEmpty())    continue;

                static QString IP_Port, IP_Port_Pre;
                IP_Port = tr("[%1:%2]:").arg(tcpClient[i]->peerAddress().toString().split("::ffff:")[1])\
                                             .arg(tcpClient[i]->peerPort());

                //如果是新的客户端发消息回来就在接收框输出一个新的[ip:port]
                if(IP_Port != IP_Port_Pre)
                    ui->re_edit->append(IP_Port);

                //检测是否是HEX显示
                if(ui->ckb_hex_show->isChecked())
                {
                    QString ret(buffer.toHex().toUpper());
                    int len = ret.length()/2;
                    for(int j=1;j<len;j++)
                    {
                        qDebug()<<j;
                        ret.insert(2*j+j-1," ");
                    }
                    ui->re_edit->append(ret);
                }else{
                     ui->re_edit->append(QString::fromLocal8Bit(buffer));
                }

                IP_Port_Pre = IP_Port;
            }
        });
        //当有客户端断开连接时
        connect(currentClient,&QTcpSocket::disconnected,[=](){
            //遍历断开的客户
            for(int i=0; i<tcpClient.length(); i++)
            {
                if(tcpClient[i]->state() == QAbstractSocket::UnconnectedState)
                {
                    QString str=tr("%1:%2").arg(currentClient->peerAddress().toString().split("::ffff:")[1])\
                            .arg(currentClient->peerPort());
                    // 删除下拉框的元素
                    ui->cbx_connection->removeItem(ui->cbx_connection->findText(tr("%1:%2")\
                                          .arg(tcpClient[i]->peerAddress().toString().split("::ffff:")[1])\
                                          .arg(tcpClient[i]->peerPort())));
                    // 删除QList里面的元素
                     tcpClient[i]->destroyed();
                     tcpClient.removeAt(i);
                     //接收框输出断开连接的客户
                    ui->re_edit->append(QString("TcpClient:%1 已断开连接").arg(str));
                }
            }
         });
    });
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::on_btn_connect_clicked()
{
    //判断当前的状态是否是监听的状态
    if(ui->btn_connect->text()=="监听")
    {

        bool ok = tcpServer->listen(QHostAddress::Any,ui->prot_edit->text().toInt());
        if(ok)
        {
            ui->btn_connect->setText("断开");
            ui->send_btn->setEnabled(true);

        }else{
            qDebug()<<"该端口已经被监听";
            QMessageBox::warning(0,tr("错误："),QString("端口: %1 已经被占用").arg(ui->prot_edit->text()));
        }
    }
    else{
        //断开所有连接
         for(int i=0; i<tcpClient.length(); i++)
         {
             tcpClient[i]->disconnectFromHost();
             bool ok = tcpClient[i]->waitForDisconnected(1000);
             if(!ok)
             {
                  QMessageBox::warning(0,tr("错误："),QString("出现异常"));
             }
             tcpClient.removeAt(i);
         }
         tcpServer->close();
         ui->btn_connect->setText("监听");
         ui->send_btn->setEnabled(false);
    }
}
void TcpServer::on_send_btn_clicked()
{
    QString data = ui->send_edit->toPlainText();
     // 文本输入框为空时直接退出无需发送
    if(data == "")
    {
        QMessageBox::warning(0,tr("错误："),QString("请输入发送内容"));
        return;
    }
    //当下拉框没有选择任何客户机时就全部发送
    if(ui->cbx_connection->currentIndex() == 0)
    {
        for(int i=0; i<tcpClient.length(); i++)
            sendData(tcpClient[i],data);
    }
    //当有选中时就仅仅对这一台发送
    else
    {
        QString clientIP = ui->cbx_connection->currentText().split(":")[0];
        int clientPort = ui->cbx_connection->currentText().split(":")[1].toInt();
        for(int i=0; i<tcpClient.length(); i++)
        {
            if(tcpClient[i]->peerAddress().toString().split("::ffff:")[1]==clientIP\
                            && tcpClient[i]->peerPort()==clientPort)
            {
                sendData(tcpClient[i],data);
                return;
            }
        }
    }
}
/**
 *发送数据用 可发HEX数据
 * @brief TcpServer::sendData
 * @param client
 * @param data
 */
void  TcpServer::sendData(QTcpSocket *client,QString data){
        //如果发送HEX的选择框选中 那么就是需要发送HEX数据
        if(ui->ckb_hex_send->isChecked())
        {
            bool ok;
            QByteArray ret;
            data = data.trimmed();
            data = data.simplified();
            QStringList sl = data.split(" ");

            foreach (QString s, sl) {
                if(!s.isEmpty())
                {
                    char c = s.toInt(&ok,16)&0xFF;
                    if(ok){
                        ret.append(c);
                    }else{
                        QMessageBox::warning(0,tr("错误："),QString("非法的16进制字符: \"%1\"").arg(s));
                    }
                }
            }
            client->write(ret);
        }
        //直接发送字符串
        else{
            client->write(data.toLocal8Bit());
        }
}

void TcpServer::on_clear_btn_clicked()
{
    ui->re_edit->clear();
}


