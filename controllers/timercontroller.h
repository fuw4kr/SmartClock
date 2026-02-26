#ifndef TIMERCONTROLLER_H
#define TIMERCONTROLLER_H

#include <QObject>
#include <QList>
#include "../timer/timermanager.h"

class TimerWindow;

class TimerController : public QObject
{
    Q_OBJECT
public:
    explicit TimerController(TimerManager *model, TimerWindow *view, QObject *parent = nullptr);

private slots:
    void onAddTimerRequested(const QString &name, int seconds, const QString &type, const QString &group);
    void onEditTimerRequested(int index, const QString &name, int seconds, const QString &type, const QString &group);
    void onDeleteTimersRequested(const QList<int> &rows);
    void onStartPauseRequested(const QList<int> &rows);
    void onSaveRequested();

private:
    TimerManager *model;
    TimerWindow *view;
};

#endif // TIMERCONTROLLER_H
