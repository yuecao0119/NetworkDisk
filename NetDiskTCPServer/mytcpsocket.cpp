#include "mytcpsocket.h"

MyTcpSocket::MyTcpSocket()
{
    connect(this, SIGNAL(readyRead()), // 当接收到客户端的数据时，服务器会发送readyRead()信号
            this, SLOT(receiveMsg())); // 需要由服务器的相应receiveMsg槽函数进行处理
    connect(this, SIGNAL(disconnected()), this, SLOT(handleClientOffline())); // 关联Socket连接断开与客户端下线处理槽函数
}

QString MyTcpSocket::getStrName()
{
    return m_strName;
}

// 处理注册请求并返回响应PDU
PDU* handleRegistRequest(PDU* pdu)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(caName, pdu -> caData, 32);
    strncpy(caPwd, pdu -> caData + 32, 32);
    qDebug() << pdu -> uiMsgType << " " << caName << " " << caPwd;
    bool ret = DBOperate::getInstance().handleRegist(caName, caPwd); // 处理请求，插入数据库

    // 响应客户端
    PDU *resPdu = mkPDU(0); // 响应消息
    resPdu -> uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
    if(ret)
    {
        strcpy(resPdu -> caData, REGIST_OK);
    }
    else
    {
        strcpy(resPdu -> caData, REGIST_FAILED);
    }
    // qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;

    return resPdu;
}

// 处理登录请求并返回响应PDU
PDU* handleLoginRequest(PDU* pdu, QString& m_strName)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(caName, pdu -> caData, 32);
    strncpy(caPwd, pdu -> caData + 32, 32);
    qDebug() << pdu -> uiMsgType << " " << caName << " " << caPwd;
    bool ret = DBOperate::getInstance().handleLogin(caName, caPwd); // 处理请求，插入数据库

    // 响应客户端
    PDU *resPdu = mkPDU(0); // 响应消息
    resPdu -> uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    if(ret)
    {
        strcpy(resPdu -> caData, LOGIN_OK);
        // 在登陆成功时，记录Socket对应的用户名
        m_strName = caName;
        qDebug() << "m_strName: " << m_strName;
    }
    else
    {
        strcpy(resPdu -> caData, LOGIN_FAILED);
    }
    // qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;

    return resPdu;
}

void MyTcpSocket::receiveMsg()
{
    qDebug() << this -> bytesAvailable(); // 输出接收到的数据大小
    uint uiPDULen = 0;
    this -> read((char*)&uiPDULen, sizeof(uint)); // 先读取uint大小的数据，首个uint正是总数据大小
    uint uiMsgLen = uiPDULen - sizeof(PDU); // 实际消息大小，sizeof(PDU)只会计算结构体大小，而不是分配的大小
    PDU *pdu = mkPDU(uiMsgLen);
    this -> read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint)); // 接收剩余部分数据（第一个uint已读取）
    // qDebug() << pdu -> uiMsgType << ' ' << (char*)pdu -> caMsg; // 输出

    // 根据不同消息类型，执行不同操作
    PDU* resPdu = NULL; // 响应报文
    switch(pdu -> uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_REQUEST: // 注册请求
    {
        resPdu = handleRegistRequest(pdu); // 请求处理
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST: // 登录请求
    {
        resPdu = handleLoginRequest(pdu, m_strName);
        break;
    }
    default:
        break;
    }

    // 响应客户端
    if(NULL != resPdu)
    {
        qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;
        this -> write((char*)resPdu, resPdu -> uiPDULen);
        // 释放空间
        free(resPdu);
        resPdu = NULL;
    }
    // 释放空间
    free(pdu);
    pdu = NULL;
}

void MyTcpSocket::handleClientOffline()
{
    DBOperate::getInstance().handleOffline(m_strName.toStdString().c_str());
    emit offline(this); // 发送给mytcpserver该socket删除信号
}



