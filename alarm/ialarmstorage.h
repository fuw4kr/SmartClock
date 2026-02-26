
/**
 * @file ialarmstorage.h
 * @brief Declarations for ialarmstorage.
 * @details Defines types and functions related to ialarmstorage.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef IALARMSTORAGE_H
#define IALARMSTORAGE_H

#include <QList>

/**
 * @brief AlarmData alarm component.
 * @details Implements alarm-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct AlarmData; /**< Alarm-related state. */

/**
 * @brief IAlarmStorage interface.
 * @details Defines the contract for AlarmStorage implementations.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class IAlarmStorage
{
public:
/**
 * @brief Destroy IAlarmStorage instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual ~IAlarmStorage() = default;
/**
 * @brief Load snapshot from storage.
 * @details Reads persisted snapshot data from storage into the output object.
 * @param out Output snapshot to populate.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool load(QList<AlarmData> &out) = 0;
/**
 * @brief Save snapshot to storage.
 * @details Writes snapshot data to persistent storage.
 * @param alarms Time delta in milliseconds.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool save(const QList<AlarmData> &alarms) = 0;
};

#endif // IALARMSTORAGE_H
