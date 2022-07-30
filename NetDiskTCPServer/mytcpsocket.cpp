#include "mytcpsocket.h"
#include "mytcpserver.h"
#include <QDir> // 操作文件夹的库
#include <QDateTime>

MyTcpSocket::MyTcpSocket()
{
    m_uploadFile = new TransFile;
    m_uploadFile->bTransform = false; // 默认不是在上传文件
    m_pDownloadFile = new QFile;
    m_pTimer = new QTimer;

    connect(this, SIGNAL(readyRead()), // 当接收到客户端的数据时，服务器会发送readyRead()信号
            this, SLOT(receiveMsg())); // 需要由服务器的相应receiveMsg槽函数进行处理
    connect(this, SIGNAL(disconnected()), this, SLOT(handleClientOffline())); // 关联Socket连接断开与客户端下线处理槽函数
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handledownloadFileData())); // 关联计时器倒计时
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
    // qDebug() << pdu -> uiMsgType << " " << caName << " " << caPwd;
    bool ret = DBOperate::getInstance().handleRegist(caName, caPwd); // 处理请求，插入数据库

    // 响应客户端
    PDU *resPdu = mkPDU(0); // 响应消息
    resPdu -> uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
    if(ret)
    {
        strcpy(resPdu -> caData, REGIST_OK);
        // 注册成功，为新用户按用户名创建文件夹
        QDir dir;
        ret = dir.mkdir(QString("%1/%2").arg(MyTcpServer::getInstance().getStrRootPath()).arg(caName));
        qDebug() << "创建新用户文件夹 " << ret;
    }
    if(!ret)
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
    // qDebug() << pdu -> uiMsgType << " " << caName << " " << caPwd;
    bool ret = DBOperate::getInstance().handleLogin(caName, caPwd); // 处理请求，插入数据库

    // 响应客户端
    PDU *resPdu = NULL; // 响应消息
    if(ret)
    {
        QString strUserRootPath = QString("%1/%2")
                .arg(MyTcpServer::getInstance().getStrRootPath()).arg(caName); // 用户文件系统根目录
        qDebug() << "登录用户的路径：" << strUserRootPath;
        resPdu = mkPDU(strUserRootPath.size() + 1);
        memcpy(resPdu -> caData, LOGIN_OK, 32);
        memcpy(resPdu -> caData + 32, caName, 32); // 将登录后的用户名传回，便于tcpclient确认已经登陆的用户名
        // 在登陆成功时，记录Socket对应的用户名
        m_strName = caName;
        // qDebug() << "m_strName: " << m_strName;
        // 返回用户的根目录
        strncpy((char*)resPdu -> caMsg, strUserRootPath.toStdString().c_str(), strUserRootPath.size() + 1);
    }
    else
    {
        resPdu = mkPDU(0);
        strcpy(resPdu -> caData, LOGIN_FAILED);
    }
    resPdu -> uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    qDebug() << "登录处理：" << resPdu -> uiMsgType << " " << resPdu ->caData << " " << resPdu ->caData + 32;

    return resPdu;
}

// 处理查询所有在线用户的请求
PDU* handleOnlineUsersRequest()
{
    QStringList strList = DBOperate::getInstance().handleOnlineUsers(); // 查询请求，查询数据库所有在线用户
    uint uiMsgLen = strList.size() * 32; // 消息报文的长度

    // 响应客户端
    PDU *resPdu = mkPDU(uiMsgLen); // 响应消息
    resPdu -> uiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_RESPOND;
    // qDebug() << "在线用户数：" << strList.size();
    for(int i = 0; i < strList.size(); ++ i)
    {
        memcpy((char*)(resPdu -> caMsg) + 32 * i, strList[i].toStdString().c_str(), strList[i].size());
        // qDebug() << "所有在线用户有：" << (char*)(resPdu -> caMsg) + 32 * i;
    }

    return resPdu;
}

// 处理查找用户的请求
PDU* handleSearchUserRequest(PDU* pdu)
{
    char caName[32] = {'\0'};
    strncpy(caName, pdu -> caData, 32);
    int ret = DBOperate::getInstance().handleSearchUser(caName); // 处理请求

    // 响应客户端
    PDU *resPdu = mkPDU(0); // 响应消息
    resPdu -> uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
    if(ret == 1)
    {
        strcpy(resPdu -> caData, SEARCH_USER_OK);
    }
    else if(ret == 0)
    {
        strcpy(resPdu -> caData, SEARCH_USER_OFFLINE);
    }
    else
    {
        strcpy(resPdu -> caData, SEARCH_USER_EMPTY);
    }

    return resPdu;
}

