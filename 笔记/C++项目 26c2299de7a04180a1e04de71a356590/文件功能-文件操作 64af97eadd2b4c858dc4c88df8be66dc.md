# 文件功能-文件操作

# 文件重命名

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled.png)

基本实现逻辑与其他操作相同，这里只展示服务器mytcpsocket接收请求之后的处理的代码。

```cpp
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
```

# 进入文件夹

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%201.png)

1. 客户端发送请求

FileSystem

```cpp
void FileSystem::entryDir(const QModelIndex &index)
{
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QString strFileName = index.data().toString();
    strFileName = strFileName.split('\t')[0]; // 获得双击的文件名
    QString strEntryPath = QString("%1/%2").arg(strCurPath).arg(strFileName);
    qDebug() << "进入 " << strEntryPath;
    m_strTryEntryDir = strEntryPath; // 将想要进入的目录临时存储下来
    PDU* pdu = mkPDU(strEntryPath.size() + 1);

    pdu -> uiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_REQUEST;
    memcpy((char*)pdu -> caMsg, strEntryPath.toStdString().c_str(), strEntryPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}
```

1. 服务器处理请求

mytcpsocket

```cpp
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
```

1. 客户端接收响应

tcpclient

```cpp
case ENUM_MSG_TYPE_ENTRY_DIR_RESPOND: // 进入文件夹响应
    {
        qDebug() << "进入文件夹响应：" << pdu -> caData;
        if(strcmp(ENTRY_DIR_OK, pdu -> caData) == 0)
        {
            OperateWidget::getInstance().getPFileSystem() -> updateFileList(pdu); // 刷新文件列表
            QString entryPath = OperateWidget::getInstance().getPFileSystem()->strTryEntryDir();
            if(!entryPath.isEmpty())
            {
                m_strCurPath = entryPath;
                OperateWidget::getInstance().getPFileSystem()->clearStrTryEntryDir(); // 清空m_strTryEntryDir
                qDebug() << "当前路径：" << m_strCurPath;
            }
        }
        else
        {
            QMessageBox::warning(this, "进入文件夹", pdu -> caData);
        }
        break;
    }
```

# 返回上一级

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%202.png)

1. 客户端发送请求

FileSystem

```cpp
void FileSystem::returnPreDir()
{
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QString strRootPath = TcpClient::getInstance().getStrRootPath();

    if(strCurPath == strRootPath)
    {
        QMessageBox::warning(this, "返回上一目录", "已经是根目录！");
        return ;
    }
    int index = strCurPath.lastIndexOf("/");
    strCurPath = strCurPath.remove(index, strCurPath.size() - index);
    qDebug() << "返回到" << strCurPath;
    m_strTryEntryDir = strCurPath; // 临时存储目标目录

    // 给服务器发消息
    PDU* pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_PRE_DIR_REQUEST;
    memcpy((char*)pdu -> caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}
```

1. 服务器处理请求

mytcpsocket

```cpp
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
```

1. 客户端接收响应

tcpclient

```cpp
case ENUM_MSG_TYPE_PRE_DIR_RESPOND: // 上一目录响应
    {
        qDebug() << "上一文件夹响应：" << pdu -> caData;
        if(strcmp(PRE_DIR_OK, pdu -> caData) == 0)
        {
            OperateWidget::getInstance().getPFileSystem() -> updateFileList(pdu); // 刷新文件列表
            QString entryPath = OperateWidget::getInstance().getPFileSystem()->strTryEntryDir();
            if(!entryPath.isEmpty())
            {
                m_strCurPath = entryPath;
                OperateWidget::getInstance().getPFileSystem()->clearStrTryEntryDir(); // 清空m_strTryEntryDir
                qDebug() << "当前路径：" << m_strCurPath;
            }
        }
        else
        {
            QMessageBox::warning(this, "上一文件夹", pdu -> caData);
        }
        break;
    }
```

# 上传文件

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%203.png)

注意，上传文件需要分为两次请求：

1. 客户端发送”当前路径和上传文件名“，服务器接收数据然后创建文件，再响应客户端
2. 客户端收到服务器响应无误之后，再上传文件内容

