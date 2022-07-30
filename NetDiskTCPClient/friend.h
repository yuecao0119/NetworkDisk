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
#include "privatechatwid.h" // 私聊窗口

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);

    QString getStrSearchName() const;
    void setStrSearchName(const QString &strSearchName);

    void setOnlineUsers(PDU* pdu);    // 设置所有在线用户的信息
    void updateFriendList(PDU *pdu);  // 刷新好友列表

    QListWidget *getPFriendLW() const;

public slots:
    void showOrHideOnlineUserW();     // 处理显示/隐藏所有在线用户按钮点击信号的槽函数
    void searchUser();                // 处理查找用户按钮点击信号的槽函数
    void flushFriendList();           // 刷新好友列表按钮点击信号的槽函数
    void deleteFriend();              // 删除好友按钮点击信号的槽函数
    void privateChat();               // 私聊按钮的槽函数
    PrivateChatWid* searchPriChatWid(const char* chatName); // 获得对应用户名的私聊窗口
    void insertPriChatWidList(PrivateChatWid* priChat); // 将私聊窗口插入到私聊窗口List
    void groupChatSendMsg();          // 群聊发送按钮的槽函数
    void updateGroupShowMsgTE(PDU* pdu); // 更新群聊showMsgTE聊天消息窗口的消息

signals:

private:
    QListWidget *m_pFriendLW;         // 好友列表   
    QPushButton *m_pDelFriendPB;      // 删除好友
    QPushButton *m_pFlushFriendPB;    // 刷新好友列表
    QPushButton *m_pSOrHOnlineUserPB; // 显示/隐藏所有在线用户
    QPushButton *m_pSearchUserPB;     // 查找用户
    QLineEdit *m_pGroupInputLE;       // 群聊信息输入框
    QPushButton *m_pGroupSendMsgPB;   // 群聊发送消息
    QTextEdit *m_pGroupShowMsgTE;     // 显示群聊信息
    QPushButton *m_pPrivateChatPB;    // 私聊按钮，默认群聊

    OnlineUserWid *m_pOnlineUserW;    // 所有在线用户页面

    QString m_strSearchName;          // 查找的用户的名字

    QList<PrivateChatWid*> m_priChatWidList; // 所有私聊的窗口
};

#endif // FRIEND_H
