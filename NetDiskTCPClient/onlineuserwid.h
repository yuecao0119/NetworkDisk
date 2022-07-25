#ifndef ONLINEUSERWID_H
#define ONLINEUSERWID_H

#include <QWidget>

namespace Ui {
class OnlineUserWid;
}

class OnlineUserWid : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUserWid(QWidget *parent = nullptr);
    ~OnlineUserWid();

private:
    Ui::OnlineUserWid *ui;
};

#endif // ONLINEUSERWID_H