首先，客户端fileSystem实现上传文件按钮和对应槽函数

```cpp
void FileSystem::uploadFile()
{
    QString strCurPath = TcpClient::getInstance().getStrCurPath(); // 当前目录
    m_strUploadFilePath = QFileDialog::getOpenFileName(); // 将上传的文件的路径
    qDebug() << m_strUploadFilePath;

    if(m_strUploadFilePath.isEmpty())
    {
        QMessageBox::warning(this, "上传文件", "请选择需要上传的文件！");
        return ;
    }
    // 获取上传文件的名字作为远程服务器新建文件的名字
    int index = m_strUploadFilePath.lastIndexOf('/');
    QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);
    // 获得文件大小
    QFile file(m_strUploadFilePath);
    qint64 fileSize = file.size(); // 获得文件大小

    qDebug() << "上传文件：" << strFileName << " " << fileSize;
    PDU* pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu -> caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    sprintf(pdu -> caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}
```

服务器接收到上传文件请求之后，将文件的名字、大小、路径等信息进行保存，然后设置状态为接收上传文件，便于之后接收文件。

mytcpsocket

```cpp
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
```

然后客户端如果接收到UPLOAD_FILE_START的PDU，那么就开始一个定时器（1000ms），定时器时间到的信号timeout绑定了传输文件数据的函数，实际进行文件数据传输。

tcpClient

```cpp
case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND: // 上传文件响应
    {
        if(strcmp(UPLOAD_FILE_START, pdu -> caData) == 0) // 开始上传文件数据内容
        {
            OperateWidget::getInstance().getPFileSystem()->startTimer();
        }
        else if 
						...
        break;
    }
```

fileSystem

```cpp
// FileSystem构造函数中添加：
connect(m_pTimer, SIGNAL(timeout()),   // 时间间隔之后再上传文件，防止粘包
        this, SLOT(uploadFileData()));
```

```cpp
void FileSystem::startTimer()
{
    m_pTimer -> start(1000); // 1000ms
}

void FileSystem::uploadFileData()
{
    m_pTimer->stop(); // 关闭定时器，不然定时器会重新计时

    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)) // 只读形式打开文件
    {
        // 打开失败
        QMessageBox::warning(this, "打开文件", "打开文件失败！");
    }
    // 二进制形式传输文件
    char *pBuffer = new char[4096]; // 4096个字节读写效率更高
    qint64 iActualSize = 0;         // 实际读取文件内容大小

    while(true)
    {
        iActualSize = file.read(pBuffer, 4096); // 读数据，返回值是实际读取数据大小
        if (iActualSize > 0 && iActualSize <= 4096)
        {
            TcpClient::getInstance().getTcpSocket().write(pBuffer, iActualSize);
        }
        else if (iActualSize == 0)
        { // 发送完成
            break;
        }
        else
        {
            QMessageBox::warning(this, "上传文件", "上传失败！");
            break;
        }
    }
    file.close();
    delete [] pBuffer;
    pBuffer = NULL;
    m_strUploadFilePath.clear(); // 清楚上传文件夹名，以免影响之后上传操作
}
```

服务器的myTcpSocket进行改变，当处于上传文件状态时，采用readAll()来接收数据。

```cpp
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
    ... // 其他请求的处理代码
}
```

客户端再进行接收resPdu

tcpClient

```cpp
case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND: // 上传文件响应
    {
        if(strcmp(UPLOAD_FILE_START, pdu -> caData) == 0) // 开始上传文件数据内容
        {
            ...
        }
        else if(strcmp(UPLOAD_FILE_OK, pdu -> caData) == 0) // 上传文件成功
        {
            QMessageBox::information(this, "上传文件", pdu -> caData);
        }
        else if(strcmp(UPLOAD_FILE_FAILED, pdu -> caData) == 0) // 上传失败
        {
            QMessageBox::warning(this, "上传文件", pdu -> caData);
        }
        break;
    }
```

# 下载文件

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%204.png)

1. 客户端发送请求，传输当前路径，下载文件名，选择下载位置以及保存所用名，然后发送给服务器

