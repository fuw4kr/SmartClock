
/**
 * @file alarmwindow.h
 * @brief Declarations for alarmwindow.
 * @details Defines types and functions related to alarmwindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ALARMWINDOW_H
#define ALARMWINDOW_H

#include <QDialog>
#include "alarmmanager.h"
#include "../controllers/alarmcontroller.h"

namespace Ui {
/**
 * @brief AlarmWindow Top-level window UI class.
 * @details Provides window behavior for Alarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmWindow; /**< Alarm-related state. */
}

/**
 * @brief AlarmWindow Top-level window UI class.
 * @details Provides window behavior for Alarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmWindow : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create AlarmWindow instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit AlarmWindow(QWidget *parent = nullptr);
/**
 * @brief Destroy AlarmWindow instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~AlarmWindow();
/**
 * @brief Get next alarm string.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getNextAlarmString() const;

signals:
/**
 * @brief Add alarm requested.
 * @details Modifies the associated collection and notifies listeners.
 * @param data data value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addAlarmRequested(const AlarmData &data);
/**
 * @brief Remove alarms requested.
 * @details Modifies the associated collection and notifies listeners.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void removeAlarmsRequested(const QList<int> &rows);
/**
 * @brief Alarm toggled.
 * @details Performs the operation and updates state as needed.
 * @param index Zero-based index.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void alarmToggled(int index, bool enabled);
/**
 * @brief Snooze requested.
 * @details Performs the operation and updates state as needed.
 * @param alarm alarm value.
 * @param minutes minutes value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void snoozeRequested(const AlarmData &alarm, int minutes);

public slots:
/**
 * @brief Set alarms.
 * @details Updates internal state and emits signals as needed.
 * @param alarms Time delta in milliseconds.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setAlarms(const QList<AlarmData> &alarms);
/**
 * @brief Show alarm triggered.
 * @details Performs the operation and updates state as needed.
 * @param alarm alarm value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void showAlarmTriggered(const AlarmData &alarm);

private slots:
/**
 * @brief On add alarm.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAddAlarm();
/**
 * @brief On remove alarm.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onRemoveAlarm();

private:
/**
 * @brief Selected alarm rows.
 * @details Performs the operation and updates state as needed.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QList<int> selectedAlarmRows() const;
/**
 * @brief Show no selection warning.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void showNoSelectionWarning();
/**
 * @brief Confirm delete.
 * @details Performs the operation and updates state as needed.
 * @param count Number of items.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool confirmDelete(int count);
/**
 * @brief Show delete info.
 * @details Performs the operation and updates state as needed.
 * @param count Number of items.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void showDeleteInfo(int count);

    Ui::AlarmWindow *ui;
    AlarmManager *manager;
    AlarmController *controller;
};

#endif // ALARMWINDOW_H
