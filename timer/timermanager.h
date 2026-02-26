
/**
 * @file timermanager.h
 * @brief Declarations for timermanager.
 * @details Defines types and functions related to timermanager.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QDateTime>
#include <QMap>
#include <memory>
#include "itimerstorage.h"

/**
 * @brief TimerStatus timer component.
 * @details Implements timer-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
enum class TimerStatus {
    Running, ///< Enum value: running.
    Paused, ///< Enum value: paused.
    Finished ///< Enum value: finished.
};

/**
 * @brief TimerData timer component.
 * @details Implements timer-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct TimerData {
    QString name; /**< Internal state value. */
    int duration; /**< Internal state value. */
    int remaining; /**< Internal state value. */
    bool running; /**< Current state flag. */
    QDateTime lastUpdated; /**< Internal state value. */
    TimerStatus status; /**< Internal state value. */
    QString type; /**< Internal state value. */
    QString groupName; /**< Internal state value. */
};

/**
 * @brief TimerManager Manager that owns and coordinates related objects.
 * @details Provides manager behavior for Timer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class TimerManager : public QObject
{
    Q_OBJECT

public:
/**
 * @brief Create TimerManager instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @param storage Storage backend instance.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit TimerManager(QObject *parent = nullptr, std::unique_ptr<ITimerStorage> storage = {});

/**
 * @brief Add timer.
 * @details Modifies the associated collection and notifies listeners.
 * @param name Name string.
 * @param durationSeconds durationSeconds value.
 * @param type type value.
 * @param group group value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addTimer(const QString &name, int durationSeconds, const QString &type = "Normal", const QString &group = "Default");
/**
 * @brief Remove timer.
 * @details Modifies the associated collection and notifies listeners.
 * @param index Zero-based index.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void removeTimer(int index);
/**
 * @brief Start timer.
 * @details Transitions the state machine and notifies listeners.
 * @param index Zero-based index.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void startTimer(int index);
/**
 * @brief Pause timer.
 * @details Performs the operation and updates state as needed.
 * @param index Zero-based index.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void pauseTimer(int index);
/**
 * @brief Edit timer.
 * @details Performs the operation and updates state as needed.
 * @param index Zero-based index.
 * @param name Name string.
 * @param durationSeconds durationSeconds value.
 * @param type type value.
 * @param group group value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void editTimer(int index, const QString &name, int durationSeconds, const QString &type, const QString &group);
/**
 * @brief Get timers.
 * @details Returns the current value derived from internal state.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QList<TimerData> getTimers() const;
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
    void setStorage(std::unique_ptr<ITimerStorage> storage);
/**
 * @brief Set timers.
 * @details Updates internal state and emits signals as needed.
 * @param list list value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setTimers(const QList<TimerData> &list);
/**
 * @brief Get filtered timers.
 * @details Returns the current value derived from internal state.
 * @param filterType filterType value.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QList<TimerData> getFilteredTimers(const QString &filterType) const;

/**
 * @brief Set recommendation.
 * @details Updates internal state and emits signals as needed.
 * @param fromName Name string.
 * @param toName Name string.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setRecommendation(const QString &fromName, const QString &toName);
/**
 * @brief Remove recommendation.
 * @details Modifies the associated collection and notifies listeners.
 * @param fromName Name string.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void removeRecommendation(const QString &fromName);
/**
 * @brief Get recommendation.
 * @details Returns the current value derived from internal state.
 * @param fromName Name string.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getRecommendation(const QString &fromName) const;
/**
 * @brief Get all recommendations.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QMap<QString, QString> getAllRecommendations() const;

/**
 * @brief Get group timers.
 * @details Returns the current value derived from internal state.
 * @param groupName Name string.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QList<TimerData> getGroupTimers(const QString &groupName) const;

/**
 * @brief Start group.
 * @details Transitions the state machine and notifies listeners.
 * @param groupName Name string.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void startGroup(const QString &groupName);

/**
 * @brief Get next timer.
 * @details Returns the current value derived from internal state.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    TimerData getNextTimer() const;

/**
 * @brief Check whether timer.
 * @details Returns the current value derived from internal state.
 * @param name Name string.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool hasTimer(const QString &name) const;

/**
 * @brief Get deleted timers.
 * @details Returns the current value derived from internal state.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QList<TimerData> getDeletedTimers() const { return deletedTimers; }
/**
 * @brief Get deleted timers ptr.
 * @details Returns the current value derived from internal state.
 * @return List of values.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QList<TimerData>* getDeletedTimersPtr() { return &deletedTimers; }

/**
 * @brief Add deleted timer.
 * @details Modifies the associated collection and notifies listeners.
 * @param t t value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addDeletedTimer(const TimerData &t);
/**
 * @brief Clear deleted timers.
 * @details Modifies the associated collection and notifies listeners.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void clearDeletedTimers();

signals:
/**
 * @brief Timers updated.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void timersUpdated();
/**
 * @brief Timer finished.
 * @details Performs the operation and updates state as needed.
 * @param name Name string.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void timerFinished(const QString &name);
/**
 * @brief Recommendation available.
 * @details Performs the operation and updates state as needed.
 * @param nextName Name string.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void recommendationAvailable(const QString &nextName);

private slots:
/**
 * @brief Update timers.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateTimers();

private:
/**
 * @brief Apply snapshot.
 * @details Performs the operation and updates state as needed.
 * @param snapshot snapshot value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void applySnapshot(const struct TimerSnapshot &snapshot);

    QList<TimerData> timers; /**< Timer-related state. */
    QTimer *tickTimer;

    QMap<QString, QString> recommendations;

    QList<TimerData> deletedTimers; /**< Timer-related state. */
    std::unique_ptr<ITimerStorage> storage; /**< Owned storage backend. */

};

#endif // TIMERMANAGER_H
