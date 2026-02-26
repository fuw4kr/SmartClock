
/**
 * @file alarmmanager.h
 * @brief Declarations for alarmmanager.
 * @details Defines types and functions related to alarmmanager.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QList>
#include <memory>
#include "ialarmstorage.h"
#include "alarmrepeatmode.h"

/**
 * @brief AlarmData alarm component.
 * @details Implements alarm-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct AlarmData {
    QString name; /**< Internal state value. */
    QTime time; /**< Internal state value. */
    RepeatMode repeatMode = RepeatMode::Never; /**< Internal state value. */
    QStringList days; /**< Internal state value. */
    QString soundPath; /**< Filesystem path. */
    bool snooze; /**< Internal state value. */
    bool enabled; /**< Current state flag. */
    QDateTime nextTrigger; /**< Internal state value. */
};

/**
 * @brief AlarmManager Manager that owns and coordinates related objects.
 * @details Provides manager behavior for Alarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmManager : public QObject
{
    Q_OBJECT

public:
/**
 * @brief Create AlarmManager instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @param storage Storage backend instance.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit AlarmManager(QObject *parent = nullptr, std::unique_ptr<IAlarmStorage> storage = {});
/**
 * @brief Add alarm.
 * @details Modifies the associated collection and notifies listeners.
 * @param data data value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addAlarm(const AlarmData &data);
/**
 * @brief Remove alarm.
 * @details Modifies the associated collection and notifies listeners.
 * @param index Zero-based index.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void removeAlarm(int index);
/**
 * @brief Toggle alarm.
 * @details Performs the operation and updates state as needed.
 * @param index Zero-based index.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void toggleAlarm(int index);
/**
 * @brief Get alarms.
 * @details Returns the current value derived from internal state.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QList<AlarmData> getAlarms() const;
/**
 * @brief Save to file.
 * @details Writes current state to persistent storage.
 * @param path Filesystem path.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void saveToFile(const QString &path);
/**
 * @brief Load from file.
 * @details Reads persisted state into the object.
 * @param path Filesystem path.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void loadFromFile(const QString &path);
/**
 * @brief Save operation.
 * @details Writes current state to persistent storage.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool save();
/**
 * @brief Load operation.
 * @details Reads persisted state into the object.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool load();
/**
 * @brief Set storage.
 * @details Updates internal state and emits signals as needed.
 * @param storage Storage backend instance.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setStorage(std::unique_ptr<IAlarmStorage> storage);
/**
 * @brief Snooze alarm.
 * @details Performs the operation and updates state as needed.
 * @param alarm alarm value.
 * @param minutes minutes value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void snoozeAlarm(const AlarmData &alarm, int minutes);

signals:
/**
 * @brief Alarms updated.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void alarmsUpdated();
/**
 * @brief Alarm triggered.
 * @details Performs the operation and updates state as needed.
 * @param alarm alarm value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void alarmTriggered(const AlarmData &alarm);

private slots:
/**
 * @brief Check alarms.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void checkAlarms();

private:
/**
 * @brief Compute initial trigger.
 * @details Performs the operation and updates state as needed.
 * @param t t value.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static QDateTime computeInitialTrigger(const QTime &t);
/**
 * @brief Compute next trigger.
 * @details Performs the operation and updates state as needed.
 * @param a a value.
 * @param after after value.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static QDateTime computeNextTrigger(const AlarmData &a, const QDateTime &after);
/**
 * @brief Compute weekly trigger.
 * @details Performs the operation and updates state as needed.
 * @param a a value.
 * @param startDate startDate value.
 * @param t t value.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static QDateTime computeWeeklyTrigger(const AlarmData &a, const QDate &startDate, const QTime &t);
/**
 * @brief Check whether one time.
 * @details Returns the current value derived from internal state.
 * @param a a value.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static bool isOneTime(const AlarmData &a);
/**
 * @brief Check whether due.
 * @details Returns the current value derived from internal state.
 * @param a a value.
 * @param now now value.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static bool isDue(const AlarmData &a, const QDateTime &now);
/**
 * @brief Ensure next trigger.
 * @details Performs the operation and updates state as needed.
 * @param a a value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static void ensureNextTrigger(AlarmData &a);
/**
 * @brief Handle triggered alarm.
 * @details Performs the operation and updates state as needed.
 * @param a a value.
 * @param now now value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void handleTriggeredAlarm(AlarmData &a, const QDateTime &now);
/**
 * @brief Find alarm index.
 * @details Performs the operation and updates state as needed.
 * @param alarm alarm value.
 * @return Integer value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    int findAlarmIndex(const AlarmData &alarm) const;

    QList<AlarmData> alarms; /**< Elapsed time in milliseconds. */
    QTimer checkTimer; /**< Timer-related state. */
    std::unique_ptr<IAlarmStorage> storage; /**< Owned storage backend. */
};

#endif // ALARMMANAGER_H
