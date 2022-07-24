#include "tcpserver.h"

#include <QApplication>
#include "dboperate.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TcpServer w;
    w.show();

    return a.exec();
}
