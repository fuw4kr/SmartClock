/**
 * @file analogstopwatchdial.cpp
 * @brief Definitions for analogstopwatchdial.
 * @details Implements logic declared in the corresponding header for analogstopwatchdial.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "analogstopwatchdial.h"
#include "../thememanager.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>

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
void AnalogStopwatchDial::show() {
    if (qEnvironmentVariableIsSet("TEST_MODE")) {
        qDebug() << "[TEST_MODE] Suppressed Settings dialog";
        return;
    }
    QWidget::show();
}

void AnalogStopwatchDial::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    p.translate(width() / 2, height() / 2);
    p.scale(side / 200.0, side / 200.0);

    Theme theme = ThemeManager::instance().currentTheme();
    bool isDark = (theme == Theme::Dark);

    QColor bg, dialBg, text, accent, milli, border;

    if (isDark) {
        bg      = QColor("#0E273C");
        dialBg  = QColor("#090C08");
        text    = QColor("#C9D1D9");
        accent  = QColor("#99AA38");
        milli   = QColor("#D52941");
        border  = QColor("#99AA38");
    } else {
        bg      = QColor("#FFF8E8");
        dialBg  = QColor("#FFF8E8");
        text    = QColor("#41521F");
        accent  = QColor("#D52941");
        milli   = QColor("#990D35");
        border  = QColor("#FCD581");
    }

    p.setBrush(dialBg);
    p.setPen(QPen(border, 3));
    p.drawEllipse(-100, -100, 200, 200);

    p.setPen(QPen(text, 2));
    for (int i = 0; i < 60; ++i) {
        if (i % 5 == 0)
            p.drawLine(0, -90, 0, -100);
        else
            p.drawLine(0, -95, 0, -100);
        p.rotate(6.0);
    }

    p.save();
    p.setPen(QPen(border.lighter(120), 1.5));
    p.drawEllipse(-35, -35, 70, 70);
    p.restore();

    p.save();
    p.setPen(QPen(text, 2, Qt::SolidLine, Qt::RoundCap));
    double minuteAngle = (elapsed.minute() % 30) * 12.0 + (elapsed.second() / 60.0) * 12.0;
    p.rotate(minuteAngle);
    p.drawLine(0, 0, 0, -25);
    p.restore();

    p.save();
    p.setPen(QPen(accent, 2.4, Qt::SolidLine, Qt::RoundCap));
    double secAngle = (elapsed.second() + elapsed.msec() / 1000.0) * 6.0;
    p.rotate(secAngle);
    p.drawLine(0, 10, 0, -85);
    p.restore();

    p.save();
    p.setPen(QPen(milli, 1.2, Qt::SolidLine, Qt::RoundCap));
    double msAngle = (elapsed.msec() / 1000.0) * 360.0;
    p.rotate(msAngle);
    p.drawLine(0, 0, 0, -60);
    p.restore();

    p.setBrush(accent);
    p.setPen(Qt::NoPen);
    p.drawEllipse(-4, -4, 8, 8);

    p.setPen(text);
    p.setFont(QFont("Poppins", 10, QFont::Bold));
    p.drawText(-40, 40, 80, 20, Qt::AlignCenter, elapsed.toString("mm:ss.zzz").left(8));
}
