#ifndef PYPROCESS_H
#define PYPROCESS_H

#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QApplication>
#include <QElapsedTimer>
#include <QDir>
#include <QFile>
#include <QFileInfo>

class pyprocess
{
public:
    pyprocess(const QString &program,const QString &script,const QString &iuputpath,QString &outputpath);

    pyprocess(const QString &program,const QString &script);

    ~pyprocess();

    void startProcess(void);

    void endProcess(void);

    void setProgram(void);

    void setInputPath(const QString &path);

    void setScript(const QString &path);

    void setOutputPath(const QString &path);


public:
    qint64 time;

private:
    QString _script;
    QString _inputpath;
    QString _outputpath;
    QString _modelpath;

    QProcess process;
    QString qprogram;
    QElapsedTimer timer;
    QStringList arguments;
};

#endif // PYPROCESS_H
