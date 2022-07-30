#include "filesystem.h"
#include <QInputDialog>
#include <QMessageBox>
#include "tcpclient.h"
#include <QListWidgetItem>
#include <QFileDialog>

FileSystem::FileSystem(QWidget *parent) : QWidget(parent)
{
    m_strTryEntryDir.clear(); // 开始时清空要进入的文件夹的属性值
    m_strUploadFilePath.clear();
    m_pTimer = new QTimer;
    m_downloadFile = new TransFile;
    m_downloadFile->bTransform = false;
    m_pFileListW = new QListWidget; // 文件列表，显示所有文件

    m_pReturnPrePB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelFileOrDirPB = new QPushButton("删除");
    m_pFlushDirPB = new QPushButton("刷新文件夹");
    m_pRenameFilePB = new QPushButton("重命名");

    QHBoxLayout *pDirOpVBL = new QHBoxLayout;
    pDirOpVBL -> addWidget(m_pReturnPrePB);
    pDirOpVBL -> addWidget(m_pCreateDirPB);
    pDirOpVBL -> addWidget(m_pFlushDirPB);
    pDirOpVBL -> addWidget(m_pDelFileOrDirPB);
    pDirOpVBL -> addWidget(m_pRenameFilePB);

    m_pUploadFilePB = new QPushButton("上传文件");
    m_pDownloadFilePB = new QPushButton("下载文件");
    m_pShareFilePB = new QPushButton("分享文件");
    m_pMoveFilePB = new QPushButton("移动文件");
    m_pMoveDesDirDB = new QPushButton("目标目录");
    m_pMoveDesDirDB->setEnabled(false); // 设置目标文件不可点击

    QHBoxLayout *pFileOpVBL = new QHBoxLayout;
    pFileOpVBL -> addWidget(m_pUploadFilePB);
    pFileOpVBL -> addWidget(m_pDownloadFilePB);
    pFileOpVBL -> addWidget(m_pShareFilePB);
    pFileOpVBL -> addWidget(m_pMoveFilePB);
    pFileOpVBL -> addWidget(m_pMoveDesDirDB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL -> addLayout(pDirOpVBL);
    pMainVBL -> addLayout(pFileOpVBL);
    pMainVBL -> addWidget(m_pFileListW);

    setLayout(pMainVBL);


    m_pSharedFileFLW = new sharedFileFriendList;

    connect(m_pCreateDirPB, SIGNAL(clicked(bool)),
            this, SLOT(createDir()));
    connect(m_pFlushDirPB, SIGNAL(clicked(bool)),
            this, SLOT(flushDir()));
    connect(m_pDelFileOrDirPB, SIGNAL(clicked(bool)),
            this, SLOT(delFileOrDir()));
    connect(m_pRenameFilePB, SIGNAL(clicked(bool)),
            this, SLOT(renameFile()));
    connect(m_pFileListW, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(entryDir(QModelIndex)));
    connect(m_pReturnPrePB, SIGNAL(clicked(bool)),
            this, SLOT(returnPreDir()));
    connect(m_pUploadFilePB, SIGNAL(clicked(bool)),
            this, SLOT(uploadFile()));
    connect(m_pTimer, SIGNAL(timeout()),   // 时间间隔之后再上传文件，防止粘包
            this, SLOT(uploadFileData()));
    connect(m_pDownloadFilePB, SIGNAL(clicked(bool)),
            this, SLOT(downloadFile()));
    connect(m_pMoveFilePB, SIGNAL(clicked(bool)),
            this, SLOT(moveFile()));
    connect(m_pMoveDesDirDB, SIGNAL(clicked(bool)),
            this, SLOT(moveDesDir()));
    connect(m_pShareFilePB, SIGNAL(clicked(bool)),
            this, SLOT(shareFile()));
}

void FileSystem::updateFileList(PDU *pdu)
{
    if(NULL == pdu)
    {
        return ;
    }
    uint uiFileNum = pdu -> uiMsgLen / sizeof(FileInfo); // 文件数
    FileInfo *pFileInfo = NULL; // 通过FileInfo指针依此访问caMsg中数据
    QListWidgetItem *pItem = NULL;

    m_pFileListW -> clear(); // 清除文件列表原有数据
    for(uint i = 0; i < uiFileNum; ++ i)
    {
        pFileInfo = (FileInfo*)(pdu -> caMsg) + i;
        if(strcmp(pFileInfo -> caName, ".") == 0 || strcmp(pFileInfo -> caName, "..") == 0)
        {   // 不显示 "." ".." 文件夹
            continue;
        }
        pItem = new QListWidgetItem;

        if(pFileInfo ->bIsDir) // 根据文件类型设置图标
        {
            pItem->setIcon(QIcon(QPixmap(":/icon/dir.jpeg")));
        }
        else
        {
            pItem->setIcon(QIcon(QPixmap(":/icon/file.jpeg")));
        }
        // 文件名 文件大小 最后修改时间  形式展示文件
        pItem ->setText(QString("%1\t%2\t%3").arg(pFileInfo->caName)
                        .arg(pFileInfo->uiSize).arg(pFileInfo->caTime));
        m_pFileListW->addItem(pItem);
    }
}

void FileSystem::createDir()
{
    QString strDirName = QInputDialog::getText(this, "新建文件夹", "文件夹名："); // 获得文件夹名
    QString strCurPath = TcpClient::getInstance().getStrCurPath();

    if(!strDirName.isEmpty())
    {
        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu -> uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
        strncpy(pdu -> caData, strDirName.toStdString().c_str(), strDirName.size());
        memcpy((char*)pdu ->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void FileSystem::flushDir()
{
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    PDU *pdu = mkPDU(strCurPath.size() + 1);

    pdu -> uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_REQUEST;
    memcpy((char*)pdu ->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

void FileSystem::delFileOrDir()
{
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QListWidgetItem *qItem = m_pFileListW->currentItem(); // 获得当前选中文件
    if(NULL == qItem)
    {
        QMessageBox::warning(this, "删除文件", "请选中需要删除的文件");
        return ;
    }
    QString strFileName = qItem->text().split('\t')[0]; // 获取文件名
    QString strDelPath = QString("%1/%2").arg(strCurPath).arg(strFileName); // 要删除文件路径
    qDebug() << "删除文件：" << strDelPath;
    PDU *pdu = mkPDU(strDelPath.size() + 1);

    pdu -> uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
    memcpy((char*)pdu ->caMsg, strDelPath.toStdString().c_str(), strDelPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

void FileSystem::renameFile()
{
    QString strCurPath = TcpClient::getInstance().getStrCurPath();
    QListWidgetItem *qItem = m_pFileListW->currentItem(); // 获得当前选中文件
    if(NULL == qItem)
    {
        QMessageBox::warning(this, "重命名文件", "请选择需要重命名的文件！");
        return ;
    }
    QString strOldName = qItem -> text().split('\t')[0]; // 获取旧文件名
    QString strNewName = QInputDialog::getText(this, "文件重命名", "新文件名："); // 新文件名
    qDebug() << "重命名：" << strCurPath << " " << strOldName << " -> " << strNewName;

    if(strNewName.isEmpty())
    {
        QMessageBox::warning(this, "重命名文件", "文件名不能为空！");
        return ;
    }

    PDU *pdu = mkPDU(strCurPath.size() + 1);

    pdu -> uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    // 传输给服务器 旧名字 新名字 目录路径
    strncpy(pdu -> caData, strOldName.toStdString().c_str(), 32);
    strncpy(pdu -> caData + 32, strNewName.toStdString().c_str(), 32);
    memcpy((char*)pdu ->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}

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
    m_strUploadFilePath.clear(); // 清除上传文件夹名，以免影响之后上传操作
}

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

TransFile *FileSystem::getDownloadFileInfo()
{
    return m_downloadFile;
}

QString FileSystem::getStrSharedFileName() const
{
    return m_strSharedFileName;
}

QString FileSystem::getStrSharedFilePath() const
{
    return m_strSharedFilePath;
}


QString FileSystem::strTryEntryDir() const
{
    return m_strTryEntryDir;
}

void FileSystem::setStrTryEntryDir(const QString &strTryEntryDir)
{
    m_strTryEntryDir = strTryEntryDir;
}

void FileSystem::clearStrTryEntryDir()
{
    m_strTryEntryDir.clear();
}