fileSystem

```cpp
void FileSystem::downloadFile()
{
    QListWidgetItem *pItem = m_pFileListW->currentItem(); // 选择要下载的文件
    if(NULL == pItem)
    {
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件！");
        return ;
    }
    // 获取保存的位置
    QString strDownloadFilePath = QFileDialog::getSaveFileName();
    if(strDownloadFilePath.isEmpty())
    {
        QMessageBox::warning(this, "下载文件", "请指定下载文件的位置！");
        m_downloadFile->file.setFileName(""); // 清空
        return ;
    }

    m_downloadFile->file.setFileName(strDownloadFilePath);

    QString strCurPath = TcpClient::getInstance().getStrCurPath(); // 当前路径
    QString strFileName = pItem->text().split('\t')[0]; // 获取文件名
    PDU* pdu = mkPDU(strCurPath.size() + 1);
    pdu -> uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    memcpy((char*)pdu -> caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    strncpy(pdu -> caData, strFileName.toStdString().c_str(), strFileName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    qDebug() << "下载文件：" << pdu -> caData;

    free(pdu);
    pdu = NULL;
}
```

1. 服务器获得客户端请求，判断其合理性，然后打开所要下载的文件，同时设置计时器（1s后开始传输），然后通知客户端。

myTcpSocket

```cpp
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
```

1. 客户端收到服务器响应，设置自己状态为下载文件bDownloadFile，同时需要实现接收文件逻辑

tcpClient

```cpp
void TcpClient::receiveMsg()
{
    // 如果处于接收文件数据的状态
    TransFile *transFile = OperateWidget::getInstance().getPFileSystem()->getDownloadFileInfo();
    if(transFile->bTransform)
    {
        QByteArray baBuffer = m_tcpSocket.readAll();
        transFile->file.write(baBuffer);

        transFile->iReceivedSize += baBuffer.size();
        if(transFile->iReceivedSize == transFile->iTotalSize)
        {

            QMessageBox::information(this, "下载文件", "下载文件成功！");
            transFile->file.close();
            transFile->file.setFileName("");
            transFile->bTransform = false;
            transFile->iTotalSize = 0;
            transFile->iReceivedSize = 0;
        }
        else if(transFile->iReceivedSize > transFile->iTotalSize)
        {
            QMessageBox::warning(this, "下载文件", "下载文件失败！");
            transFile->file.close();
            transFile->file.setFileName("");
            transFile->bTransform = false;
            transFile->iTotalSize = 0;
            transFile->iReceivedSize = 0;
        }
        return ;
    }

    // 否则，处理其他响应PDU
    ...
    // 根据不同消息类型，执行不同操作
    switch(pdu -> uiMsgType)
    {
    ...
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND: // 下载文件响应
    {
        if(strcmp(DOWNLOAD_FILE_START, pdu -> caData) == 0) // 开始下载文件数据内容
        {
            // TransFile *transFile = OperateWidget::getInstance().getPFileSystem()->getDownloadFileInfo();
            qint64 ifileSize = 0;
            char strFileName[32];
            sscanf((char*)pdu -> caMsg, "%s %lld", strFileName, &ifileSize);
            qDebug() << "下载文件中：" << strFileName << ifileSize;

            if(strlen(strFileName) > 0 && transFile->file.open(QIODevice::WriteOnly))
            {
                transFile->bTransform = true;
                transFile->iTotalSize = ifileSize;
                transFile->iReceivedSize = 0;
            }
            else
            {
                QMessageBox::warning(this, "下载文件", "下载文件失败！");
            }
        }
        else if(strcmp(DOWNLOAD_FILE_OK, pdu -> caData) == 0) // 下载文件成功
        {
            QMessageBox::information(this, "下载文件", pdu -> caData);
        }
        else if(strcmp(DOWNLOAD_FILE_FAILED, pdu -> caData) == 0) // 下载失败
        {
            QMessageBox::warning(this, "下载文件", pdu -> caData);
        }
        break;
    }
    default:
        break;
    }

    // 释放空间
    free(pdu);
    pdu = NULL;
}
```

