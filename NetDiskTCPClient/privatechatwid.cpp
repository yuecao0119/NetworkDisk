#include "privatechatwid.h"
#include "tcpclient.h"
#include "ui_privatechatwid.h"

#include <QMessageBox>
#include "protocol.h"

PrivateChatWid::PrivateChatWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChatWid)
{
    ui->setupUi(this);
}

PrivateChatWid::~PrivateChatWid()
{
    delete ui;
}

QString PrivateChatWid::strChatName() const
{
    return m_strChatName;
}

void PrivateChatWid::setStrChatName(const QString &strChatName)
{
    m_strChatName = strChatName;
}

// 发送消息按钮的槽函数
void PrivateChatWid::on_sendMsg_pb_clicked()
{
    QString strSendMsg = ui -> inputMsg_le -> text();
    if(strSendMsg.isEmpty())
    {
        QMessageBox::warning(this, "私聊", "发送消息不能为空！");
        return ;
    }

    // 显示在自己showMsgTE窗口上
    ui -> inputMsg_le -> clear(); // 清空输入框内容
    ui -> showMsg_te -> append(QString("%1 : %2").arg(m_strLoginName).arg(strSendMsg));

    // 发送消息给服务器来转发给对方
    PDU *pdu = mkPDU(strSendMsg.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
    strncpy(pdu -> caData, m_strChatName.toStdString().c_str(), 32); // 目标用户名
    strncpy(pdu -> caData + 32, m_strLoginName.toStdString().c_str(), 32); // 请求方用户名
    strncpy((char*)pdu -> caMsg, strSendMsg.toStdString().c_str(), strSendMsg.size()); // 发送内容

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

QString PrivateChatWid::strLoginName() const
{
    return m_strLoginName;
}

void PrivateChatWid::setStrLoginName(const QString &strLoginName)
{
    m_strLoginName = strLoginName;
}

void PrivateChatWid::updateShowMsgTE(PDU *pdu)
{
    if(NULL == pdu)
    {
        return ;
    }
    char caSendName[32] = {'\0'};
    strncpy(caSendName, pdu -> caData + 32, 32);
    QString strMsg = QString("%1 : %2").arg(caSendName).arg((char*)pdu -> caMsg);
    ui -> showMsg_te -> append(strMsg);
}

void PrivateChatWid::setPriChatTitle(const char *caTitle)
{
    ui -> priChatName_l -> setText(caTitle);
}
