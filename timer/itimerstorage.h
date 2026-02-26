
/**
 * @file itimerstorage.h
 * @brief Declarations for itimerstorage.
 * @details Defines types and functions related to itimerstorage.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ITIMERSTORAGE_H
#define ITIMERSTORAGE_H

#include <QList>
#include <QMap>

/**
 * @brief TimerData timer component.
 * @details Implements timer-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct TimerData; /**< Timer-related state. */

/**
 * @brief TimerSnapshot snapshot.
 * @details Captures timer state for persistence or restore.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct TimerSnapshot {
    QList<TimerData> timers; /**< Timer-related state. */
    QMap<QString, QString> recommendations;
    QList<TimerData> deletedTimers; /**< Timer-related state. */
};

/**
 * @brief ITimerStorage interface.
 * @details Defines the contract for TimerStorage implementations.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ITimerStorage
{
public:
/**
 * @brief Destroy ITimerStorage instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual ~ITimerStorage() = default;
/**
 * @brief Load snapshot from storage.
 * @details Reads persisted snapshot data from storage into the output object.
 * @param out Output snapshot to populate.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool load(TimerSnapshot &out) = 0;
/**
 * @brief Save snapshot to storage.
 * @details Writes snapshot data to persistent storage.
 * @param in Input snapshot.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool save(const TimerSnapshot &in) = 0;
};

#endif // ITIMERSTORAGE_H
