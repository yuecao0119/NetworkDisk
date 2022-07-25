#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
// 页面所用到的部件
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout> // 垂直布局
#include <QHBoxLayout> // 水平布局
#include "protocol.h"
#include "onlineuserwid.h" // 所有在线用户

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void setOnlineUsers(PDU* pdu);    // 设置所有在线用户的信息

    QString getStrSearchName() const;
    void setStrSearchName(const QString &strSearchName);

public slots:
    void showOrHideOnlineUserW();     // 处理显示/隐藏所有在线用户按钮点击信号的槽函数
    void searchUser();                // 处理查找用户按钮点击信号的槽函数
signals:

private:
    QListWidget *m_pFriendLW;         // 好友列表
    QLineEdit *m_pInputMsgLE;         // 信息输入框
    QTextEdit *m_pShowMsgTE;          // 显示信息

    QPushButton *m_pDelFriendPB;      // 删除好友
    QPushButton *m_pFlushFriendPB;    // 刷新好友列表
    QPushButton *m_pSOrHOnlineUserPB; // 显示/隐藏所有在线用户
    QPushButton *m_pSearchUserPB;     // 查找用户
    QPushButton *m_pSendMsgPB;        // 发送消息
    QPushButton *m_pPrivateChatPB;    // 私聊按钮，默认群聊

    OnlineUserWid *m_pOnlineUserW;    // 所有在线用户页面

    QString m_strSearchName;          // 查找的用户的名字
};

#endif // FRIEND_H
