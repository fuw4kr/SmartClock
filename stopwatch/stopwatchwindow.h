#ifndef STOPWATCHWINDOW_H
#define STOPWATCHWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QStackedWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "analogstopwatchdial.h"
#include "stopwatchmodel.h"
#include "../controllers/stopwatchcontroller.h"

namespace Ui {
class StopwatchWindow;
}

class StopwatchWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StopwatchWindow(QWidget *parent = nullptr);
    ~StopwatchWindow();

    void updateDisplay();
    QString getCurrentLapTimeString() const;
    QString getTotalTimeString() const;

signals:
    void startStopRequested();
    void lapRequested();

public slots:
    void syncFromModel();

private slots:
    void onStartStopClicked();
    void onLapClicked();

private:
    Ui::StopwatchWindow *ui;
    QTimer *timer;
    StopwatchModel *model;
    StopwatchController *controller;

    AnalogStopwatchDial *analogDial;
    bool analogMode = false;

    void updateLapColors();
    QStackedWidget *stackedView;

    friend class StopwatchWindowTest_LapAndResetWork_Test;
};

#endif // STOPWATCHWINDOW_H
