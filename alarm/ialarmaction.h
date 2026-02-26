
/**
 * @file ialarmaction.h
 * @brief Declarations for ialarmaction.
 * @details Defines types and functions related to ialarmaction.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef IALARMACTION_H
#define IALARMACTION_H

/**
 * @brief IAlarmAction interface.
 * @details Defines the contract for AlarmAction implementations.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class IAlarmAction
{
public:
/**
 * @brief Destroy IAlarmAction instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual ~IAlarmAction() = default;
/**
 * @brief Execute.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    virtual void execute() = 0;
};

#endif // IALARMACTION_H
