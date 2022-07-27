#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QTcpServer> // 派生类实现客户端连接的监听和接收
#include <QList>      // 链表
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT // 类既要继承QObject又要写上宏Q_OBJECT，才能支持信号槽
private:
    MyTcpServer();
public:
    static MyTcpServer& getInstance(); // 实现单例模式获取静态对象的引用
    void incomingConnection(qintptr handle) override; // 判断何时有客户端接入并处理
    bool forwardMsg(const QString caDesName, PDU *pdu); // 服务器转发给其他客户端消息

    QString getStrRootPath() const;
    void setStrRootPath(const QString &strRootPath);

public slots:
    void deleteSocket(MyTcpSocket *mySocket); // 捕获mytcpsocket删除Socket信号并处理，注意槽函数与信号参数一致

private:
    QList<MyTcpSocket*> m_tcpSocketList; // 存储服务器所有已经建立的Socket连接
    QString m_strRootPath; // 文件系统根目录
};

#endif // MYTCPSERVER_H
