#ifndef STOPWATCHCONTROLLER_H
#define STOPWATCHCONTROLLER_H

#include <QObject>
#include "../stopwatch/stopwatchmodel.h"

class StopwatchWindow;

class StopwatchController : public QObject
{
    Q_OBJECT
public:
    explicit StopwatchController(StopwatchModel *model, StopwatchWindow *view, QObject *parent = nullptr);

private slots:
    void onStartStopRequested();
    void onLapRequested();

private:
    StopwatchModel *model;
    StopwatchWindow *view;
};

#endif // STOPWATCHCONTROLLER_H
