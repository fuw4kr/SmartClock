
/**
 * @file jsontimerstorage.h
 * @brief Declarations for jsontimerstorage.
 * @details Defines types and functions related to jsontimerstorage.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef JSONTIMERSTORAGE_H
#define JSONTIMERSTORAGE_H

#include "itimerstorage.h"
#include <QString>

/**
 * @brief JsonTimerStorage Storage interface or implementation for persistence.
 * @details Provides storage behavior for JsonTimer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class JsonTimerStorage : public ITimerStorage
{
public:
/**
 * @brief Create JsonTimerStorage instance.
 * @details Initializes instance state.
 * @param path Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit JsonTimerStorage(const QString &path = QString());

/**
 * @brief Load snapshot from storage.
 * @details Reads persisted snapshot data from storage into the output object.
 * @param out Output snapshot to populate.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool load(TimerSnapshot &out) override;
/**
 * @brief Save snapshot to storage.
 * @details Writes snapshot data to persistent storage.
 * @param in Input snapshot.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool save(const TimerSnapshot &in) override;

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

#endif // JSONTIMERSTORAGE_H
