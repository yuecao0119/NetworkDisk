#include "onlineuserwid.h"
#include "ui_onlineuserwid.h"
#include <QDebug>

OnlineUserWid::OnlineUserWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUserWid)
{
    ui->setupUi(this);
}

OnlineUserWid::~OnlineUserWid()
{
    delete ui;
}

void OnlineUserWid::setOnlineUsers(PDU *pdu)
{
    if(NULL == pdu)
    {
        return ;
    }
    // 处理pdu的Msg部分，将所有在线用户显示出来
    uint uiSize = pdu -> uiMsgLen / 32; // 消息Msg部分包含的用户数
    char caTmp[32];

    ui -> onlineuser_lw -> clear();// 清除之前在线用户列表的旧数据
    for(uint i = 0; i < uiSize; ++ i)
    {
        memcpy(caTmp, (char*)(pdu -> caMsg) + 32 * i, 32);
        // qDebug() << "在线用户：" << caTmp;
        ui -> onlineuser_lw -> addItem(caTmp);
    }
}
