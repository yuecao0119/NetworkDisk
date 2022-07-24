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

public slots:
    void receiveMsg(); // 槽函数，按照协议形式处理传输过来的数据
};

#endif // MYTCPSOCKET_H
