#include "filewatch.h"

void filewatch::directoryChanged(const QString &path){
    QDir dir(path);
    QStringList filters;
    filters << "*.jpg" << "*.jpeg" << "*.png"; // 过滤图片文件类型
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    // 获取最新添加的图片文件
    if (!files.isEmpty()) {
        QString newestImagePath = files.last().absoluteFilePath();
        qDebug() << "New image detected: " << newestImagePath;

        // 加载并显示图片
        QPixmap pixmap(newestImagePath);
        qlabel->setAlignment(Qt::AlignCenter);
        //qlabel->setScaledContents(true); // 设置QLabel自动缩放内容以适应其大小
        qlabel->setPixmap(pixmap.scaled(qlabel->size(), Qt::KeepAspectRatio));
        //qlabel->setFixedSize(pixmap.width(), pixmap.height());

    }
}

void filewatch::directoryChangedHandler(void){
    std::cout << "Directory json changed " <<std::endl;
    _callback();
}

void filewatch::setDirectory(const QString &path){

    removePath(_dir);
    _dir=path;
    addPath(_dir);
    disconnect(this, &QFileSystemWatcher::directoryChanged, this, &filewatch::directoryChanged);
    connect(this, &QFileSystemWatcher::directoryChanged, this, &filewatch::directoryChanged);
}

void filewatch::delAllFile(void){
    QDir directory(_dir);
    // 确保目录存在
    if (!directory.exists()) {
        qWarning() << "Directory does not exist:" << _dir;
        return;
    }

    // 获取目录中的所有文件
    QStringList fileList = directory.entryList(QDir::Files);

    // 遍历文件列表并删除每个文件
    for (const QString &fileName : fileList) {
        QFile file(directory.filePath(fileName));
        if (file.exists()) {
            if (file.remove()) {
                qDebug() << "Removed file:" << fileName;
            } else {
                qWarning() << "Failed to remove file:" << fileName;
            }
        }
    }
}

filewatch::~filewatch() {
    // 如果需要，在这里清理资源
}
