#include "tcpclient.h"

#include <QApplication>
#include "operatewidget.h" // 测试主操作页面
#include "onlineuserwid.h" // 测试所有在线用户页面
#include "friend.h" // 测试好友页面

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    TcpClient w;
    OperateWidget w; // 测试主操作页面
//    OnlineUserWid w; // 测试所有在线用户页面
//    Friend w; // 测试好友页面

    w.show();

    return a.exec();
}
