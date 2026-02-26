#include "jsontimerstorage.h"
#include "timermanager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

JsonTimerStorage::JsonTimerStorage(const QString &path)
    : path(path)
{
}

QString JsonTimerStorage::resolvePath() const
{
    if (!path.isEmpty())
        return path;

    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/timers.json";
}

bool JsonTimerStorage::load(TimerSnapshot &out)
{
    const QString p = resolvePath();
    QFile f(p);
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return false;

    const auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    out.timers.clear();
    out.recommendations.clear();
    out.deletedTimers.clear();

    if (!doc.isObject())
        return true;

    QJsonObject root = doc.object();
    const QJsonArray arr = root["timers"].toArray();
    for (const auto &v : arr) {
        const QJsonObject o = v.toObject();
        TimerData t;
        t.name        = o["name"].toString();
        t.duration    = o["duration"].toInt();
        t.remaining   = o["remaining"].toInt();
        t.running     = o["running"].toBool();
        t.lastUpdated = QDateTime::fromString(o["lastUpdated"].toString(), Qt::ISODate);
        t.type        = o["type"].toString();
        t.groupName   = o["groupName"].toString();
        out.timers.append(t);
    }

    const QJsonArray deletedArr = root["deletedTimers"].toArray();
    for (const auto &v : deletedArr) {
        const QJsonObject o = v.toObject();
        TimerData t;
        t.name        = o["name"].toString();
        t.duration    = o["duration"].toInt();
        t.remaining   = o["remaining"].toInt();
        t.running     = o["running"].toBool();
        t.lastUpdated = QDateTime::fromString(o["lastUpdated"].toString(), Qt::ISODate);
        t.type        = o["type"].toString();
        t.groupName   = o["groupName"].toString();
        out.deletedTimers.append(t);
    }

    QJsonObject recObj = root["recommendations"].toObject();
    for (auto it = recObj.begin(); it != recObj.end(); ++it)
        out.recommendations[it.key()] = it.value().toString();

    return true;
}

bool JsonTimerStorage::save(const TimerSnapshot &in)
{
    const QString p = resolvePath();

    QJsonArray arr;
    for (const auto &t : in.timers) {
        QJsonObject o;
        o["name"]        = t.name;
        o["duration"]    = t.duration;
        o["remaining"]   = t.remaining;
        o["running"]     = t.running;
        o["lastUpdated"] = t.lastUpdated.toString(Qt::ISODate);
        o["type"]        = t.type;
        o["groupName"]   = t.groupName;
        arr.append(o);
    }

    QJsonArray deletedArr;
    for (const auto &t : in.deletedTimers) {
        QJsonObject o;
        o["name"]        = t.name;
        o["duration"]    = t.duration;
        o["remaining"]   = t.remaining;
        o["running"]     = t.running;
        o["lastUpdated"] = t.lastUpdated.toString(Qt::ISODate);
        o["type"]        = t.type;
        o["groupName"]   = t.groupName;
        deletedArr.append(o);
    }

    QJsonObject recObj;
    for (auto it = in.recommendations.begin(); it != in.recommendations.end(); ++it)
        recObj[it.key()] = it.value();

    QJsonObject root;
    root["timers"] = arr;
    root["recommendations"] = recObj;
    root["deletedTimers"] = deletedArr;

    QJsonDocument doc(root);
    QFile f(p);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    f.write(doc.toJson(QJsonDocument::Compact));
    f.close();
    return true;
}
