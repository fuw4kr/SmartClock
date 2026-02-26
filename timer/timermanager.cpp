/**
 * @file timermanager.cpp
 * @brief Definitions for timermanager.
 * @details Implements logic declared in the corresponding header for timermanager.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "timermanager.h"
#include "jsontimerstorage.h"
#include "itimerstorage.h"
#include <utility>

TimerManager::TimerManager(QObject *parent, std::unique_ptr<ITimerStorage> storage)
    : QObject(parent)
    , storage(storage ? std::move(storage) : std::make_unique<JsonTimerStorage>())
{
    tickTimer = new QTimer(this);
    connect(tickTimer, &QTimer::timeout, this, &TimerManager::updateTimers);
    tickTimer->start(1000);
}

void TimerManager::addTimer(const QString &name, int durationSeconds, const QString &type, const QString &group)
{
    TimerData t;
    t.name = name;
    t.duration = durationSeconds;
    t.remaining = durationSeconds;
    t.running = false;
    t.lastUpdated = QDateTime::currentDateTime();
    t.status = TimerStatus::Paused;
    t.type = type;
    t.groupName = group.isEmpty() ? "Default" : group;
    timers.append(t);
    emit timersUpdated();
}

void TimerManager::removeTimer(int index)
{
    if (index >= 0 && index < timers.size()) {
        timers.removeAt(index);
        emit timersUpdated();
    }
}

void TimerManager::startTimer(int index)
{
    if (index >= 0 && index < timers.size()) {
        TimerData &t = timers[index];

        if (t.status == TimerStatus::Finished) {
            t.remaining = t.duration;
        }

        t.running = true;
        t.status = TimerStatus::Running;
        t.lastUpdated = QDateTime::currentDateTime();
        emit timersUpdated();
    }
}

void TimerManager::pauseTimer(int index)
{
    if (index >= 0 && index < timers.size()) {
        TimerData &t = timers[index];

        if (!t.running || t.status == TimerStatus::Finished)
            return;

        int elapsed = t.lastUpdated.secsTo(QDateTime::currentDateTime());
        if (elapsed > 0)
            t.remaining = qMax(0, t.remaining - elapsed);

        t.running = false;
        t.status = TimerStatus::Paused;
        t.lastUpdated = QDateTime::currentDateTime();
        emit timersUpdated();
    }
}

void TimerManager::editTimer(int index, const QString &name, int durationSeconds, const QString &type, const QString &group)
{
    if (index >= 0 && index < timers.size()) {
        TimerData &t = timers[index];
        t.name = name;
        t.duration = durationSeconds;
        t.remaining = durationSeconds;
        t.type = type;
        t.running = false;
        t.status = TimerStatus::Paused;
        t.groupName = group.isEmpty() ? "Default" : group;
        emit timersUpdated();
    }
}

void TimerManager::setRecommendation(const QString &fromName, const QString &toName)
{
    recommendations[fromName] = toName;
}

void TimerManager::removeRecommendation(const QString &fromName)
{
    recommendations.remove(fromName);
}

QString TimerManager::getRecommendation(const QString &fromName) const
{
    return recommendations.value(fromName, QString());
}

QMap<QString, QString> TimerManager::getAllRecommendations() const
{
    return recommendations;
}

void TimerManager::saveToFile(const QString &path)
{
    JsonTimerStorage storage(path);
    TimerSnapshot snap;
    snap.timers = timers;
    snap.recommendations = recommendations;
    snap.deletedTimers = deletedTimers;
    storage.save(snap);
}

void TimerManager::loadFromFile(const QString &path)
{
    JsonTimerStorage storage(path);
    TimerSnapshot snap;
    if (!storage.load(snap))
        return;
    applySnapshot(snap);
}

bool TimerManager::save()
{
    if (!storage)
        return false;
    TimerSnapshot snap;
    snap.timers = timers;
    snap.recommendations = recommendations;
    snap.deletedTimers = deletedTimers;
    return storage->save(snap);
}

bool TimerManager::load()
{
    if (!storage)
        return false;
    TimerSnapshot snap;
    if (!storage->load(snap))
        return false;
    applySnapshot(snap);
    return true;
}

void TimerManager::setStorage(std::unique_ptr<ITimerStorage> storage)
{
    this->storage = std::move(storage);
}

void TimerManager::applySnapshot(const TimerSnapshot &snapshot)
{
    timers.clear();
    recommendations = snapshot.recommendations;
    deletedTimers.clear();

    const QDateTime now = QDateTime::currentDateTime();
    for (auto t : snapshot.timers) {
        if (t.type.isEmpty()) t.type = "Normal";
        if (t.groupName.isEmpty()) t.groupName = "Default";

        if (t.running && t.lastUpdated.isValid()) {
            const int diff = t.lastUpdated.secsTo(now);
            t.remaining -= qMax(0, diff);
            if (t.remaining <= 0) {
                t.remaining = 0;
                t.running = false;
            } else {
                t.lastUpdated = now;
            }
        }
        if (t.remaining <= 0) {
            t.status = TimerStatus::Finished;
            t.running = false;
        } else if (t.running) {
            t.status = TimerStatus::Running;
        } else {
            t.status = TimerStatus::Paused;
        }
        timers.append(t);
    }

    for (auto t : snapshot.deletedTimers) {
        if (t.groupName.isEmpty()) t.groupName = "Default";
        t.status = TimerStatus::Paused;
        deletedTimers.append(t);
    }

    emit timersUpdated();
}

void TimerManager::updateTimers()
{
    bool changed = false;

    for (int i = 0; i < timers.size(); ++i) {
        TimerData &t = timers[i];

        if (t.running && t.status == TimerStatus::Running) {
            int elapsed = t.lastUpdated.secsTo(QDateTime::currentDateTime());
            if (elapsed > 0) {
                t.remaining = qMax(0, t.remaining - elapsed);
                t.lastUpdated = QDateTime::currentDateTime();
                changed = true;
            }

            if (t.remaining <= 0) {
                t.remaining = 0;
                t.running = false;
                t.status = TimerStatus::Finished;
                emit timerFinished(t.name);

                QString next = getRecommendation(t.name);
                if (!next.isEmpty())
                    emit recommendationAvailable(next);

                changed = true;
            }
        }
    }

    if (changed)
        emit timersUpdated();
}

QList<TimerData> TimerManager::getTimers() const
{
    return timers;
}

QList<TimerData> TimerManager::getFilteredTimers(const QString &filterType) const
{
    QList<TimerData> result;
    for (const TimerData &t : timers) {
        if (filterType == "All timers") result.append(t);
        else if (filterType == "Running" && t.status == TimerStatus::Running) result.append(t);
        else if (filterType == "Paused" && t.status == TimerStatus::Paused) result.append(t);
        else if (filterType == "Finished" && t.status == TimerStatus::Finished) result.append(t);
    }
    return result;
}

QList<TimerData> TimerManager::getGroupTimers(const QString &groupName) const
{
    QList<TimerData> result;
    for (const TimerData &t : timers) {
        if (t.groupName == groupName)
            result.append(t);
    }
    return result;
}

void TimerManager::startGroup(const QString &groupName)
{
    for (int i = 0; i < timers.size(); ++i) {
        if (timers[i].groupName == groupName) {
            startTimer(i);
        }
    }
}

TimerData TimerManager::getNextTimer() const
{
    TimerData nextTimer;
    int minRemaining = INT_MAX;
    bool found = false;

    for (const auto &timer : timers) {
        if (timer.running && timer.remaining > 0) {
            if (timer.remaining < minRemaining) {
                minRemaining = timer.remaining;
                nextTimer = timer;
                found = true;
            }
        }
    }

    if (!found) {
        nextTimer.name = "";
        nextTimer.remaining = 0;
    }

    return nextTimer;
}

bool TimerManager::hasTimer(const QString &name) const
{
    for (const auto &t : timers)
        if (t.name == name)
            return true;
    return false;
}

void TimerManager::addDeletedTimer(const TimerData &t)
{
    deletedTimers.append(t);
}

void TimerManager::clearDeletedTimers()
{
    deletedTimers.clear();
}
