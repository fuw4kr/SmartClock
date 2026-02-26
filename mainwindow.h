
/**
 * @file mainwindow.h
 * @brief Main application window declarations.
 * @details Defines types and functions related to mainwindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
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
/**
 * @brief MainWindow Top-level window UI class.
 * @details Provides window behavior for Main.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class MainWindow; /**< Internal state value. */
}

/**
 * @brief MainWindow Top-level window UI class.
 * @details Provides window behavior for Main.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class MainWindow : public FramelessWindow
{
    Q_OBJECT

public:
/**
 * @brief Create MainWindow instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit MainWindow(QWidget *parent = nullptr);
/**
 * @brief Destroy MainWindow instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~MainWindow();

/**
 * @brief Get running timers.
 * @details Returns the current value derived from internal state.
 * @return Integer value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    int getRunningTimers() const;
/**
 * @brief Get finished timers.
 * @details Returns the current value derived from internal state.
 * @return Integer value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    int getFinishedTimers() const;
/**
 * @brief Get next alarm time.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getNextAlarmTime() const;
/**
 * @brief Get stopwatch lap time.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getStopwatchLapTime() const;
/**
 * @brief Get stopwatch total time.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getStopwatchTotalTime() const;

protected:
/**
 * @brief Close event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayMenu = nullptr;
/**
 * @brief Set up tray icon.
 * @details Updates internal state and emits signals as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setupTrayIcon();

    ClockWindow *clockWindow;
    AlarmWindow *alarmWindow;
    StopwatchWindow *stopwatchWindow;
    TimerWindow *timerWindow;

/**
 * @brief Set up title bar.
 * @details Updates internal state and emits signals as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setupTitleBar();
/**
 * @brief Update maximize icon.
 * @details Performs the operation and updates state as needed.
 * @param maxed maxed value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateMaximizeIcon(bool maxed);

/**
 * @brief Update theme icon.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateThemeIcon();

private slots:
/**
 * @brief On tray open timer.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onTrayOpenTimer();
/**
 * @brief On tray open stopwatch.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onTrayOpenStopwatch();
/**
 * @brief On tray open alarm.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onTrayOpenAlarm();
/**
 * @brief On tray open clock.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onTrayOpenClock();
/**
 * @brief On tray exit.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onTrayExit();

/**
 * @brief On btn theme clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void on_btnTheme_clicked();
};

#endif // MAINWINDOW_H
