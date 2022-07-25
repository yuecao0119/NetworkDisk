#ifndef ONLINEUSERWID_H
#define ONLINEUSERWID_H

#include <QWidget>
#include "protocol.h"

namespace Ui {
class OnlineUserWid;
}

class OnlineUserWid : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUserWid(QWidget *parent = nullptr);
    ~OnlineUserWid();

    void setOnlineUsers(PDU *pdu);     // 设置页面显示所有的在线用户

private slots:
    void on_addfriend_pb_clicked();

private:
    Ui::OnlineUserWid *ui;
};

#endif // ONLINEUSERWID_H
