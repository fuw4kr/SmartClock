
/**
 * @file soundalarmaction.h
 * @brief Declarations for soundalarmaction.
 * @details Defines types and functions related to soundalarmaction.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef SOUNDALARMACTION_H
#define SOUNDALARMACTION_H

#include "ialarmaction.h"
#include <QString>

/**
 * @brief SoundAlarmAction Action implementation triggered by alarm or timer events.
 * @details Provides action behavior for SoundAlarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class SoundAlarmAction : public IAlarmAction
{
public:
/**
 * @brief Create SoundAlarmAction instance.
 * @details Initializes instance state.
 * @param soundPath Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit SoundAlarmAction(const QString &soundPath = QString());

/**
 * @brief Execute.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void execute() override;
/**
 * @brief Set sound path.
 * @details Updates internal state and emits signals as needed.
 * @param soundPath Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setSoundPath(const QString &soundPath);

private:
    QString soundPath; /**< Filesystem path. */
};

#endif // SOUNDALARMACTION_H