// 处理添加好友请求
PDU* handleAddFriendRequest(PDU* pdu)
{
    char addedName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(addedName, pdu -> caData, 32);
    strncpy(sourceName, pdu -> caData + 32, 32);
    // qDebug() << "handleAddFriendRequest  " << addedName << " " << sourceName;
    int iSearchUserStatus = DBOperate::getInstance().handleAddFriend(addedName, sourceName);
    // 0对方存在不在线，1对方存在在线，2不存在，3已是好友，4请求错误
    PDU* resPdu = NULL;

    switch (iSearchUserStatus) {
    case 0: // 0对方存在不在线
    {
        resPdu = mkPDU(0);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(resPdu -> caData, ADD_FRIEND_OFFLINE);
        break;
    }
    case 1: // 1对方存在在线
    {
        // 需要转发给对方请求添加好友消息
        MyTcpServer::getInstance().forwardMsg(addedName, pdu);

        resPdu = mkPDU(0);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(resPdu -> caData, ADD_FRIEND_OK); // 表示加好友请求已发送
        break;
    }
    case 2: // 2用户不存在
    {
        resPdu = mkPDU(0);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(resPdu -> caData, ADD_FRIEND_EMPTY);
        break;
    }
    case 3: // 3已是好友
    {
        resPdu = mkPDU(0);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(resPdu -> caData, ADD_FRIEND_EXIST);
        break;
    }
    case 4: // 4请求错误
    {
        resPdu = mkPDU(0);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(resPdu -> caData, UNKNOWN_ERROR);
        break;
    }
    default:
        break;
    }

    return resPdu;
}

// 同意加好友
void handleAddFriendAgree(PDU* pdu)
{
    char addedName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(addedName, pdu -> caData, 32);
    strncpy(sourceName, pdu -> caData + 32, 32);

    // 将新的好友关系信息写入数据库
    DBOperate::getInstance().handleAddFriendAgree(addedName, sourceName);

    // 服务器需要转发给发送好友请求方其被同意的消息
    MyTcpServer::getInstance().forwardMsg(sourceName, pdu);
}

