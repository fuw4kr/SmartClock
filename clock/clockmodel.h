
/**
 * @file clockmodel.h
 * @brief Declarations for clockmodel.
 * @details Defines types and functions related to clockmodel.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef CLOCKMODEL_H
#define CLOCKMODEL_H

#include <QObject>
#include <QList>
#include <memory>
#include "iclockstorage.h"

/**
 * @brief ClockInfo clock component.
 * @details Implements clock-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct ClockInfo {
    QString zone; /**< Internal state value. */
};

/**
 * @brief IClockStorage interface.
 * @details Defines the contract for ClockStorage implementations.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class IClockStorage; /**< Owned storage backend. */

/**
 * @brief ClockModel Data model holding state and exposing operations for the UI.
 * @details Provides model behavior for Clock.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ClockModel : public QObject
{
    Q_OBJECT
public:
/**
 * @brief Create ClockModel instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @param storage Storage backend instance.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit ClockModel(QObject *parent = nullptr, std::unique_ptr<IClockStorage> storage = {});

/**
 * @brief Clocks.
 * @details Performs the operation and updates state as needed.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    const QList<ClockInfo>& clocks() const;
/**
 * @brief Get formatted string.
 * @details Performs the operation and updates state as needed.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool format12h() const;
/**
 * @brief Get formatted string.
 * @details Updates internal state and emits signals as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setFormat12h(bool enabled);

/**
 * @brief Add clock.
 * @details Modifies the associated collection and notifies listeners.
 * @param zone Time zone identifier.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addClock(const QString &zone);
/**
 * @brief Remove clock.
 * @details Modifies the associated collection and notifies listeners.
 * @param index Zero-based index.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void removeClock(int index);

/**
 * @brief Time text for.
 * @details Performs the operation and updates state as needed.
 * @param ci ci value.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString timeTextFor(const ClockInfo &ci) const;

/**
 * @brief Load operation.
 * @details Reads persisted state into the object.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool load();
/**
 * @brief Save operation.
 * @details Writes current state to persistent storage.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool save() const;
/**
 * @brief Set storage.
 * @details Updates internal state and emits signals as needed.
 * @param storage Storage backend instance.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setStorage(std::unique_ptr<IClockStorage> storage);

signals:
/**
 * @brief Emitted when clocks change.
 * @details Signal emitted when the associated state changes.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void clocksChanged();
/**
 * @brief Emitted when format changes.
 * @details Signal emitted when the associated state changes.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void formatChanged(bool enabled);

private:
    QList<ClockInfo> clocksList; /**< Internal state value. */
    bool format12 = false; /**< Internal state value. */
    std::unique_ptr<IClockStorage> storage; /**< Owned storage backend. */
};

#endif // CLOCKMODEL_H
