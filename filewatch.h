#ifndef FILEWATCH_H
#define FILEWATCH_H


#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QFileSystemWatcher>
#include <QDir>
#include <QDebug>
#include <QObject>
#include <iostream>
#include <functional>

#include <jsonresult.h>


class filewatch : public QFileSystemWatcher
{
    Q_OBJECT
public:
    filewatch(const QString &dir, QLabel *label, QWidget *parent = nullptr):QFileSystemWatcher(parent), qlabel(label){
        _dir=dir;
        addPath(_dir); // 使用基类的addPath方法
        connect(this, &QFileSystemWatcher::directoryChanged, this, &filewatch::directoryChanged);
    }

    filewatch(const QString &dir, std::function<void()> callback)
     :_callback(callback)
    {
        _dir=dir;
        addPath(_dir);
        connect(this, &QFileSystemWatcher::directoryChanged, this, &filewatch::directoryChangedHandler);
    }


    void setDirectory(const QString &path);
    void delAllFile(void);
    ~filewatch();
private slots:
   void directoryChanged(const QString &path);
   void directoryChangedHandler(void);

private:
    QLabel *qlabel;
    QString _dir;
    std::function<void()> _callback;
};


#endif // FILEWATCH_H
