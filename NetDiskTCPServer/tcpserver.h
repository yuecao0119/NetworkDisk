#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig(); // 加载配置文件信息

private:
    Ui::TcpServer *ui;
    QString m_strIP; // 存储配置文件读取到的IP地址
    quint16 m_usPort; // 无符号16位整型 存储配置文件的端口号
};
#endif // TCPSERVER_H
