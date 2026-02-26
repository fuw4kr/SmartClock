
/**
 * @file istopwatchstorage.h
 * @brief Declarations for istopwatchstorage.
 * @details Defines types and functions related to istopwatchstorage.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ISTOPWATCHSTORAGE_H
#define ISTOPWATCHSTORAGE_H

#include <QList>

/**
 * @brief StopwatchSnapshot snapshot.
 * @details Captures stopwatch state for persistence or restore.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct StopwatchSnapshot {
    int elapsedMs = 0;          /**< Total elapsed time at capture (milliseconds). */
    bool running = false;       /**< True if the stopwatch was running when captured. */
    QList<int> lapDurations;    /**< Ordered list of lap segment durations (milliseconds). */
};

/**
 * @brief IStopwatchStorage interface.
 * @details Defines the contract for StopwatchStorage implementations.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class IStopwatchStorage
{
public:
/**
 * @brief Destroy IStopwatchStorage instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual ~IStopwatchStorage() = default;
/**
 * @brief Load snapshot from storage.
 * @details Reads persisted snapshot data from storage into the output object.
 * @param out Output snapshot to populate.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool load(StopwatchSnapshot &out) = 0;
/**
 * @brief Save snapshot to storage.
 * @details Writes snapshot data to persistent storage.
 * @param in Input snapshot.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual bool save(const StopwatchSnapshot &in) = 0;
};

#endif // ISTOPWATCHSTORAGE_H
