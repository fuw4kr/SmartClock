
/**
 * @file clockcontroller.h
 * @brief Declarations for clockcontroller.
 * @details Defines types and functions related to clockcontroller.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef CLOCKCONTROLLER_H
#define CLOCKCONTROLLER_H

#include <QObject>
#include "../clock/clockmodel.h"

/**
 * @brief ClockWindow Top-level window UI class.
 * @details Provides window behavior for Clock.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ClockWindow; /**< Internal state value. */

/**
 * @brief ClockController Controller coordinating UI actions and model updates.
 * @details Provides controller behavior for Clock.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ClockController : public QObject
{
    Q_OBJECT
public:
/**
 * @brief Create ClockController instance.
 * @details Initializes instance state.
 * @param model model value.
 * @param view view value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit ClockController(ClockModel *model, ClockWindow *view, QObject *parent = nullptr);

private slots:
/**
 * @brief On add clock requested.
 * @details Performs the operation and updates state as needed.
 * @param zone Time zone identifier.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAddClockRequested(const QString &zone);
/**
 * @brief On remove clocks requested.
 * @details Performs the operation and updates state as needed.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onRemoveClocksRequested(const QList<int> &rows);
/**
 * @brief Get formatted string.
 * @details Performs the operation and updates state as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onFormatToggled(bool enabled);

private:
    ClockModel *model;
    ClockWindow *view;
};

#endif // CLOCKCONTROLLER_H
