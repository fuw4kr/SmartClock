/**
 * @file jsonalarmstorage.cpp
 * @brief Definitions for jsonalarmstorage.
 * @details Implements logic declared in the corresponding header for jsonalarmstorage.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "jsonalarmstorage.h"
#include "alarmmanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

JsonAlarmStorage::JsonAlarmStorage(const QString &path)
    : path(path)
{
}

QString JsonAlarmStorage::resolvePath() const
{
    if (!path.isEmpty())
        return path;

    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/alarms.json";
}

bool JsonAlarmStorage::load(QList<AlarmData> &out)
{
    const QString p = resolvePath();
    QFile f(p);
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return false;

    const auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    out.clear();
    if (!doc.isArray())
        return true;

    for (const auto &v : doc.array()) {
        QJsonObject o = v.toObject();
        AlarmData a;
        a.name = o["name"].toString();
        a.time = QTime::fromString(o["time"].toString(), "HH:mm:ss");
        a.repeatMode = repeatModeFromString(o["repeatMode"].toString());
        for (const auto &d : o["days"].toArray())
            a.days << d.toString();
        a.soundPath = o["soundPath"].toString();
        a.snooze = o["snooze"].toBool();
        a.enabled = o["enabled"].toBool();
        a.nextTrigger = QDateTime::fromString(o["nextTrigger"].toString(), Qt::ISODate);
        out.append(a);
    }

    return true;
}

bool JsonAlarmStorage::save(const QList<AlarmData> &alarms)
{
    const QString p = resolvePath();

    QJsonArray arr;
    for (const auto &a : alarms) {
        QJsonObject o;
        o["name"] = a.name;
        o["time"] = a.time.toString("HH:mm:ss");
        o["repeatMode"] = repeatModeToString(a.repeatMode);
        o["days"] = QJsonArray::fromStringList(a.days);
        o["soundPath"] = a.soundPath;
        o["snooze"] = a.snooze;
        o["enabled"] = a.enabled;
        o["nextTrigger"] = a.nextTrigger.toString(Qt::ISODate);
        arr.append(o);
    }

    QJsonDocument doc(arr);
    QFile f(p);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    f.write(doc.toJson());
    f.close();
    return true;
}
