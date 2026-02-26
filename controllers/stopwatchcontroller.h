
/**
 * @file stopwatchcontroller.h
 * @brief Declarations for stopwatchcontroller.
 * @details Defines types and functions related to stopwatchcontroller.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef STOPWATCHCONTROLLER_H
#define STOPWATCHCONTROLLER_H

#include <QObject>
#include "../stopwatch/stopwatchmodel.h"

/**
 * @brief StopwatchWindow Top-level window UI class.
 * @details Provides window behavior for Stopwatch.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class StopwatchWindow; /**< Internal state value. */

/**
 * @brief StopwatchController Controller coordinating UI actions and model updates.
 * @details Provides controller behavior for Stopwatch.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class StopwatchController : public QObject
{
    Q_OBJECT
public:
/**
 * @brief Create StopwatchController instance.
 * @details Initializes instance state.
 * @param model model value.
 * @param view view value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit StopwatchController(StopwatchModel *model, StopwatchWindow *view, QObject *parent = nullptr);

private slots:
/**
 * @brief On start stop requested.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onStartStopRequested();
/**
 * @brief On lap requested.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onLapRequested();

private:
    StopwatchModel *model;
    StopwatchWindow *view;
};

#endif // STOPWATCHCONTROLLER_H
