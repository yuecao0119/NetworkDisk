#ifndef DBOPERATE_H
#define DBOPERATE_H

#include <QObject>
#include <QSqlDatabase> // 连接数据库
#include <QSqlQuery> // 数据库操作

class DBOperate : public QObject
{
    Q_OBJECT
public:
    explicit DBOperate(QObject *parent = nullptr);
    static DBOperate& getInstance(); // 公用获取引用，实现单例模式
    void init();                     // 初始化函数，数据库连接
    ~DBOperate();                    // 析构函数，关闭数据库连接

    bool handleRegist(const char *name, const char *pwd); // 处理注册操作，写入数据库用户信息
    bool handleLogin(const char *name, const char *pwd);  // 处理登录操作
    bool handleOffline(const char *name);                 // 处理用户下线
signals:

public slots:
private:
    QSqlDatabase m_db; // 连接数据库

};

#endif // DBOPERATE_H
