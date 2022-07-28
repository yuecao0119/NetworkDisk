#include "filesystem.h"
#include <QInputDialog>
#include <QMessageBox>
#include "tcpclient.h"
#include <QListWidgetItem>

FileSystem::FileSystem(QWidget *parent) : QWidget(parent)
{
    m_strTryEntryDir.clear(); // 开始时清空要进入的文件夹的属性值

    m_pFileListW = new QListWidget; // 文件列表，显示所有文件

    m_pReturnPrePB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelFileOrDirPB = new QPushButton("删除");
    m_pFlushDirPB = new QPushButton("刷新文件夹");

    QHBoxLayout *pDirOpVBL = new QHBoxLayout;
    pDirOpVBL -> addWidget(m_pReturnPrePB);
    pDirOpVBL -> addWidget(m_pCreateDirPB);
    pDirOpVBL -> addWidget(m_pFlushDirPB);
    pDirOpVBL -> addWidget(m_pDelFileOrDirPB);

    m_pRenameFilePB = new QPushButton("重命名文件");
    m_pUploadFilePB = new QPushButton("上传文件");
    m_pDownloadFilePB = new QPushButton("下载文件");
    m_pShareFilePB = new QPushButton("分享文件");

    QHBoxLayout *pFileOpVBL = new QHBoxLayout;
    pFileOpVBL -> addWidget(m_pRenameFilePB);
    pFileOpVBL -> addWidget(m_pUploadFilePB);
    pFileOpVBL -> addWidget(m_pDownloadFilePB);
    pFileOpVBL -> addWidget(m_pShareFilePB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL -> addLayout(pDirOpVBL);
    pMainVBL -> addLayout(pFileOpVBL);
    pMainVBL -> addWidget(m_pFileListW);

    setLayout(pMainVBL);

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

QString FileSystem::strTryEntryDir() const
{
    return m_strTryEntryDir;
}

void FileSystem::setStrTryEntryDir(const QString &strTryEntryDir)
{
    m_strTryEntryDir = strTryEntryDir;
}
