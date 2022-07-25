#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance; // 由于是静态的，所以这个函数调用多次也只是创建一次
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    // 派生QTcpSocket，然后对Socket进行绑定相应的槽函数，这样就可以不同客户端由不同MyTcpSocket进行处理
    // 从而可以实现客户端连接和对应数据收发的socket的关联
    // qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket; // 建立新的socket连接
    pTcpSocket -> setSocketDescriptor(handle); // 设置其Socket描述符，不同描述符指示不同客户端
    m_tcpSocketList.append(pTcpSocket);

    // 关联socket用户下线信号与删除socket的槽函数
    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket *)), this, SLOT(deleteSocket(MyTcpSocket *)));
}

void MyTcpServer::deleteSocket(MyTcpSocket *mySocket)
{
    // 遍历m_tcpSocketList并删除socket
    QList<MyTcpSocket*>::iterator iter = m_tcpSocketList.begin();
    for(; iter != m_tcpSocketList.end(); iter ++)
    {
        if(mySocket == *iter)
        {
            (*iter) -> deleteLater(); // 延迟释放空间，使用delete会报错！！！
            *iter = NULL;
            m_tcpSocketList.erase(iter); // 删除列表中指针
            break;
        }
    }
    // 输出一下所有socket，看看是否删除成功  --- 测试
//    for(int i = 0; i < m_tcpSocketList.size(); ++ i)
//    {
//        QString tmp = m_tcpSocketList.at(i) -> getStrName();
//        qDebug() << tmp;
//    }
}
