#ifndef DBOPERATE_H
#define DBOPERATE_H

#include <QObject>
#include <QSqlDatabase> // 连接数据库
#include <QSqlQuery>    // 数据库操作
#include <QStringList>

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
    QStringList handleOnlineUsers();                      // 处理查询所有在线用户
    int handleSearchUser(const char *name);               // 处理查找用户，0存在不在线，1存在并在线，2不存在
    int handleAddFriend(const char *addedName, const char *sourceName);       // 处理添加好友
    bool handleAddFriendAgree(const char *addedName, const char *sourceName); // 处理同意好友申请
    int getIdByUserName(const char *name);                // 根据用户名获取用户id
    QStringList handleFlushFriend(const char *name); // 处理刷新好友列表
    bool handleDeleteFriend(const char *deletedName, const char *sourceName); // 处理删除好友

signals:

public slots:
private:
    QSqlDatabase m_db; // 连接数据库

};

#endif // DBOPERATE_H
