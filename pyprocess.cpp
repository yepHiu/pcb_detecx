#include "pyprocess.h"

/**
 * @brief pyprocess::pyprocess
 * @param dir 脚本目录位置
 */
pyprocess::pyprocess(const QString &program,const QString &script,const QString &inputpath,QString &outputpath)
{
    qprogram=program;

    _script=script;
    _inputpath=inputpath;
    _outputpath=outputpath;
    QFileInfo scriptpath(_script);
    QString sss=scriptpath.absolutePath();
    _modelpath=scriptpath.absolutePath()+QDir::separator()+"yolov5n_1684_f32.bmodel";
    //scriptpath.~QFileInfo();

    arguments << _script << "--model" << _modelpath
                << "--input_path" << _inputpath << "--output_dir" << _outputpath;
}

pyprocess::pyprocess(const QString &program,const QString &script)
{
    qprogram=program;

    _script=script;
    QFileInfo scriptpath(_script);
    QString sss=scriptpath.absolutePath();
    //scriptpath.~QFileInfo();

    arguments << _script;
}


pyprocess::~pyprocess(){

}

/**
 * @brief pyprocess::startProcess 启动脚本并计时
 */
void pyprocess::startProcess(void){
    process.start(qprogram,arguments);
    if(!process.waitForStarted()){
        qDebug()<<"python脚本已经启动";
        return;
    }

    timer.start();
    if(process.waitForFinished()){
        QString output = process.readAllStandardOutput();
        qDebug() << "Output:" << output;

        // 读取进程的标准错误（如果有的话）
        QString error = process.readAllStandardError();
        qDebug() << "Error:" << error;
    }else{
        qDebug() << "Process execution timed out";
    }

    int exitCode = process.exitCode();
    qDebug() << "Exit code:" << exitCode;

    time=timer.elapsed();
    qDebug() << "Elapsed time: " << time << " milliseconds";
}

void pyprocess::endProcess(void){
    if (process.state() == QProcess::Running) {
        process.terminate();
        if (!process.waitForFinished(500)) { // wait for 3 seconds
            process.kill();
            process.waitForFinished();
        }
    }
    qDebug() << "Process ended";
}

void pyprocess::setInputPath(const QString &path){
    _inputpath=path;
    qDebug()<<_inputpath;
    arguments.clear();
    arguments << _script << "--model" << _modelpath
                << "--input_path" << _inputpath << "--output_dir" << _outputpath;
}

void pyprocess::setOutputPath(const QString &path){
    _outputpath=path;
    qDebug()<<_outputpath;
    arguments.clear();
    arguments << _script << "--model" << _modelpath
                << "--input_path" << _inputpath << "--output_dir" << _outputpath;
}

void pyprocess::setScript(const QString &path){
    _script=path;
    QFileInfo scriptpath(_script);
    _modelpath=scriptpath.absolutePath()+QDir::separator()+"yolov5n_1684_f16.bmodel";
    //scriptpath.~QFileInfo();
    qDebug()<<_modelpath;

    arguments.clear();
    arguments << _script << "--model" << _modelpath
                << "--input_path" << _inputpath << "--output_dir" << _outputpath;
}
