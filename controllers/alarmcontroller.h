
/**
 * @file alarmcontroller.h
 * @brief Declarations for alarmcontroller.
 * @details Defines types and functions related to alarmcontroller.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ALARMCONTROLLER_H
#define ALARMCONTROLLER_H

#include <QObject>
#include <QList>
#include "../alarm/alarmmanager.h"

/**
 * @brief AlarmWindow Top-level window UI class.
 * @details Provides window behavior for Alarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmWindow; /**< Alarm-related state. */

/**
 * @brief AlarmController Controller coordinating UI actions and model updates.
 * @details Provides controller behavior for Alarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmController : public QObject
{
    Q_OBJECT
public:
/**
 * @brief Create AlarmController instance.
 * @details Initializes instance state.
 * @param model model value.
 * @param view view value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit AlarmController(AlarmManager *model, AlarmWindow *view, QObject *parent = nullptr);

/**
 * @brief Next alarm string.
 * @details Performs the operation and updates state as needed.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString nextAlarmString() const;

private slots:
/**
 * @brief On add alarm requested.
 * @details Performs the operation and updates state as needed.
 * @param data data value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAddAlarmRequested(const AlarmData &data);
/**
 * @brief On remove alarms requested.
 * @details Performs the operation and updates state as needed.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onRemoveAlarmsRequested(const QList<int> &rows);
/**
 * @brief On alarm toggled.
 * @details Performs the operation and updates state as needed.
 * @param index Zero-based index.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAlarmToggled(int index, bool enabled);
/**
 * @brief On snooze requested.
 * @details Performs the operation and updates state as needed.
 * @param alarm alarm value.
 * @param minutes minutes value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onSnoozeRequested(const AlarmData &alarm, int minutes);
/**
 * @brief On model updated.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onModelUpdated();

private:
    AlarmManager *model;
    AlarmWindow *view;
};

#endif // ALARMCONTROLLER_H
