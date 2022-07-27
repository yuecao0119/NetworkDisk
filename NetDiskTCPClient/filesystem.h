#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QWidget>
#include <QListWidget> // 显示文件的列表
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"

class FileSystem : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystem(QWidget *parent = nullptr);
    void updateFileList(PDU* pdu);

public slots:
    void createDir();  // 创建文件夹按钮槽函数
    void flushDir();   // 刷新文件夹按钮槽函数

signals:

private:
    QListWidget *m_pFileListW;      // 文件列表，显示所有文件

    QPushButton *m_pReturnPrePB;    // 返回上一路径
    QPushButton *m_pCreateDirPB;    // 创建文件夹
    QPushButton *m_pDeleteDirPB;    // 删除文件夹
    QPushButton *m_pRenameFilePB;   // 重命名文件
    QPushButton *m_pFlushDirPB;     // 刷新文件夹
    QPushButton *m_pUploadFilePB;   // 上传文件
    QPushButton *m_pDownloadFilePB; // 下载文件
    QPushButton *m_pDeleteFilePB;   // 删除文件
    QPushButton *m_pShareFilePB;    // 分享文件

};

#endif // FILESYSTEM_H
