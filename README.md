## 基于QT的TCP通讯TCPServer服务端C++ 代码开源

TCP服务端,如需客户端请点击[基于QT的TCP客户端](https://blog.csdn.net/dwh1314/article/details/105686751)
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200422133112524.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2R3aDEzMTQ=,size_16,color_FFFFFF,t_70)
Qt是一种基于C++的跨平台图形用户界面应用程序开发框架
本次使用QT的版本是5.6

TCP旨在适应支持多网络应用的分层协议层次结构。 连接到不同但互连的计算机通信网络的主计算机中的成对进程之间依靠TCP提供可靠的通信服务。TCP假设它可以从较低级别的协议获得简单的，可能不可靠的数据报服务。 原则上，TCP应该能够在从硬线连接到分组交换或电路交换网络的各种通信系统之上操作。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200422132604385.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2R3aDEzMTQ=,size_16,color_FFFFFF,t_70)
可以实现16进制的接收显示以及16进制的发送。
可同时向所有客户机发送消息或单独一台客户机发送消息
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200422133531151.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2R3aDEzMTQ=,size_16,color_FFFFFF,t_70)
需要在pro文件添加

```bash
QT       += network
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/2020042213551334.png)
数据发送
```cpp
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
```
数据接收

```cpp
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
```
界面：
![在这里插入图片描述](https://img-blog.csdnimg.cn/2020042213381780.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2R3aDEzMTQ=,size_16,color_FFFFFF,t_70)
更多资源欢迎关注微信公众号:**智慧小巷**
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200326102352938.jpg)

