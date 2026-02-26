
/**
 * @file stopwatchwindow.h
 * @brief Declarations for stopwatchwindow.
 * @details Defines types and functions related to stopwatchwindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
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
/**
 * @brief StopwatchWindow Top-level window UI class.
 * @details Provides window behavior for Stopwatch.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class StopwatchWindow; /**< Internal state value. */
}

/**
 * @brief StopwatchWindow Top-level window UI class.
 * @details Provides window behavior for Stopwatch.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class StopwatchWindow : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create StopwatchWindow instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit StopwatchWindow(QWidget *parent = nullptr);
/**
 * @brief Destroy StopwatchWindow instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~StopwatchWindow();

/**
 * @brief Update display.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateDisplay();
/**
 * @brief Get current lap time string.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getCurrentLapTimeString() const;
/**
 * @brief Get total time string.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getTotalTimeString() const;

signals:
/**
 * @brief Start stop requested.
 * @details Transitions the state machine and notifies listeners.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void startStopRequested();
/**
 * @brief Lap requested.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void lapRequested();

public slots:
/**
 * @brief Sync from model.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void syncFromModel();

private slots:
/**
 * @brief On start stop clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onStartStopClicked();
/**
 * @brief On lap clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onLapClicked();

private:
    Ui::StopwatchWindow *ui;
    QTimer *timer;
    StopwatchModel *model;
    StopwatchController *controller;

    AnalogStopwatchDial *analogDial;
    bool analogMode = false; /**< Internal state value. */

/**
 * @brief Update lap colors.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateLapColors();
    QStackedWidget *stackedView;

    friend class StopwatchWindowTest_LapAndResetWork_Test;
};

#endif // STOPWATCHWINDOW_H
