/********************************************************************************
** Form generated from reading UI file 'privatechatwid.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIVATECHATWID_H
#define UI_PRIVATECHATWID_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrivateChatWid
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *priChatName_l;
    QTextEdit *showMsg_te;
    QHBoxLayout *horizontalLayout;
    QLineEdit *inputMsg_le;
    QPushButton *sendMsg_pb;

    void setupUi(QWidget *PrivateChatWid)
    {
        if (PrivateChatWid->objectName().isEmpty())
            PrivateChatWid->setObjectName(QString::fromUtf8("PrivateChatWid"));
        PrivateChatWid->resize(686, 527);
        verticalLayout_2 = new QVBoxLayout(PrivateChatWid);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        priChatName_l = new QLabel(PrivateChatWid);
        priChatName_l->setObjectName(QString::fromUtf8("priChatName_l"));
        QFont font;
        font.setFamily(QString::fromUtf8("Adobe Devanagari"));
        font.setPointSize(16);
        priChatName_l->setFont(font);

        verticalLayout->addWidget(priChatName_l);

        showMsg_te = new QTextEdit(PrivateChatWid);
        showMsg_te->setObjectName(QString::fromUtf8("showMsg_te"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Adobe Devanagari"));
        font1.setPointSize(14);
        showMsg_te->setFont(font1);

        verticalLayout->addWidget(showMsg_te);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        inputMsg_le = new QLineEdit(PrivateChatWid);
        inputMsg_le->setObjectName(QString::fromUtf8("inputMsg_le"));
        inputMsg_le->setFont(font);

        horizontalLayout->addWidget(inputMsg_le);

        sendMsg_pb = new QPushButton(PrivateChatWid);
        sendMsg_pb->setObjectName(QString::fromUtf8("sendMsg_pb"));
        sendMsg_pb->setFont(font);

        horizontalLayout->addWidget(sendMsg_pb);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(PrivateChatWid);

        QMetaObject::connectSlotsByName(PrivateChatWid);
    } // setupUi

    void retranslateUi(QWidget *PrivateChatWid)
    {
        PrivateChatWid->setWindowTitle(QCoreApplication::translate("PrivateChatWid", "Form", nullptr));
        priChatName_l->setText(QCoreApplication::translate("PrivateChatWid", "\347\247\201\350\201\212\345\257\271\350\261\241", nullptr));
        sendMsg_pb->setText(QCoreApplication::translate("PrivateChatWid", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PrivateChatWid: public Ui_PrivateChatWid {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIVATECHATWID_H
