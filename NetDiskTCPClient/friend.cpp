#include "friend.h"

#include "tcpclient.h"  // 不放在头文件，减少交叉引入
#include <QInputDialog> // 一个内置输入框窗口

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pFriendLW = new QListWidget;
    m_pInputMsgLE = new QLineEdit;
    m_pShowMsgTE = new QTextEdit;
    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pSOrHOnlineUserPB = new QPushButton("所有在线用户");
    m_pSearchUserPB = new QPushButton("查找用户");
    m_pSendMsgPB = new QPushButton("发送");
    m_pPrivateChatPB = new QPushButton("私聊");

    m_pOnlineUserW = new OnlineUserWid;

    QVBoxLayout *pLeftRightVBL = new QVBoxLayout; // 左侧右部分好友操作按钮布局
    pLeftRightVBL -> addWidget(m_pPrivateChatPB);
    pLeftRightVBL -> addWidget(m_pDelFriendPB);
    pLeftRightVBL -> addWidget(m_pFlushFriendPB);
    pLeftRightVBL -> addWidget(m_pSOrHOnlineUserPB);
    pLeftRightVBL -> addWidget(m_pSearchUserPB);
    QHBoxLayout *pRightDownHBL = new QHBoxLayout; // 右侧下方发送消息布局
    pRightDownHBL -> addWidget(m_pInputMsgLE);
    pRightDownHBL -> addWidget(m_pSendMsgPB);
    QVBoxLayout *pRightVBL = new QVBoxLayout; // 右侧聊天布局
    pRightVBL -> addWidget(m_pShowMsgTE);
    pRightVBL -> addLayout(pRightDownHBL);
    QHBoxLayout *pMainHBL = new QHBoxLayout; // 整体水平布局
    pMainHBL -> addWidget(m_pFriendLW);      // 左侧左部分好友列表
    pMainHBL -> addLayout(pLeftRightVBL);    // 左侧右部分好友操作
    pMainHBL -> addLayout(pRightVBL);        // 右侧聊天布局

    m_pOnlineUserW -> hide(); // 默认所有在线用户页面隐藏

    setLayout(pMainHBL); // 将整体布局pAllHBL设置为页面布局

    // 绑定打开所有在线用户按钮与对应事件
    connect(m_pSOrHOnlineUserPB, SIGNAL(clicked(bool)),
            this, SLOT(showOrHideOnlineUserW()));
    // 绑定查找用户按钮与对应事件
    connect(m_pSearchUserPB, SIGNAL(clicked(bool)),
            this, SLOT(searchUser()));
}

void Friend::setOnlineUsers(PDU* pdu)
{
    if(NULL == pdu)
    {
        return ;
    }
    m_pOnlineUserW->setOnlineUsers(pdu);
}

void Friend::showOrHideOnlineUserW()
{
    if(m_pOnlineUserW -> isHidden())
    {
        // 显示onlineUserWid页面
        m_pOnlineUserW -> show();
        // 发送请求查询数据库获取在线用户
        PDU *pdu = mkPDU(0);
        pdu -> uiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        m_pOnlineUserW -> hide();
    }
}

void Friend::searchUser()
{
    m_strSearchName = QInputDialog::getText(this, "搜索", "用户名："); // 通过输入子页面来获取用户输入返回一个文本类型

    if(!m_strSearchName.isEmpty())
    {
        qDebug() << "查找：" << m_strSearchName;
        PDU *pdu = mkPDU(0);
        pdu -> uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        memcpy((char*)pdu -> caData, m_strSearchName.toStdString().c_str(), m_strSearchName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

QString Friend::getStrSearchName() const
{
    return m_strSearchName;
}

void Friend::setStrSearchName(const QString &strSearchName)
{
    m_strSearchName = strSearchName;
}
