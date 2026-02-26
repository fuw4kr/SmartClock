/**
 * @file alarmmanager.cpp
 * @brief Definitions for alarmmanager.
 * @details Implements logic declared in the corresponding header for alarmmanager.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "alarmmanager.h"
#include "jsonalarmstorage.h"
#include <QTimer>
#include <QMap>
#include <utility>

AlarmManager::AlarmManager(QObject *parent, std::unique_ptr<IAlarmStorage> storage)
    : QObject(parent)
    , storage(storage ? std::move(storage) : std::make_unique<JsonAlarmStorage>())
{
    connect(&checkTimer, &QTimer::timeout, this, &AlarmManager::checkAlarms);
    checkTimer.start(1000);
}

void AlarmManager::addAlarm(const AlarmData &data)
{
    AlarmData a = data;
    QDateTime now = QDateTime::currentDateTime();
    if (!a.nextTrigger.isValid())
        a.nextTrigger = computeNextTrigger(a, now);

    alarms.append(a);
    emit alarmsUpdated();
}

void AlarmManager::removeAlarm(int index)
{
    if (index >= 0 && index < alarms.size()) {
        alarms.removeAt(index);
        emit alarmsUpdated();
    }
}

void AlarmManager::toggleAlarm(int index)
{
    if (index < 0 || index >= alarms.size()) return;
    alarms[index].enabled = !alarms[index].enabled;
    if (alarms[index].enabled) {
        alarms[index].nextTrigger = computeInitialTrigger(alarms[index].time);
    }
    emit alarmsUpdated();
}

QList<AlarmData> AlarmManager::getAlarms() const
{
    return alarms;
}

void AlarmManager::snoozeAlarm(const AlarmData &alarm, int minutes)
{
    const int idx = findAlarmIndex(alarm);
    if (idx < 0)
        return;
    alarms[idx].nextTrigger = QDateTime::currentDateTime().addSecs(minutes * 60);
    alarms[idx].enabled = true;
    emit alarmsUpdated();
}

int AlarmManager::findAlarmIndex(const AlarmData &alarm) const
{
    for (int i = 0; i < alarms.size(); ++i) {
        const auto &a = alarms[i];
        if (a.name == alarm.name &&
            a.time == alarm.time &&
            a.repeatMode == alarm.repeatMode &&
            a.days == alarm.days) {
            return i;
        }
    }
    return -1;
}

bool AlarmManager::save()
{
    if (!storage)
        return false;
    return storage->save(alarms);
}

bool AlarmManager::load()
{
    if (!storage)
        return false;
    QList<AlarmData> loaded;
    if (!storage->load(loaded))
        return false;
    alarms = loaded;
    for (auto &a : alarms) {
        if (!a.nextTrigger.isValid())
            a.nextTrigger = computeInitialTrigger(a.time);
    }
    emit alarmsUpdated();
    return true;
}

void AlarmManager::setStorage(std::unique_ptr<IAlarmStorage> storage)
{
    this->storage = std::move(storage);
}

QDateTime AlarmManager::computeInitialTrigger(const QTime &t)
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime trigger(now.date(), t);
    if (trigger <= now)
        trigger = trigger.addDays(1);
    return trigger;
}

QDateTime AlarmManager::computeNextTrigger(const AlarmData &a, const QDateTime &after)
{
    QDate d = after.date();
    QTime t = a.time;
    QDateTime candidate(d, t);

    if (candidate <= after)
        d = d.addDays(1);

    QDateTime weekly = computeWeeklyTrigger(a, d, t);
    if (weekly.isValid())
        return weekly;

    return QDateTime(after.date().addDays(1), t);
}

QDateTime AlarmManager::computeWeeklyTrigger(const AlarmData &a, const QDate &startDate, const QTime &t)
{
    QDate d = startDate;
    const RepeatMode repeat = a.repeatMode;
    static QMap<QString, int> map = {
        {"Mon", 1}, {"Tue", 2}, {"Wed", 3}, {"Thu", 4},
        {"Fri", 5}, {"Sat", 6}, {"Sun", 7}
    };

    for (int i = 0; i < 7; ++i) {
        int dow = d.dayOfWeek();

        bool ok = false;
        switch (repeat) {
        case RepeatMode::EveryDay:
            ok = true;
            break;
        case RepeatMode::Weekdays:
            ok = (dow <= 5);
            break;
        case RepeatMode::Weekends:
            ok = (dow >= 6);
            break;
        case RepeatMode::SpecificDays:
            ok = a.days.contains(map.key(dow));
            break;
        default:
            ok = false;
            break;
        }

        if (ok)
            return QDateTime(d, t);

        d = d.addDays(1);
    }

    return QDateTime();
}

bool AlarmManager::isOneTime(const AlarmData &a)
{
    return a.repeatMode == RepeatMode::Never || a.repeatMode == RepeatMode::Once;
}

bool AlarmManager::isDue(const AlarmData &a, const QDateTime &now)
{
    return a.nextTrigger.isValid() && a.nextTrigger <= now;
}

void AlarmManager::ensureNextTrigger(AlarmData &a)
{
    if (!a.nextTrigger.isValid())
        a.nextTrigger = computeInitialTrigger(a.time);
}

void AlarmManager::handleTriggeredAlarm(AlarmData &a, const QDateTime &now)
{
    emit alarmTriggered(a);

    if (isOneTime(a)) {
        a.enabled = false;
    } else {
        a.nextTrigger = computeNextTrigger(a, now);
        a.enabled = true;
    }

    emit alarmsUpdated();
}

void AlarmManager::checkAlarms()
{
    const QDateTime now = QDateTime::currentDateTime();

    for (AlarmData &a : alarms) {
        if (!a.enabled)
            continue;

        ensureNextTrigger(a);
        if (isDue(a, now))
            handleTriggeredAlarm(a, now);
    }
}

void AlarmManager::saveToFile(const QString &path)
{
    JsonAlarmStorage storage(path);
    storage.save(alarms);
}

void AlarmManager::loadFromFile(const QString &path)
{
    JsonAlarmStorage storage(path);
    QList<AlarmData> loaded;
    if (!storage.load(loaded))
        return;
    alarms = loaded;
    for (auto &a : alarms) {
        if (!a.nextTrigger.isValid())
            a.nextTrigger = computeInitialTrigger(a.time);
    }
    emit alarmsUpdated();
}
