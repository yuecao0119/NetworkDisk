/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *name_lb;
    QLineEdit *name_le;
    QHBoxLayout *horizontalLayout_2;
    QLabel *pwd_lb;
    QLineEdit *pwd_le;
    QPushButton *login_pb;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *regist_pb;
    QSpacerItem *horizontalSpacer;
    QPushButton *logout_pb;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(464, 280);
        verticalLayout_2 = new QVBoxLayout(TcpClient);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        name_lb = new QLabel(TcpClient);
        name_lb->setObjectName(QString::fromUtf8("name_lb"));
        QFont font;
        font.setFamily(QString::fromUtf8("Adobe Devanagari"));
        font.setPointSize(16);
        name_lb->setFont(font);

        horizontalLayout->addWidget(name_lb);

        name_le = new QLineEdit(TcpClient);
        name_le->setObjectName(QString::fromUtf8("name_le"));
        name_le->setFont(font);

        horizontalLayout->addWidget(name_le);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pwd_lb = new QLabel(TcpClient);
        pwd_lb->setObjectName(QString::fromUtf8("pwd_lb"));
        pwd_lb->setFont(font);

        horizontalLayout_2->addWidget(pwd_lb);

        pwd_le = new QLineEdit(TcpClient);
        pwd_le->setObjectName(QString::fromUtf8("pwd_le"));
        pwd_le->setFont(font);
        pwd_le->setEchoMode(QLineEdit::Password);

        horizontalLayout_2->addWidget(pwd_le);


        verticalLayout->addLayout(horizontalLayout_2);

        login_pb = new QPushButton(TcpClient);
        login_pb->setObjectName(QString::fromUtf8("login_pb"));
        login_pb->setFont(font);

        verticalLayout->addWidget(login_pb);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        regist_pb = new QPushButton(TcpClient);
        regist_pb->setObjectName(QString::fromUtf8("regist_pb"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Adobe Devanagari"));
        font1.setPointSize(12);
        regist_pb->setFont(font1);

        horizontalLayout_3->addWidget(regist_pb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        logout_pb = new QPushButton(TcpClient);
        logout_pb->setObjectName(QString::fromUtf8("logout_pb"));
        logout_pb->setFont(font1);

        horizontalLayout_3->addWidget(logout_pb);


        verticalLayout->addLayout(horizontalLayout_3);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        name_lb->setText(QCoreApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        name_le->setText(QString());
        pwd_lb->setText(QCoreApplication::translate("TcpClient", "\345\257\206     \347\240\201\357\274\232", nullptr));
        pwd_le->setText(QString());
        login_pb->setText(QCoreApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        regist_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        logout_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
