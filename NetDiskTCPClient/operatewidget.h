#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H

#include <QWidget>
#include <QListWidget> // 列表框控件用来加载并显示多个列表项，这里用来组织客户端主要功能（好友、文件等）
#include "friend.h"
#include "filesystem.h"
#include <QStackedWidget>

class OperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateWidget(QWidget *parent = nullptr);

signals:


private:
    QListWidget *m_pListWidget; // 组织主页面左侧常用功能（好友、文件按钮等）
    Friend *m_pFriend;          // 好友页面
    FileSystem *m_pFileSystem;  // 文件页面
    QStackedWidget *m_pSW;      // 容器，每次显示一个页面（好友or文件）
};

#endif // OPERATEWIDGET_H
