#include "sharedfilefriendlist.h"
#include <QDebug>
#include "operatewidget.h"
#include "tcpclient.h"

sharedFileFriendList::sharedFileFriendList(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选"); // 全选
    m_pCancleSelectPB = new QPushButton("清空"); // 取消选择

    m_pAffirmPB = new QPushButton("确认"); // 确认键
    m_pCanclePB = new QPushButton("取消"); // 取消键

    m_pFriendsSA = new QScrollArea; // 展示好友区
    m_pFriendsWid = new QWidget; // 所有好友窗口
    m_pFriendsVBL = new QVBoxLayout(m_pFriendsWid); // 好友信息垂直布局
    m_pFriendsBG = new QButtonGroup(m_pFriendsWid); // bg主要用来管理好友选项，Wid设置为其父类
    m_pFriendsBG->setExclusive(false); // 可以多选

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addStretch(); // 添加弹簧
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancleSelectPB);

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pAffirmPB);
    pDownHBL->addWidget(m_pCanclePB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pFriendsSA); // SA中放置Wid，Wid是BG的父类
    pMainVBL->addLayout(pDownHBL);

    setLayout(pMainVBL);

    connect(m_pSelectAllPB, SIGNAL(clicked(bool)),
            this, SLOT(selectAll()));
    connect(m_pCancleSelectPB, SIGNAL(clicked(bool)),
            this, SLOT(cancleSelect()));
    connect(m_pAffirmPB, SIGNAL(clicked(bool)),
            this, SLOT(affirmShare()));
    connect(m_pCanclePB, SIGNAL(clicked(bool)),
            this, SLOT(cancleShare()));
}

void sharedFileFriendList::updateFriendList(QListWidget *pFriendList)
{
    if(NULL == pFriendList)
    {
        return ;
    }

    // 移除之前的好友列表
    QList<QAbstractButton*> preFriendList = m_pFriendsBG->buttons();
    for(QAbstractButton* pItem:preFriendList)
    {
        m_pFriendsVBL->removeWidget(pItem);
        m_pFriendsBG->removeButton(pItem);
        delete pItem;
        pItem = NULL;
    }
    // 设置新的好友列表
    QCheckBox *pCB = NULL;
    for(int i = 0; i < pFriendList->count(); i ++)
    {
        qDebug() << "好友：" << pFriendList->item(i)->text();
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendsVBL->addWidget(pCB);
        m_pFriendsBG->addButton(pCB);
    }
    m_pFriendsSA->setWidget(m_pFriendsWid); // 每次都需要重新设置SA的Widget！
}

void sharedFileFriendList::selectAll()
{
    QList<QAbstractButton*> friendsButtons = m_pFriendsBG->buttons();

    for(QAbstractButton* pItem:friendsButtons)
    {
        pItem->setChecked(true);
    }
}

void sharedFileFriendList::cancleSelect()
{
    QList<QAbstractButton*> friendsButtons = m_pFriendsBG->buttons();

    for(QAbstractButton* pItem:friendsButtons)
    {
        pItem->setChecked(false);
    }
}

void sharedFileFriendList::affirmShare()
{
    QString strFileName = OperateWidget::getInstance().getPFileSystem()->getStrSharedFileName();
    QString strFilePath = OperateWidget::getInstance().getPFileSystem()->getStrSharedFilePath();

    QList<QAbstractButton*> abList = m_pFriendsBG->buttons();
    QList<QString> userList; // 要分享的好友列表

    for(int i = 0; i < abList.count(); ++ i)
    {
        if(abList[i]->isChecked())
        {
            userList.append(abList[i]->text().split('\t')[0]);
        }
    }
    int iUserNum = userList.count();
    qDebug() << "分享好友：" << userList << " " << iUserNum;

    PDU* pdu = mkPDU(strFilePath.size() + userList.count() * 32 + 1); // caMsg中存放文件路径、分享好友名
    pdu -> uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    for(int i = 0; i < iUserNum; ++ i)
    {
        strncpy((char*)(pdu -> caMsg) + 32 * i, userList[i].toStdString().c_str(), 32);
    }
    memcpy((char*)(pdu -> caMsg) + 32 * iUserNum, strFilePath.toStdString().c_str(), strFilePath.size());

    sprintf(pdu -> caData, "%s %d", strFileName.toStdString().c_str(), iUserNum); // caData存放文件名、分享好友数

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;

    this -> hide();
}

void sharedFileFriendList::cancleShare()
{
    this -> hide();
}
