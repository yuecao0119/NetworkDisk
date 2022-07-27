#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile> // 操作文件，在这里主要为了启动时访问配置文件
#include <QTcpSocket> // 建立TCP会话连接，为了实现TCP客户端连接服务器及与数据库交互
#include "operatewidget.h" // 主操作页面

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig(); // 加载配置文件信息

    static TcpClient& getInstance(); // 将TcpClient设置为单例模式，方便外部调用socket功能
    QTcpSocket& getTcpSocket();

    QString getStrName() const;
    void setStrName(const QString &strName);

    QString getStrCurPath() const;
    void setStrCurPath(const QString &strCurPath);

    QString getStrRootPath() const;
    void setStrRootPath(const QString &strRootPath);

public slots: // 槽函数，主要用来处理信号
    void showConnect(); // 检测服务器是否连接成功
    void receiveMsg();  // 接收服务器数据

private slots:
    // void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_logout_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;        // 存储配置文件读取到的IP地址
    quint16 m_usPort;       // 无符号16位整型 存储配置文件的端口号
    QTcpSocket m_tcpSocket; // TCPSocket用来和服务器连接与交互

    QString m_strName;      // 该客户端用户名
    QString m_strRootPath;  // 用户根目录
    QString m_strCurPath;   // 当前目录
};
#endif // TCPCLIENT_H
