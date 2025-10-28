#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "timerwindow.h"
#include "clockwindow.h"
#include "alarmwindow.h"
#include "stopwatchwindow.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    ClockWindow *clockWindow = nullptr;
    AlarmWindow *alarmWindow = nullptr;
    StopwatchWindow *stopwatchWindow = nullptr;
    TimerWindow *timerWindow = nullptr;
};

#endif // MAINWINDOW_H

