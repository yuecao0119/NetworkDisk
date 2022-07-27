#include "operatewidget.h"

OperateWidget::OperateWidget(QWidget *parent) : QWidget(parent)
{
    m_pUserLabel = new QLabel;
    m_pListWidget = new QListWidget(this); // 参数指示QWidget *parent
    m_pListWidget -> addItem("好友");
    m_pListWidget -> addItem("文件");

    m_pFriend = new Friend;
    m_pFileSystem = new FileSystem;

    m_pSW = new QStackedWidget;
    m_pSW -> addWidget(m_pFriend);     // 如果没有设置，默认显示第一个页面
    m_pSW -> addWidget(m_pFileSystem);

    QHBoxLayout *pBodyHBL = new QHBoxLayout;
    pBodyHBL -> addWidget(m_pListWidget);
    pBodyHBL -> addWidget(m_pSW);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL -> addWidget(m_pUserLabel);
    pMainVBL -> addLayout(pBodyHBL);

    setLayout(pMainVBL);

    // 将m_pListWidget的行号变化信号与m_pSW的设置当前页面槽函数关联
    connect(m_pListWidget, SIGNAL(currentRowChanged(int)), // 函数参数为改变后的行号
            m_pSW, SLOT(setCurrentIndex(int))); // 函数参数为设置的页面下标
}

OperateWidget &OperateWidget::getInstance()
{
    static OperateWidget instance;
    return instance;
}

Friend *OperateWidget::getPFriend() const
{
    return m_pFriend;
}

void OperateWidget::setUserLabel(const char *name)
{
    m_pUserLabel->setText(name);
}

FileSystem *OperateWidget::getPFileSystem() const
{
    return m_pFileSystem;
}

