#include "timermanager.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

TimerManager::TimerManager(QObject *parent)
    : QObject(parent)
{
    tickTimer = new QTimer(this);
    connect(tickTimer, &QTimer::timeout, this, &TimerManager::updateTimers);
    tickTimer->start(1000);
}

void TimerManager::addTimer(const QString &name, int durationSeconds)
{
    TimerData t;
    t.name = name;
    t.duration = durationSeconds;
    t.remaining = durationSeconds;
    t.running = false;
    t.lastUpdated = QDateTime::currentDateTime();
    t.status = TimerStatus::Paused;
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


void TimerManager::editTimer(int index, const QString &name, int durationSeconds)
{
    if (index >= 0 && index < timers.size()) {
        timers[index].name = name;
        timers[index].duration = durationSeconds;
        timers[index].remaining = durationSeconds;
        timers[index].running = false;
        emit timersUpdated();
    }
}

void TimerManager::saveToFile(const QString &path)
{
    QJsonArray arr;
    for (const auto &t : timers) {
        QJsonObject o;
        o["name"]        = t.name;
        o["duration"]    = t.duration;
        o["remaining"]   = t.remaining;
        o["running"]     = t.running;
        o["lastUpdated"] = t.lastUpdated.toString(Qt::ISODate);
        arr.append(o);
    }
    QJsonDocument doc(arr);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        return;
    }
    f.write(doc.toJson(QJsonDocument::Compact));
    f.close();
}

void TimerManager::loadFromFile(const QString &path)
{
    QFile f(path);
    if (!f.exists()) {
        return;
    }
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    const auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    if (!doc.isArray()) {
        return;
    }

    timers.clear();
    const QJsonArray arr = doc.array();
    const QDateTime now = QDateTime::currentDateTime();

    for (const auto &v : arr) {
        const QJsonObject o = v.toObject();
        TimerData t;
        t.name        = o["name"].toString();
        t.duration    = o["duration"].toInt();
        t.remaining   = o["remaining"].toInt();
        t.running     = o["running"].toBool();
        t.lastUpdated = QDateTime::fromString(o["lastUpdated"].toString(), Qt::ISODate);

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
        timers.append(t);
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
