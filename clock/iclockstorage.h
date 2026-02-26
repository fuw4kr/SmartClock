
/**
 * @file iclockstorage.h
 * @brief Declarations for iclockstorage.
 * @details Defines types and functions related to iclockstorage.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ICLOCKSTORAGE_H
#define ICLOCKSTORAGE_H

#include <QList>

/**
 * @brief ClockInfo clock component.
 * @details Implements clock-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct ClockInfo; /**< Internal state value. */

/**
 * @brief ClockSnapshot snapshot.
 * @details Captures clock state for persistence or restore.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct ClockSnapshot {
    QList<ClockInfo> clocks; /**< Internal state value. */
    bool format12h = false; /**< Internal state value. */
};

/**
 * @brief IClockStorage interface.
 * @details Defines the contract for ClockStorage implementations.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class IClockStorage
{
public:
/**
 * @brief Destroy IClockStorage instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual ~IClockStorage() = default;
/**
 * @brief Load snapshot from storage.
 * @details Reads persisted snapshot data from storage into the output object.
 * @param out Output snapshot to populate.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool load(ClockSnapshot &out) = 0;
/**
 * @brief Save snapshot to storage.
 * @details Writes snapshot data to persistent storage.
 * @param in Input snapshot.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool save(const ClockSnapshot &in) = 0;
};

#endif // ICLOCKSTORAGE_H
