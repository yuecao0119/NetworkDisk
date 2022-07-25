#include "tcpclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    TcpClient w;
//    w.show();
    TcpClient::getInstance().show();

    return a.exec();
}
