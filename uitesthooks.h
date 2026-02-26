#ifndef UITESTHOOKS_H
#define UITESTHOOKS_H

#include <QString>
#include <QStringList>
#include <QTime>
#include "alarm/alarmrepeatmode.h"

struct UiTestHooks {
    struct AlarmDialogData {
        bool accepted = false;
        QString name;
        QTime time;
        RepeatMode repeat = RepeatMode::Never;
        QStringList days;
        QString soundPath;
        bool snooze = false;
    };

    struct TimerEditData {
        bool accepted = false;
        QString name;
        int seconds = 0;
        QString type;
        QString group;
    };

    struct TimerSettingsData {
        bool accepted = false;
        bool playSound = true;
        bool runAction = false;
        QString actionPath;
        bool continueAfterExit = false;
        QString melodyPath;
        QString reminderPath;
        bool recommendationsEnabled = false;
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

    static bool isTestMode()
    {
#ifdef UNIT_TESTING
        return true;
#else
        return false;
#endif
    }

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
