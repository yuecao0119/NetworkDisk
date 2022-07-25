#include "onlineuserwid.h"
#include "ui_onlineuserwid.h"

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
