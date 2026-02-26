
/**
 * @file uitesthooks.h
 * @brief Declarations for uitesthooks.
 * @details Defines types and functions related to uitesthooks.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef UITESTHOOKS_H
#define UITESTHOOKS_H

#include <QString>
#include <QStringList>
#include <QTime>
#include "alarm/alarmrepeatmode.h"

/**
 * @brief UiTestHooks type.
 * @details Holds data and behavior specific to UiTestHooks.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
struct UiTestHooks {
/**
 * @brief AlarmDialogData alarm component.
 * @details Implements alarm-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
    struct AlarmDialogData {
        bool accepted = false; /**< Internal state value. */
        QString name; /**< Internal state value. */
        QTime time; /**< Internal state value. */
        RepeatMode repeat = RepeatMode::Never; /**< Internal state value. */
        QStringList days; /**< Internal state value. */
        QString soundPath; /**< Filesystem path. */
        bool snooze = false; /**< Internal state value. */
    };

/**
 * @brief TimerEditData timer component.
 * @details Implements timer-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
    struct TimerEditData {
        bool accepted = false; /**< Internal state value. */
        QString name; /**< Internal state value. */
        int seconds = 0; /**< Internal state value. */
        QString type; /**< Internal state value. */
        QString group; /**< Internal state value. */
    };

/**
 * @brief TimerSettingsData timer component.
 * @details Implements timer-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
    struct TimerSettingsData {
        bool accepted = false; /**< Internal state value. */
        bool playSound = true; /**< Internal state value. */
        bool runAction = false; /**< Internal state value. */
        QString actionPath; /**< Filesystem path. */
        bool continueAfterExit = false; /**< Internal state value. */
        QString melodyPath; /**< Filesystem path. */
        QString reminderPath; /**< Filesystem path. */
        bool recommendationsEnabled = false; /**< Internal state value. */
    };

    static inline AlarmDialogData alarmDialog{};
    static inline TimerEditData timerEdit{};
    static inline TimerSettingsData timerSettings{};

    static inline bool nextQuestionResult = true;
    static inline bool nextSnoozeResult = false;
    static inline bool nextRecommendationStart = false;
    static inline QString nextFileDialogPath{};
    static inline bool nextOpenUrlResult = true;

    static inline int infoCount = 0;
    static inline int warningCount = 0;
    static inline int criticalCount = 0;
    static inline QString lastInfoTitle{};
    static inline QString lastInfoText{};
    static inline QString lastWarningTitle{};
    static inline QString lastWarningText{};

/**
 * @brief Check whether test mode.
 * @details Returns the current value derived from internal state.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static bool isTestMode()
    {
#ifdef UNIT_TESTING
        return true; /**< Internal state value. */
#else
        return false; /**< Internal state value. */
#endif
    }

/**
 * @brief Reset operation.
 * @details Transitions the state machine and notifies listeners.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static void reset()
    {
        alarmDialog = AlarmDialogData{};
        timerEdit = TimerEditData{};
        timerSettings = TimerSettingsData{};
        nextQuestionResult = true;
        nextSnoozeResult = false;
        nextRecommendationStart = false;
        nextFileDialogPath.clear();
        nextOpenUrlResult = true;
        infoCount = 0;
        warningCount = 0;
        criticalCount = 0;
        lastInfoTitle.clear();
        lastInfoText.clear();
        lastWarningTitle.clear();
        lastWarningText.clear();
    }
};

#endif // UITESTHOOKS_H
