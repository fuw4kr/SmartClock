
/**
 * @file timercontroller.h
 * @brief Declarations for timercontroller.
 * @details Defines types and functions related to timercontroller.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef TIMERCONTROLLER_H
#define TIMERCONTROLLER_H

#include <QObject>
#include <QList>
#include "../timer/timermanager.h"

/**
 * @brief TimerWindow Top-level window UI class.
 * @details Provides window behavior for Timer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class TimerWindow; /**< Timer-related state. */

/**
 * @brief TimerController Controller coordinating UI actions and model updates.
 * @details Provides controller behavior for Timer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class TimerController : public QObject
{
    Q_OBJECT
public:
/**
 * @brief Create TimerController instance.
 * @details Initializes instance state.
 * @param model model value.
 * @param view view value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit TimerController(TimerManager *model, TimerWindow *view, QObject *parent = nullptr);

private slots:
/**
 * @brief On add timer requested.
 * @details Performs the operation and updates state as needed.
 * @param name Name string.
 * @param seconds seconds value.
 * @param type type value.
 * @param group group value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAddTimerRequested(const QString &name, int seconds, const QString &type, const QString &group);
/**
 * @brief On edit timer requested.
 * @details Performs the operation and updates state as needed.
 * @param index Zero-based index.
 * @param name Name string.
 * @param seconds seconds value.
 * @param type type value.
 * @param group group value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onEditTimerRequested(int index, const QString &name, int seconds, const QString &type, const QString &group);
/**
 * @brief On delete timers requested.
 * @details Performs the operation and updates state as needed.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onDeleteTimersRequested(const QList<int> &rows);
/**
 * @brief On start pause requested.
 * @details Performs the operation and updates state as needed.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onStartPauseRequested(const QList<int> &rows);
/**
 * @brief On save requested.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onSaveRequested();

private:
    TimerManager *model;
    TimerWindow *view;
};

#endif // TIMERCONTROLLER_H
