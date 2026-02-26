
/**
 * @file jsonalarmstorage.h
 * @brief Declarations for jsonalarmstorage.
 * @details Defines types and functions related to jsonalarmstorage.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef JSONALARMSTORAGE_H
#define JSONALARMSTORAGE_H

#include "ialarmstorage.h"
#include <QString>

/**
 * @brief JsonAlarmStorage Storage interface or implementation for persistence.
 * @details Provides storage behavior for JsonAlarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class JsonAlarmStorage : public IAlarmStorage
{
public:
/**
 * @brief Create JsonAlarmStorage instance.
 * @details Initializes instance state.
 * @param path Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit JsonAlarmStorage(const QString &path = QString());

/**
 * @brief Load snapshot from storage.
 * @details Reads persisted snapshot data from storage into the output object.
 * @param out Output snapshot to populate.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool load(QList<AlarmData> &out) override;
/**
 * @brief Save snapshot to storage.
 * @details Writes snapshot data to persistent storage.
 * @param alarms Time delta in milliseconds.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool save(const QList<AlarmData> &alarms) override;

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

#endif // JSONALARMSTORAGE_H
