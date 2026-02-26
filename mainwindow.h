#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "timer/timerwindow.h"
#include "clock/clockwindow.h"
#include "alarm/alarmwindow.h"
#include "stopwatch/stopwatchwindow.h"
#include "windowedit/framelesswindow.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public FramelessWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int getRunningTimers() const;
    int getFinishedTimers() const;
    QString getNextAlarmTime() const;
    QString getStopwatchLapTime() const;
    QString getStopwatchTotalTime() const;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayMenu = nullptr;
    void setupTrayIcon();

    ClockWindow *clockWindow;
    AlarmWindow *alarmWindow;
    StopwatchWindow *stopwatchWindow;
    TimerWindow *timerWindow;

    void setupTitleBar();
    void updateMaximizeIcon(bool maxed);

    void updateThemeIcon();

private slots:
    void onTrayOpenTimer();
    void onTrayOpenStopwatch();
    void onTrayOpenAlarm();
    void onTrayOpenClock();
    void onTrayExit();

    void on_btnTheme_clicked();
};

#endif // MAINWINDOW_H
