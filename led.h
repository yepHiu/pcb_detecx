#ifndef LED_H
#define LED_H

#include <QApplication>
#include <QLabel>

class led
{
public:
    led();
};

void setLED(QLabel *label,qint32 sta,qint32 size);

#endif // LED_H
