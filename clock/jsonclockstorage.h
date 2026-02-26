
/**
 * @file jsonclockstorage.h
 * @brief Declarations for jsonclockstorage.
 * @details Defines types and functions related to jsonclockstorage.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef JSONCLOCKSTORAGE_H
#define JSONCLOCKSTORAGE_H

#include "iclockstorage.h"
#include <QString>

/**
 * @brief JsonClockStorage Storage interface or implementation for persistence.
 * @details Provides storage behavior for JsonClock.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class JsonClockStorage : public IClockStorage
{
public:
/**
 * @brief Create JsonClockStorage instance.
 * @details Initializes instance state.
 * @param path Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit JsonClockStorage(const QString &path = QString());

/**
 * @brief Load snapshot from storage.
 * @details Reads persisted snapshot data from storage into the output object.
 * @param out Output snapshot to populate.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool load(ClockSnapshot &out) override;
/**
 * @brief Save snapshot to storage.
 * @details Writes snapshot data to persistent storage.
 * @param in Input snapshot.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool save(const ClockSnapshot &in) override;

private:
/**
 * @brief Resolve path.
 * @details Performs the operation and updates state as needed.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString resolvePath() const;

    QString path; /**< Filesystem path. */
};

#endif // JSONCLOCKSTORAGE_H