1. 服务器实现传输文件逻辑槽函数，计时器结束之后触发槽函数

myTcpSocket

```cpp
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
```

# 移动文件

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%205.png)

1. 客户端需要提供两个按钮，一个是“移动文件”，选中需要移动的文件之后，点击该按钮，可以设置需要移动的文件；另一个是“目标目录”（默认状态不可点击，需要点击过“移动文件”按钮之后才可以点击），在选中需要移动的文件之后，再跳转到想要移动的目标目录，进入目标目录之后再点击该按钮，即可设置移动的目标目录，然后向服务器发送请求。

fileSystem

```cpp
void FileSystem::moveFile()
{
    QListWidgetItem *pItem = m_pFileListW->currentItem();
    if(pItem == NULL)
    {
        QMessageBox::warning(this, "移动文件", "请选择需要移动的文件！");
        return ;
    }

    m_strMoveFileName = pItem -> text().split('\t')[0]; // 设置需要移动的文件名
    m_strMoveOldDir = TcpClient::getInstance().getStrCurPath(); // 设置移动文件的原目录

    m_pMoveDesDirDB->setEnabled(true); // 设置目标目录可点击
    QMessageBox::information(this, "移动文件", "请跳转到需要移动到的目录，\n然后点击“目标目录”按钮。");
}

void FileSystem::moveDesDir()
{
    QString strDesDir = TcpClient::getInstance().getStrCurPath(); // 设置移动文件的目标目录
    QMessageBox::StandardButton sbMoveAffirm; // 确认弹框返回值
    QString strMoveAffirm = QString("您确认将 %1 的 %2 文件\n移动到 %3 目录下吗？")
            .arg(m_strMoveOldDir).arg(m_strMoveFileName).arg(strDesDir);
    sbMoveAffirm = QMessageBox::question(this, "移动文件", strMoveAffirm);
    if(sbMoveAffirm == QMessageBox::No) // 不移动
    {
        m_strMoveOldDir.clear();
        m_strMoveFileName.clear();
        m_pMoveDesDirDB->setEnabled(false);
        return ;
    }

    qDebug() << "移动文件：" << strMoveAffirm;
    // 确认移动文件
    PDU *pdu = mkPDU(strDesDir.size() + m_strMoveOldDir.size() + 5);

    pdu -> uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    sprintf((char*)pdu -> caMsg, "%s %s", strDesDir.toStdString().c_str(),
            m_strMoveOldDir.toStdString().c_str());
    sprintf(pdu -> caData, "%s %d %d", m_strMoveFileName.toStdString().c_str(), strDesDir.size(), m_strMoveOldDir.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;

    m_strMoveOldDir.clear();
    m_strMoveFileName.clear();
    m_pMoveDesDirDB->setEnabled(false);
}
```

1. 服务器接收到请求之后，对文件进行移动，并返回移动结果

myTcpSocket

```cpp
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
```

1. 客户端接收响应并提示用户。

# 分享文件

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%206.png)

## 界面设计

选中需要分享的文件之后，用户需要选择分享的好友对象，所以我们需要设计一个选择好友的页面。

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%207.png)

代码实现

```cpp
sharedFileFriendList::sharedFileFriendList(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB = new QPushButton("全选"); // 全选
    m_pCancleSelectPB = new QPushButton("清空"); // 取消选择

    m_pAffirmPB = new QPushButton("确认"); // 确认键
    m_pCanclePB = new QPushButton("取消"); // 取消键

    m_pFriendsSA = new QScrollArea; // 展示好友区
    m_pFriendsWid = new QWidget; // 所有好友窗口
    m_pFriendsVBL = new QVBoxLayout(m_pFriendsWid); // 好友信息垂直布局
    m_pFriendsBG = new QButtonGroup(m_pFriendsWid); // bg主要用来管理好友选项，Wid设置为其父类
    m_pFriendsBG->setExclusive(false); // 可以多选

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addStretch(); // 添加弹簧
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancleSelectPB);

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pAffirmPB);
    pDownHBL->addWidget(m_pCanclePB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pFriendsSA); // SA中放置Wid，Wid是BG的父类
    pMainVBL->addLayout(pDownHBL);

    setLayout(pMainVBL);

    connect(m_pSelectAllPB, SIGNAL(clicked(bool)),
            this, SLOT(selectAll()));
    connect(m_pCancleSelectPB, SIGNAL(clicked(bool)),
            this, SLOT(cancleSelect()));
    connect(m_pAffirmPB, SIGNAL(clicked(bool)),
            this, SLOT(affirmShare()));
    connect(m_pCanclePB, SIGNAL(clicked(bool)),
            this, SLOT(cancleShare()));
}
```

