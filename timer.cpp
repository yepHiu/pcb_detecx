#include "timer.h"

/**
 * @brief timer::timer 构造函数
 */
timer::timer(pyprocess *proc,QObject *parent):QObject(parent){
    qtimer =new QTimer();
    qproc=proc;

    QObject::connect(qtimer,&QTimer::timeout,this,&timer::onTimeoutHandle);
}

/**
 * @brief timer::~timer 析构函数不用管
 */
timer::~timer(){
    qtimer->stop();
    delete qtimer;
}

void timer::start(qint32 mils){
    qDebug()<<"倒计时启动";
    qtimer->start(mils);
}

void timer::onTimeoutHandle(){
    qproc->startProcess();// 倒计时出发后的任务函数
    qtimer->stop();
    qDebug()<<"倒计时关闭";
}
