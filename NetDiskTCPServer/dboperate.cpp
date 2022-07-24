#include "dboperate.h"
#include <QMessageBox>
#include <QDebug>

DBOperate::DBOperate(QObject *parent) : QObject(parent)
{
    // 连接数据库
    m_db = QSqlDatabase::addDatabase("QMYSQL"); // 连接的数据库类型

}

DBOperate &DBOperate::getInstance()
{
    static DBOperate instance;
    return instance;
}

// 数据库连接
void DBOperate::init()
{
    m_db.setHostName("localhost");         // 数据库服务器IP
    m_db.setUserName("root");              // 数据库用户名
    m_db.setPassword("root");              // 数据库密码
    m_db.setDatabaseName("networkdiskdb"); // 数据库名
    if(m_db.open()) // 数据库是否打开成功
    {
        QSqlQuery query;
        query.exec("select * from userInfo");
        while(query.next())
        {
            QString data = QString("%1, %2, %3, %4").arg(query.value(0).toString()).arg(query.value(1).toString())
                    .arg(query.value(2).toString()).arg(query.value(3).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(NULL, "数据库打开", "数据库打开失败");
    }
}

DBOperate::~DBOperate()
{
    m_db.close(); // 关闭数据库连接
}

bool DBOperate::handleRegist(const char *name, const char *pwd)
{
    // 考虑极端情况
    if(NULL == name || NULL == pwd)
    {
        return false;
    }
    // 数据插入数据库
    QString strQuery = QString("insert into userInfo(name, pwd) values('%1', '%2')").arg(name).arg(pwd);
    QSqlQuery query;

    // qDebug() << strQuery;
    return query.exec(strQuery); // 数据库中name索引是unique，所以如果name重复会返回false，插入成功返回true
}
