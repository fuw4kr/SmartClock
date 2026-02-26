#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QList>
#include <memory>
#include "ialarmstorage.h"
#include "alarmrepeatmode.h"

struct AlarmData {
    QString name;
    QTime time;
    RepeatMode repeatMode = RepeatMode::Never;
    QStringList days;
    QString soundPath;
    bool snooze;
    bool enabled;
    QDateTime nextTrigger;
};

class AlarmManager : public QObject
{
    Q_OBJECT

public:
    explicit AlarmManager(QObject *parent = nullptr, std::unique_ptr<IAlarmStorage> storage = {});
    void addAlarm(const AlarmData &data);
    void removeAlarm(int index);
    void toggleAlarm(int index);
    QList<AlarmData> getAlarms() const;
    void saveToFile(const QString &path);
    void loadFromFile(const QString &path);
    bool save();
    bool load();
    void setStorage(std::unique_ptr<IAlarmStorage> storage);
    void snoozeAlarm(const AlarmData &alarm, int minutes);

signals:
    void alarmsUpdated();
    void alarmTriggered(const AlarmData &alarm);

private slots:
    void checkAlarms();

private:
    static QDateTime computeInitialTrigger(const QTime &t);
    static QDateTime computeNextTrigger(const AlarmData &a, const QDateTime &after);
    static QDateTime computeWeeklyTrigger(const AlarmData &a, const QDate &startDate, const QTime &t);
    static bool isOneTime(const AlarmData &a);
    static bool isDue(const AlarmData &a, const QDateTime &now);
    static void ensureNextTrigger(AlarmData &a);
    void handleTriggeredAlarm(AlarmData &a, const QDateTime &now);
    int findAlarmIndex(const AlarmData &alarm) const;

    QList<AlarmData> alarms;
    QTimer checkTimer;
    std::unique_ptr<IAlarmStorage> storage;
};

#endif // ALARMMANAGER_H
