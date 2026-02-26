
/**
 * @file stopwatchmodel.h
 * @brief Declarations for stopwatchmodel.
 * @details Defines types and functions related to stopwatchmodel.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef STOPWATCHMODEL_H
#define STOPWATCHMODEL_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QTime>
#include <memory>
#include "istopwatchstorage.h"

/**
 * @brief StopwatchModel Data model holding state and exposing operations for the UI.
 * @details Provides model behavior for Stopwatch.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class StopwatchModel : public QObject
{
    Q_OBJECT
public:
/**
 * @brief Create StopwatchModel instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @param storage Storage backend instance.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit StopwatchModel(QObject *parent = nullptr, std::unique_ptr<IStopwatchStorage> storage = {});

/**
 * @brief Check whether running.
 * @details Returns the current value derived from internal state.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool isRunning() const;
/**
 * @brief Get elapsed time in milliseconds.
 * @details Performs the operation and updates state as needed.
 * @return Elapsed time in milliseconds.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    int elapsedMs() const;
/**
 * @brief Get elapsed time as QTime.
 * @details Performs the operation and updates state as needed.
 * @return Elapsed time as QTime.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QTime elapsedTime() const;
/**
 * @brief Get formatted elapsed time string.
 * @details Performs the operation and updates state as needed.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString formattedElapsed() const;
/**
 * @brief Get lap segment durations.
 * @details Performs the operation and updates state as needed.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    const QList<int>& lapDurations() const;
/**
 * @brief Get formatted lap strings.
 * @details Performs the operation and updates state as needed.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QStringList lapTexts() const;

/**
 * @brief Start operation.
 * @details Transitions the state machine and notifies listeners.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void start();
/**
 * @brief Stop operation.
 * @details Transitions the state machine and notifies listeners.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void stop();
/**
 * @brief Advance operation.
 * @details Advances time-based state by the provided delta.
 * @param ms Time delta in milliseconds.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void tick(int ms);
/**
 * @brief Reset operation.
 * @details Transitions the state machine and notifies listeners.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void reset();
/**
 * @brief Add lap.
 * @details Modifies the associated collection and notifies listeners.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addLap();

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
    void setStorage(std::unique_ptr<IStopwatchStorage> storage);

signals:
/**
 * @brief Emitted when state changes.
 * @details Signal emitted when the associated state changes.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void stateChanged();
/**
 * @brief Emitted when laps change.
 * @details Signal emitted when the associated state changes.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void lapsChanged();

private:
    int elapsed = 0;                                /**< Total elapsed time (milliseconds). */
    bool running = false;                           /**< True while stopwatch is running. */
    QList<int> laps;                                /**< Lap segment durations (milliseconds). */
    std::unique_ptr<IStopwatchStorage> storage;     /**< Owned storage backend. */
};

#endif // STOPWATCHMODEL_H
