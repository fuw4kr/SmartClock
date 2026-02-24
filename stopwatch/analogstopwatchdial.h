#ifndef ANALOGSTOPWATCHDIAL_H
#define ANALOGSTOPWATCHDIAL_H

#include <QWidget>
#include <QTime>

class AnalogStopwatchDial : public QWidget
{
    Q_OBJECT

public:
    explicit AnalogStopwatchDial(QWidget *parent = nullptr);
    void setElapsed(const QTime &time);
    void show();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QTime elapsed;
};

#endif // ANALOGSTOPWATCHDIAL_H
