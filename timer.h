#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QDebug>
#include <QObject>

#include <pyprocess.h>

class timer:public QObject
{
    Q_OBJECT

public:
    explicit timer(pyprocess *proc,QObject *parent = nullptr);
    ~timer();

    void start(qint32 mils);

private:
    void onTimeoutHandle();

private:
    QTimer *qtimer;

    pyprocess *qproc;

};

#endif // TIMER_H