最后实现效果

![Untitled](%E6%96%87%E4%BB%B6%E5%8A%9F%E8%83%BD-%E6%96%87%E4%BB%B6%E6%93%8D%E4%BD%9C%2064af97eadd2b4c858dc4c88df8be66dc/Untitled%208.png)

全选按钮键实现逻辑：

```cpp
void sharedFileFriendList::selectAll()
{
    QList<QAbstractButton*> friendsButtons = m_pFriendsBG->buttons();

    for(QAbstractButton* pItem:friendsButtons)
    {
        pItem->setChecked(true);
    }
}
```

取消选择按钮实现逻辑：

```cpp
void sharedFileFriendList::cancleSelect()
{
    QList<QAbstractButton*> friendsButtons = m_pFriendsBG->buttons();

    for(QAbstractButton* pItem:friendsButtons)
    {
        pItem->setChecked(false);
    }
}
```

需要提供当客户点击fileSystem中的”分享文件“按钮之后更新可选择好友列表的代码：

```cpp
void sharedFileFriendList::updateFriendList(QListWidget *pFriendList)
{
    if(NULL == pFriendList)
    {
        return ;
    }

    // 移除之前的好友列表
    QList<QAbstractButton*> preFriendList = m_pFriendsBG->buttons();
    for(QAbstractButton* pItem:preFriendList)
    {
        m_pFriendsVBL->removeWidget(pItem);
        m_pFriendsBG->removeButton(pItem);
        delete pItem;
        pItem = NULL;
    }
    // 设置新的好友列表
    QCheckBox *pCB = NULL;
    for(int i = 0; i < pFriendList->count(); i ++)
    {
        qDebug() << "好友：" << pFriendList->item(i)->text();
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendsVBL->addWidget(pCB);
        m_pFriendsBG->addButton(pCB);
    }
    m_pFriendsSA->setWidget(m_pFriendsWid); // 每次都需要重新设置SA的Widget！
}
```

用户点击确认键之后发送请求分享消息。

其中需要包含：

1. 要分享的文件的文件名、文件路径
2. 要分享的好友名，好友数量

```cpp
void sharedFileFriendList::affirmShare()
{
    QString strFileName = OperateWidget::getInstance().getPFileSystem()->getStrSharedFileName();
    QString strFilePath = OperateWidget::getInstance().getPFileSystem()->getStrSharedFilePath();

    QList<QAbstractButton*> abList = m_pFriendsBG->buttons();
    QList<QString> userList; // 要分享的好友列表

    for(int i = 0; i < abList.count(); ++ i)
    {
        if(abList[i]->isChecked())
        {
            userList.append(abList[i]->text().split('\t')[0]);
        }
    }
    int iUserNum = userList.count();
    qDebug() << "分享好友：" << userList << " " << iUserNum;

    PDU* pdu = mkPDU(strFilePath.size() + userList.count() * 32 + 1); // caMsg中存放文件路径、分享好友名
    pdu -> uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    for(int i = 0; i < iUserNum; ++ i)
    {
        strncpy((char*)(pdu -> caMsg) + 32 * i, userList[i].toStdString().c_str(), 32);
    }
    memcpy((char*)(pdu -> caMsg) + 32 * iUserNum, strFilePath.toStdString().c_str(), strFilePath.size());

    sprintf(pdu -> caData, "%s %d", strFileName.toStdString().c_str(), iUserNum); // caData存放文件名、分享好友数

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;

    this -> hide();
}
```

## 逻辑实现