// 拒绝加好友
void handleAddFriendReject(PDU* pdu)
{
    char sourceName[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(sourceName, pdu -> caData + 32, 32);
    // 服务器需要转发给发送好友请求方其被拒绝的消息
    MyTcpServer::getInstance().forwardMsg(sourceName, pdu);
}

// 刷新好友列表请求
PDU* handleFlushFriendRequest(PDU* pdu)
{
    char caName[32] = {'\0'};

    strncpy(caName, pdu -> caData, 32);

    QStringList strList = DBOperate::getInstance().handleFlushFriend(caName);
    uint uiMsgLen = strList.size() / 2 * 36; // 36 char[32] 好友名字+ 4 int 在线状态

    PDU* resPdu = mkPDU(uiMsgLen);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
    for(int i = 0; i * 2 < strList.size(); ++ i)
    {
        strncpy((char*)(resPdu -> caMsg) + 36 * i, strList.at(i * 2).toStdString().c_str(), 32);
        strncpy((char*)(resPdu -> caMsg) + 36 * i + 32, strList.at(i * 2 + 1).toStdString().c_str(), 4);
    }

    return resPdu;
}

// 删除好友请求
PDU* handleDeleteFriendRequest(PDU* pdu)
{
    char deletedName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(deletedName, pdu -> caData, 32);
    strncpy(sourceName, pdu -> caData + 32, 32);
    bool ret = DBOperate::getInstance().handleDeleteFriend(deletedName, sourceName);

    // 给请求删除方消息提示，以返回值形式
    PDU *resPdu = mkPDU(0);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
    if(ret)
    {
        strncpy(resPdu -> caData, DEL_FRIEND_OK, 32);
    }
    else
    {
        strncpy(resPdu -> caData, DEL_FRIEND_FAILED, 32);
    }

    // 给被删除方消息提示，如果在线的话
    MyTcpServer::getInstance().forwardMsg(deletedName, pdu);

    return resPdu;
}

// 私聊发送消息请求
PDU* handlePrivateChatRequest(PDU* pdu)
{
    char chatedName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    // 拷贝读取的信息
    strncpy(chatedName, pdu -> caData, 32);
    strncpy(sourceName, pdu -> caData + 32, 32);
    qDebug() << "handlePrivateChatRequest  " << chatedName << " " << sourceName;

    PDU* resPdu = NULL;

    // 转发给对方消息  0对方存在不在线，1对方存在在线
    bool ret = MyTcpServer::getInstance().forwardMsg(chatedName, pdu);

    // 发送失败则给发送者消息
    if(!ret)// 0对方不在线
    {
        resPdu = mkPDU(0);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND;
        strcpy(resPdu -> caData, PRIVATE_CHAT_OFFLINE);
    }

    return resPdu;
}

// 群聊请求处理
void handleGroupChatRequest(PDU* pdu)
{
    QStringList strList = DBOperate::getInstance().handleFlushFriend(pdu->caData); // 查询请求，查询数据库所有在线用户

    for(QString strName:strList)
    {
        MyTcpServer::getInstance().forwardMsg(strName, pdu);
    }
}

// 创建文件夹请求处理
PDU* handleCreateDirRequest(PDU* pdu)
{
    char caDirName[32];
    char caCurPath[pdu -> uiMsgLen];
    strncpy(caDirName, pdu -> caData, 32);
    strncpy(caCurPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);

    QString strDir = QString("%1/%2").arg(caCurPath).arg(caDirName);
    QDir dir;
    PDU *resPdu = mkPDU(0);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;

    qDebug() << "创建文件夹：" << strDir;
    if(dir.exists(caCurPath)) // 路径存在
    {
        if(dir.exists(strDir)) // 文件夹已经存在
        {
            strncpy(resPdu -> caData, CREATE_DIR_EXIST, 32);
        }
        else
        {
            dir.mkdir(strDir); // 创建文件夹
            strncpy(resPdu -> caData, CREATE_DIR_OK, 32);
        }
    }
    else // 路径不存在
    {
        strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
    }

    return resPdu;
}

// 刷新文件夹请求处理
PDU* handleFlushDirRequest(PDU* pdu)
{
    char caCurDir[pdu -> uiMsgLen];
    memcpy(caCurDir, (char*)pdu -> caMsg, pdu -> uiMsgLen);
    qDebug() << "刷新文件夹：" << caCurDir;
    QDir dir;
    PDU* resPdu = NULL;

    if(!dir.exists(caCurDir)) // 请求文件夹不存在
    {
        resPdu = mkPDU(0);
        strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
    }
    else // 存在
    {
        dir.setPath(caCurDir); // 设置为当前目录
        QFileInfoList fileInfoList = dir.entryInfoList(); // 获取当前目录下所有文件
        int iFileNum = fileInfoList.size();

        resPdu = mkPDU(sizeof(FileInfo) * iFileNum);
        FileInfo *pFileInfo = NULL; // 创建一个文件信息结构体指针，方便之后遍历PDU空间来赋值
        strncpy(resPdu -> caData, FLUSH_DIR_OK, 32);

        for(int i = 0; i < iFileNum; ++ i)
        {
            pFileInfo = (FileInfo*)(resPdu -> caMsg) + i; // 通过指针指向，直接修改PDU空间值，每次偏移FileInfo大小
            memcpy(pFileInfo -> caName, fileInfoList[i].fileName().toStdString().c_str(), fileInfoList[i].fileName().size());
            pFileInfo -> bIsDir = fileInfoList[i].isDir();
            pFileInfo -> uiSize = fileInfoList[i].size();
            QDateTime dtLastTime = fileInfoList[i].lastModified(); // 获取文件最后修改时间
            QString strLastTime = dtLastTime.toString("yyyy/MM/dd hh:mm");
            memcpy(pFileInfo -> caTime, strLastTime.toStdString().c_str(), strLastTime.size());
            qDebug() << "文件信息：" << pFileInfo -> caName << " " << pFileInfo -> bIsDir << " " << pFileInfo -> uiSize << " " << pFileInfo -> caTime;
        }
    }
    resPdu -> uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;

    return resPdu;
}

// 删除文件或文件夹处理
PDU* handleDelFileOrDirRequest(PDU* pdu)
{
    PDU* resPdu = mkPDU(0);
    char strDelPath[pdu -> uiMsgLen];
    memcpy(strDelPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
    qDebug() << "删除文件：" << strDelPath;
    QDir dir;

    resPdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
    if(!dir.exists(strDelPath)) // 路径不存在
    {
        strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
    }
    else
    {
        bool ret = false;

        QFileInfo fileInfo(strDelPath);
        if(fileInfo.isDir()) // 是文件目录
        {
            dir.setPath(strDelPath);
            ret = dir.removeRecursively();
        }
        else if(fileInfo.isFile())
        {
            ret = dir.remove(strDelPath);
        }
        if(ret)
        {
            strncpy(resPdu -> caData, DELETE_FILE_OK, 32);
        }
        else
        {
            strncpy(resPdu -> caData, DELETE_FILE_FAILED, 32);
        }
    }
    qDebug() << resPdu -> caData;

    return resPdu;
}

// 重命名文件或文件夹请求处理
PDU* handleRenameFileRequest(PDU* pdu)
{
    PDU* resPdu = mkPDU(0);
    char caCurPath[pdu -> uiMsgLen];
    char caOldName[32]; // 旧文件名
    char caNewName[32]; // 新文件名
    memcpy(caCurPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
    strncpy(caOldName, pdu -> caData, 32);
    strncpy(caNewName, pdu -> caData + 32, 32);
    qDebug() << "重命名文件：" << caCurPath << " " << caOldName << " -> " << caNewName;
    QDir dir;

    resPdu -> uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
    dir.setPath(caCurPath);
    if(dir.rename(caOldName, caNewName))
    {
        strncpy(resPdu -> caData, RENAME_FILE_OK, 32);
    }
    else
    {
        strncpy(resPdu -> caData, RENAME_FILE_FAILED, 32);
    }
    qDebug() << resPdu -> caData;

    return resPdu;
}

// 进入文件夹请求处理
PDU* handleEntryDirRequest(PDU* pdu)
{
    char strEntryPath[pdu -> uiMsgLen]; // 进入文件夹路径
    memcpy(strEntryPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
    qDebug() << "进入 " << strEntryPath;
    PDU* resPdu = NULL;
    QDir dir(strEntryPath);

    if(!dir.exists()) // 请求文件夹不存在
    {
        resPdu = mkPDU(0);
        strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
    }
    else // 存在
    {
        QFileInfo fileInfo(strEntryPath);
        if(!fileInfo.isDir()) // 不是文件夹
        {
            resPdu = mkPDU(0);
            strncpy(resPdu -> caData, ENTRY_DIR_FAILED, 32);
        }
        else
        {
            resPdu = handleFlushDirRequest(pdu); // 通过该函数获取文件夹下内容
        }
    }
    resPdu -> uiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_RESPOND;
    qDebug() << "1 resPdu -> caData ：" << resPdu -> caData;
    if(strcmp(resPdu -> caData, FLUSH_DIR_OK) == 0)
    {
        strncpy(resPdu -> caData, ENTRY_DIR_OK, 32);
        qDebug() << "2 resPdu -> caData ：" << resPdu -> caData;
    }
    else
    {
        strncpy(resPdu -> caData, ENTRY_DIR_FAILED, 32);
        qDebug() << "2 resPdu -> caData ：" << resPdu -> caData;
    }

    return resPdu;
}

// 返回上一目录请求
PDU* handlePreDirRequest(PDU* pdu)
{
    char strPrePath[pdu -> uiMsgLen]; // 进入文件夹路径
    memcpy(strPrePath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
    qDebug() << "上一目录： " << strPrePath;
    PDU* resPdu = NULL;
    QDir dir(strPrePath);

    if(!dir.exists()) // 请求文件夹不存在
    {
        resPdu = mkPDU(0);
        strncpy(resPdu -> caData, PATH_NOT_EXIST, 32);
    }
    else // 存在
    {
        resPdu = handleFlushDirRequest(pdu); // 通过该函数获取文件夹下内容
    }
    resPdu -> uiMsgType = ENUM_MSG_TYPE_PRE_DIR_RESPOND;
    qDebug() << "1 resPdu -> caData ：" << resPdu -> caData;
    if(strcmp(resPdu -> caData, FLUSH_DIR_OK) == 0)
    {
        strncpy(resPdu -> caData, PRE_DIR_OK, 32);
        qDebug() << "2 resPdu -> caData ：" << resPdu -> caData;
    }
    else
    {
        strncpy(resPdu -> caData, PRE_DIR_FAILED, 32);
        qDebug() << "2 resPdu -> caData ：" << resPdu -> caData;
    }

    return resPdu;
}

// 上传文件请求处理
PDU* handleUploadFileRequest(PDU* pdu, TransFile* transFile)
{
    char caCurPath[pdu -> uiMsgLen];
    char caFileName[32] = {'\0'};
    qint64 fileSize = 0;

    strncpy(caCurPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
    sscanf(pdu -> caData, "%s %lld", caFileName, &fileSize);
    QString strFilePath = QString("%1/%2").arg(caCurPath).arg(caFileName); // 文件路径
    qDebug() << "上传文件路径：" << strFilePath;

    PDU* resPdu = mkPDU(0);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;

    transFile->file.setFileName(strFilePath); // 设置要上传的文件名
    if(transFile->file.open(QIODevice::WriteOnly)) // 以只写的方式打开文件，文件如果不存在会自动创建
    {
        transFile->bTransform = true; // 正在上传文件状态
        transFile->iTotalSize = fileSize;
        transFile->iReceivedSize = 0;

        memcpy(resPdu -> caData, UPLOAD_FILE_START, 32);
    }
    else // 打开文件失败
    {
        memcpy(resPdu -> caData, UPLOAD_FILE_FAILED, 32);
    }

    return resPdu;
}

// 下载文件请求处理
PDU* handleDownloadFileRequest(PDU* pdu, QFile *fDownloadFile, QTimer *pTimer)
{
    char caFileName[32] = {'\0'};
    char caCurPath[pdu -> uiMsgLen];
    memcpy(caFileName, pdu -> caData, 32);
    memcpy(caCurPath, (char*)pdu -> caMsg, pdu -> uiMsgLen);
    QString strDownloadFilePath = QString("%1/%2").arg(caCurPath).arg(caFileName);
    fDownloadFile->setFileName(strDownloadFilePath);

    qDebug() << "下载文件：" << strDownloadFilePath;
    qint64 fileSize = fDownloadFile -> size();

    PDU *resPdu = NULL;
    if(fDownloadFile->open(QIODevice::ReadOnly))
    {
        resPdu = mkPDU(32 + sizeof (qint64) + 5);

        resPdu -> uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        strncpy(resPdu -> caData, DOWNLOAD_FILE_START, 32);
        sprintf((char*)resPdu -> caMsg, "%s %lld", caFileName, fileSize);
        pTimer -> start(1000); // 开始计时器1000ms
        qDebug() << (char*)resPdu -> caMsg;
    }
    else // 打开文件失败
    {
        resPdu = mkPDU(0);

        resPdu -> uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        strncpy(resPdu -> caData, DOWNLOAD_FILE_FAILED, 32);
    }

    return resPdu;
}

// 移动文件请求处理
PDU* handleMoveFileRequest(PDU* pdu)
{
    char caMoveFileName[32]; // 要移动文件名
    int iOldDirSize = 0;
    int iDesDirSize = 0;
    sscanf(pdu -> caData, "%s %d %d", caMoveFileName, &iDesDirSize, &iOldDirSize);
    char caOldDir[iOldDirSize + 33]; // +33是为了拼接文件名
    char caDesDir[iDesDirSize + 33];
    sscanf((char*)pdu -> caMsg, "%s %s", caDesDir, caOldDir);
    qDebug() << "移动文件：" << caMoveFileName << "从" << caOldDir << "到" << caDesDir;

    QFileInfo fileInfo(caDesDir);
    PDU* resPdu = mkPDU(0);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;

    if(!fileInfo.isDir())
    {
        strncpy(resPdu -> caData, MOVE_FILE_FAILED, 32);
        return resPdu;
    }

    // 拼接文件名
    strcat(caOldDir, "/");
    strcat(caOldDir, caMoveFileName);
    strcat(caDesDir, "/");
    strcat(caDesDir, caMoveFileName);
    if (QFile::rename(caOldDir, caDesDir)) // 移动
    {
        strncpy(resPdu -> caData, MOVE_FILE_OK, 32);
    }
    else
    {
        strncpy(resPdu -> caData, MOVE_FILE_FAILED, 32);
    }

    return resPdu;
}

// 分享文件请求处理
PDU* handleShareFileRequest(PDU* pdu, QString strSouName)
{
    int iUserNum = 0; // 分享好友数
    char caFileName[32]; // 分享的文件名
    sscanf(pdu -> caData, "%s %d", caFileName, &iUserNum);
    qDebug() << "分享文件：" << caFileName << " 人数：" << iUserNum;

    // 转发给被分享的好友分享文件通知
    const int iFilePathLen = pdu->uiMsgLen - iUserNum * 32;
    char caFilePath[iFilePathLen];
    PDU* resPdu = mkPDU(iFilePathLen);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
    memcpy(resPdu -> caData, strSouName.toStdString().c_str(), strSouName.size()); // 发送方
    memcpy(resPdu -> caData + 32, caFileName, 32); // 发送文件名
    memcpy(caFilePath, (char*)(pdu -> caMsg) + 32 * iUserNum, iFilePathLen);
    memcpy((char*)resPdu -> caMsg, caFilePath, iFilePathLen); // 发送文件路径
    // 遍历分享所有要接收文件的好友
    char caDesName[32]; // 目标好友名
    for(int i = 0; i < iUserNum; ++ i)
    {
        memcpy(caDesName, (char*)(pdu -> caMsg) + 32 * i, 32);
        MyTcpServer::getInstance().forwardMsg(caDesName, resPdu);
        qDebug() << caDesName;
    }
    free(resPdu);
    resPdu = NULL;

    // 回复发送方消息
    resPdu = mkPDU(0);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
    strncpy(resPdu -> caData, SHARE_FILE_OK, 32);

    return resPdu;
}

// 复制文件夹
bool copyDir(QString strOldPath, QString strNewPath)
{
    int ret = true;
    QDir dir; // 目录操作

    qDebug() << "分享目录：" << strOldPath << " " << strNewPath;
    dir.mkdir(strNewPath); // 新路径创建空目录
    dir.setPath(strOldPath); // 设置为源目录
    QFileInfoList fileInfoList = dir.entryInfoList(); // 获得源目录下文件列表
    // 对源目录下所有文件（分为普通文件、文件夹）进行递归拷贝
    QString strOldFile;
    QString strNewFile;
    for(QFileInfo fileInfo:fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
        { // 注意不要忘记这个判断，"."和".."文件夹不用复制，不然会死循环
            continue;
        }
        strOldFile = QString("%1/%2").arg(strOldPath).arg(fileInfo.fileName());
        strNewFile = QString("%1/%2").arg(strNewPath).arg(fileInfo.fileName());
        if(fileInfo.isFile())
        {
            ret = ret && QFile::copy(strOldFile, strNewFile);
        }
        else if(fileInfo.isDir())
        {
            ret = ret && copyDir(strOldFile, strNewFile);
        }
        qDebug() << strOldFile << " -> " << strNewFile;
    }

    return ret;
}

// 分享文件通知响应处理
PDU* handleShareFileNoteRespond(PDU *pdu)
{
    int iOldPathLen = 0;
    int iNewPathLen = 0;
    sscanf(pdu -> caData, "%d %d", &iOldPathLen, &iNewPathLen);
    char caOldPath[iOldPathLen];
    char caNewDir[iNewPathLen];
    sscanf((char*)pdu -> caMsg, "%s %s", caOldPath, caNewDir);

    // 获得文件新的路径
    char *pIndex = strrchr(caOldPath, '/'); // 获得最右侧的/的指针，找到文件名
    QString strNewPath = QString("%1/%2").arg(caNewDir).arg(pIndex + 1);
    qDebug() << "同意分享文件：" << caOldPath << " " << strNewPath;

    QFileInfo fileInfo(caOldPath);
    bool ret = false;
    if(fileInfo.isFile())
    {
        ret = QFile::copy(caOldPath, strNewPath);
    }
    else if(fileInfo.isDir())
    {
        ret = copyDir(caOldPath, strNewPath);
    }
    else
    {
        ret = false;
    }
    // 回复接收方
    PDU* resPdu = mkPDU(0);
    resPdu -> uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
    if(ret)
    {
        memcpy(resPdu -> caData, SHARE_FILE_OK, 32);
    }
    else
    {
        memcpy(resPdu -> caData, SHARE_FILE_FAILED, 32);
    }

    return resPdu;
}

void MyTcpSocket::receiveMsg()
{
    // 所处状态是接收文件
    if(m_uploadFile->bTransform)
    {
        // 接收数据
        QByteArray baBuffer = this -> readAll();
        m_uploadFile->file.write(baBuffer); // 文件在上一个请求已经打开了
        m_uploadFile->iReceivedSize += baBuffer.size();
        PDU* resPdu = NULL;

        qDebug() << "上传文件中：" << m_uploadFile->iReceivedSize;

        if(m_uploadFile->iReceivedSize == m_uploadFile->iTotalSize)
        {
            m_uploadFile->file.close(); // 关闭文件
            m_uploadFile->bTransform = false;

            resPdu = mkPDU(0);
            resPdu -> uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strncpy(resPdu -> caData, UPLOAD_FILE_OK, 32);
        }
        else if(m_uploadFile -> iReceivedSize > m_uploadFile->iTotalSize)
        {
            m_uploadFile->file.close(); // 关闭文件
            m_uploadFile->bTransform = false;

            resPdu = mkPDU(0);
            resPdu -> uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strncpy(resPdu -> caData, UPLOAD_FILE_FAILED, 32);
        }

        // 响应客户端
        if(NULL != resPdu)
        {
            // qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;
            this -> write((char*)resPdu, resPdu -> uiPDULen);
            // 释放空间
            free(resPdu);
            resPdu = NULL;
        }

        return ;
    }

    // 所处状态不是接收文件，接收到的是非文件传输的请求
    // qDebug() << this -> bytesAvailable(); // 输出接收到的数据大小
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
    case ENUM_MSG_TYPE_ONLINE_USERS_REQUEST: // 查询所有在线用户请求
    {
        resPdu = handleOnlineUsersRequest();
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USER_REQUEST: // 查找用户请求
    {
        resPdu = handleSearchUserRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST: // 添加好友请求
    {
        resPdu = handleAddFriendRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: // 同意加好友
    {
        handleAddFriendAgree(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REJECT: // 拒绝加好友
    {
        handleAddFriendReject(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST: // 刷新好友请求
    {
        resPdu = handleFlushFriendRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST: // 删除好友请求
    {
        resPdu = handleDeleteFriendRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST: // 私聊请求
    {
        resPdu = handlePrivateChatRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST: // 群聊请求
    {
        handleGroupChatRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST: // 创建文件夹请求
    {
        resPdu = handleCreateDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_DIR_REQUEST: // 刷新文件夹请求
    {
        resPdu = handleFlushDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_REQUEST: // 删除文件请求
    {
        resPdu = handleDelFileOrDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST: // 重命名文件请求
    {
        resPdu = handleRenameFileRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ENTRY_DIR_REQUEST: // 进入文件夹请求
    {
        resPdu = handleEntryDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_PRE_DIR_REQUEST: // 上一目录请求
    {
        resPdu = handlePreDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST: // 上传文件请求
    {
        resPdu = handleUploadFileRequest(pdu, m_uploadFile);
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST: // 下载文件请求
    {
        resPdu = handleDownloadFileRequest(pdu, m_pDownloadFile, m_pTimer);
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST: // 移动文件请求
    {
        resPdu = handleMoveFileRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST: // 分享文件请求
    {
        resPdu = handleShareFileRequest(pdu, m_strName);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND: // 分享文件通知响应处理
    {
        resPdu = handleShareFileNoteRespond(pdu);
        break;
    }
    default:
        break;
    }

    // 响应客户端
    if(NULL != resPdu)
    {
        // qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;
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

void MyTcpSocket::handledownloadFileData()
{
    m_pTimer->stop(); // 停止计时器
    // 循环传输数据
    char *pBuffer = new char[4096];
    qint64 iActualSize = 0; // 实际读取文件大小

    while(true)
    {
        iActualSize = m_pDownloadFile->read(pBuffer, 4096);
        if (iActualSize > 0 && iActualSize <= 4096)
        {
            this -> write(pBuffer, iActualSize);
        }
        else if (iActualSize == 0)
        { // 发送完成
            break;
        }
        else
        {
            qDebug() << "发送文件数据给客户端出错！";
            break;
        }
    }

    m_pDownloadFile -> close(); // 关闭文件
    delete [] pBuffer;
    pBuffer = NULL;
    m_pDownloadFile->setFileName(""); // 清除上传文件夹名，以免影响之后上传操作
}



