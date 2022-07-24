#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"  // 服务器需要按照相同协议形式处理数据
#include "dboperate.h" // 操作数据库

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getStrName();

signals:
    void offline(MyTcpSocket *socket); // 通过信号传送给mytcpserver用户下线通知，然后附带参数socket地址方便删除

public slots:
    void receiveMsg(); // 槽函数，按照协议形式处理传输过来的数据
    void handleClientOffline(); // 接收客户端下线信号并处理

private:
    QString m_strName; // 用户名，为了能区分该Socket属于哪个用户
};

#endif // MYTCPSOCKET_H