1. 分享文件的源客户端，实现点击”分享文件“按钮之后更新”shareFileFriendList”页面的好友列表并弹出该页面，同时设置分享文件的文件名和路径。

fileSystem

```cpp
void FileSystem::shareFile()
{
    // 获取要分享文件的信息
    QListWidgetItem *pFileItem = m_pFileListW->currentItem();
    if(NULL == pFileItem)
    {
        QMessageBox::warning(this, "分享文件", "请选择要分享的文件！");
        return ;
    }
    m_strSharedFileName = pFileItem->text().split('\t')[0]; // 要分享文件名
    m_strSharedFilePath = QString("%1/%2").arg(TcpClient::getInstance().getStrCurPath())
            .arg(m_strSharedFileName);
    qDebug() << "分享文件：" << m_strSharedFilePath;

    // 获得好友列表
    QListWidget *friendLW = OperateWidget::getInstance().getPFriend()->getPFriendLW();
    // 选择好友窗口展示
    m_pSharedFileFLW->updateFriendList(friendLW);
    if(m_pSharedFileFLW->isHidden()) // 如果窗口隐藏，则显示出来
    {
        m_pSharedFileFLW->show();
    }

}
```

当客户利用shareFileFriendList页面中的确认分享按钮之后，客户端向服务器发送包含了分享文件名、分享文件路径、分享好友、分享好友数等信息的PDU。

1. 服务端接收到消息之后，需要解析出其中分享的文件名和文件路径，以及分享好友，将要分享的文件名和路径发送给对应的好友。

myTcpSocket

```cpp
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
```

1. 发送分享文件的源客户会收到服务器的响应。

tcpClient

```cpp
case ENUM_MSG_TYPE_SHARE_FILE_RESPOND: // 分享文件响应
    {
        QMessageBox::information(this, "分享文件", pdu -> caData);
        break;
    }
```

1. 被分享文件的所有目的客户都会收到服务器发送的NOTE的PDU，询问是否接收文件，如果选择接收文件，则会向服务器发送确认报文。

tcpClient

```cpp
case ENUM_MSG_TYPE_SHARE_FILE_NOTE: // 被分享文件提醒
    {
        char caFileName[32]; // 文件名
        char caSouName[32]; // 用户名
        int iFilePathLen = pdu -> uiMsgLen;
        char caFilePath[iFilePathLen]; // 文件路径

        memcpy(caSouName, pdu -> caData, 32);
        memcpy(caFileName, pdu -> caData + 32, 32);
        QString strShareNote = QString("%1 想要分享 %2 文件给您，\n是否接收？").arg(caSouName).arg(caFileName);
        QMessageBox::StandardButton sbShareNote = QMessageBox::question(this, "分享文件", strShareNote);
        if(sbShareNote == QMessageBox::No)
        { // 拒绝接收
            break;
        }

        // 同意接收
        qDebug() << "接收文件：" << caSouName <<" " << caFileName;
        memcpy(caFilePath, (char*)pdu -> caMsg, iFilePathLen);
        QString strRootDir = m_strRootPath; // 用户根目录
        PDU *resPdu = mkPDU(iFilePathLen + strRootDir.size() + 1);
        resPdu -> uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
        sprintf(resPdu -> caData, "%d %d", iFilePathLen, strRootDir.size());
        sprintf((char*)resPdu -> caMsg, "%s %s", caFilePath, strRootDir.toStdString().c_str());
        qDebug() << (char*)resPdu -> caMsg;
        m_tcpSocket.write((char*)resPdu, resPdu -> uiPDULen);
        free(resPdu);
        resPdu = NULL;

        break;
    }
```

1. 服务器接收到被分享方的响应之后，开始实际拷贝文件，对文件与文件夹有不同拷贝策略。并返回被分享方拷贝结果。

myTcpSocket

```cpp
// 工具函数：复制文件夹
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
```

1. 被分享方接收到服务器的响应之后显示结果

```cpp
case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND: // 被分享文件通知响应的处理结果
    {
        QMessageBox::information(this, "分享文件", pdu -> caData);
        break;
    }
```