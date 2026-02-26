
/**
 * @file alarmrepeatmode.h
 * @brief Declarations for alarmrepeatmode.
 * @details Defines types and functions related to alarmrepeatmode.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ALARMREPEATMODE_H
#define ALARMREPEATMODE_H

#include <QString>

/**
 * @brief RepeatMode type.
 * @details Holds data and behavior specific to RepeatMode.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
enum class RepeatMode {
    Never, ///< Enum value: never.
    EveryDay, ///< Enum value: every day.
    Weekdays, ///< Enum value: weekdays.
    Weekends, ///< Enum value: weekends.
    SpecificDays, ///< Enum value: specific days.
    Once ///< Repeat mode: once.
};

/**
 * @brief Convert repeat mode to string.
 * @details Formats the value as a user-facing string.
 * @param mode Mode value.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
inline QString repeatModeToString(RepeatMode mode)
{
    switch (mode) {
    case RepeatMode::Never:
        return "Never";
    case RepeatMode::EveryDay:
        return "Every day";
    case RepeatMode::Weekdays:
        return "Weekdays";
    case RepeatMode::Weekends:
        return "Weekends";
    case RepeatMode::SpecificDays:
        return "Specific days";
    case RepeatMode::Once:
        return "Once";
    }
    return "Never";
}

/**
 * @brief Parse repeat mode from string.
 * @details Parses the value from a user-provided string.
 * @param text Text value.
 * @return Parsed value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
inline RepeatMode repeatModeFromString(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.compare("Never", Qt::CaseInsensitive) == 0)
        return RepeatMode::Never;
    if (trimmed.compare("Once", Qt::CaseInsensitive) == 0)
        return RepeatMode::Once;
    if (trimmed.compare("Every day", Qt::CaseInsensitive) == 0)
        return RepeatMode::EveryDay;
    if (trimmed.compare("Weekdays", Qt::CaseInsensitive) == 0)
        return RepeatMode::Weekdays;
    if (trimmed.compare("Weekends", Qt::CaseInsensitive) == 0)
        return RepeatMode::Weekends;
    if (trimmed.compare("Specific days", Qt::CaseInsensitive) == 0 ||
        trimmed.compare("Specific Days", Qt::CaseInsensitive) == 0)
        return RepeatMode::SpecificDays;
    return RepeatMode::Never;
}

#endif // ALARMREPEATMODE_H
