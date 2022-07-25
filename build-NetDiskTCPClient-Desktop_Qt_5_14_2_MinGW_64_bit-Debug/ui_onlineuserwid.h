/********************************************************************************
** Form generated from reading UI file 'onlineuserwid.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONLINEUSERWID_H
#define UI_ONLINEUSERWID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OnlineUserWid
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *onlineuser_lw;
    QVBoxLayout *verticalLayout;
    QPushButton *addfriend_pb;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *OnlineUserWid)
    {
        if (OnlineUserWid->objectName().isEmpty())
            OnlineUserWid->setObjectName(QString::fromUtf8("OnlineUserWid"));
        OnlineUserWid->resize(681, 459);
        horizontalLayout = new QHBoxLayout(OnlineUserWid);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        onlineuser_lw = new QListWidget(OnlineUserWid);
        new QListWidgetItem(onlineuser_lw);
        new QListWidgetItem(onlineuser_lw);
        onlineuser_lw->setObjectName(QString::fromUtf8("onlineuser_lw"));
        QFont font;
        font.setPointSize(14);
        onlineuser_lw->setFont(font);

        horizontalLayout->addWidget(onlineuser_lw);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        addfriend_pb = new QPushButton(OnlineUserWid);
        addfriend_pb->setObjectName(QString::fromUtf8("addfriend_pb"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Adobe Devanagari"));
        font1.setPointSize(16);
        addfriend_pb->setFont(font1);

        verticalLayout->addWidget(addfriend_pb);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(OnlineUserWid);

        QMetaObject::connectSlotsByName(OnlineUserWid);
    } // setupUi

    void retranslateUi(QWidget *OnlineUserWid)
    {
        OnlineUserWid->setWindowTitle(QCoreApplication::translate("OnlineUserWid", "Form", nullptr));

        const bool __sortingEnabled = onlineuser_lw->isSortingEnabled();
        onlineuser_lw->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = onlineuser_lw->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("OnlineUserWid", "\345\245\275\345\217\2131", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = onlineuser_lw->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("OnlineUserWid", "\345\245\275\345\217\2132", nullptr));
        onlineuser_lw->setSortingEnabled(__sortingEnabled);

        addfriend_pb->setText(QCoreApplication::translate("OnlineUserWid", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OnlineUserWid: public Ui_OnlineUserWid {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONLINEUSERWID_H
