#ifndef ALARMREPEATMODE_H
#define ALARMREPEATMODE_H

#include <QString>

enum class RepeatMode {
    Never,
    EveryDay,
    Weekdays,
    Weekends,
    SpecificDays,
    Once
};

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
