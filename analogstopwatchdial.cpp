#include "analogstopwatchdial.h"
#include <QPainter>
#include <QtMath>

AnalogStopwatchDial::AnalogStopwatchDial(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(250, 250);
}

void AnalogStopwatchDial::setElapsed(const QTime &time)
{
    elapsed = time;
    update();
}

void AnalogStopwatchDial::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    p.translate(width() / 2, height() / 2);
    p.scale(side / 200.0, side / 200.0);

    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);
    p.drawEllipse(-100, -100, 200, 200);

    p.setPen(QPen(Qt::black, 2));
    for (int i = 0; i < 60; ++i) {
        if (i % 5 == 0)
            p.drawLine(0, -90, 0, -100);
        else
            p.drawLine(0, -95, 0, -100);
        p.rotate(6.0);
    }

    p.save();
    p.setPen(QPen(Qt::gray, 1));
    p.drawEllipse(-35, -35, 70, 70);

    for (int i = 0; i < 30; ++i) {
        if (i % 5 == 0)
            p.drawLine(0, -30, 0, -35);
        else
            p.drawLine(0, -33, 0, -35);
        p.rotate(12.0); // 360/30
    }
    p.restore();

    p.save();
    p.setPen(QPen(Qt::black, 2));
    double minuteAngle = (elapsed.minute() % 30) * 12.0 + (elapsed.second() / 60.0) * 12.0;
    p.rotate(minuteAngle);
    p.drawLine(0, 0, 0, -25);
    p.restore();

    p.save();
    p.setPen(QPen(QColorConstants::Svg::orange, 2));
    double secAngle = (elapsed.second() + elapsed.msec() / 1000.0) * 6.0;
    p.rotate(secAngle);
    p.drawLine(0, 10, 0, -85);
    p.restore();

    p.save();
    p.setPen(QPen(Qt::cyan, 1));
    double msAngle = (elapsed.msec() / 1000.0) * 360.0;
    p.rotate(msAngle);
    p.drawLine(0, 0, 0, -60);
    p.restore();

    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    p.drawEllipse(-3, -3, 6, 6);

    p.setPen(Qt::black);
    p.setFont(QFont("Arial", 10, QFont::Bold));
    p.drawText(-40, 40, 80, 20, Qt::AlignCenter, elapsed.toString("mm:ss.zzz").left(8));
}
