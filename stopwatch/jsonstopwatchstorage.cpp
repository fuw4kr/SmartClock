/**
 * @file jsonstopwatchstorage.cpp
 * @brief Definitions for jsonstopwatchstorage.
 * @details Implements logic declared in the corresponding header for jsonstopwatchstorage.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "jsonstopwatchstorage.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTime>

JsonStopwatchStorage::JsonStopwatchStorage(const QString &path)
    : path(path)
{
}

QString JsonStopwatchStorage::resolvePath() const
{
    if (!path.isEmpty())
        return path;

    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/stopwatch.json";
}

bool JsonStopwatchStorage::load(StopwatchSnapshot &out)
{
    out = StopwatchSnapshot{};
    const QString p = resolvePath();
    QFile f(p);
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    f.close();

    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return true;

    QJsonObject obj = doc.object();
    out.elapsedMs = qMax(0, obj.value("elapsed_ms").toInt(0));
    out.running = obj.value("running").toBool(false);

    QJsonArray durArr = obj.value("durations").toArray();
    for (const QJsonValue &v : durArr)
        out.lapDurations.append(v.toInt());

    return true;
}

bool JsonStopwatchStorage::save(const StopwatchSnapshot &in)
{
    const QString p = resolvePath();

    if (in.elapsedMs <= 0 && !in.running && in.lapDurations.isEmpty()) {
        QFile::remove(p);
        return true;
    }

    QJsonObject obj;
    obj["elapsed_ms"] = in.elapsedMs;
    obj["running"] = in.running;

    QJsonArray durArr;
    for (int d : in.lapDurations)
        durArr.append(d);
    obj["durations"] = durArr;

    QJsonArray lapsArr;
    int accMs = 0;
    for (int i = 0; i < in.lapDurations.size(); ++i) {
        accMs += in.lapDurations[i];
        QString lapTimeStr = QTime(0, 0).addMSecs(accMs).toString("mm:ss.zzz").left(8);
        QString deltaStr = QTime(0, 0).addMSecs(in.lapDurations[i]).toString("mm:ss.zzz").left(8);
        QString text = QString("Lap %1: %2 (+%3)").arg(i + 1).arg(lapTimeStr).arg(deltaStr);
        lapsArr.append(text);
    }
    obj["laps"] = lapsArr;

    QJsonDocument doc(obj);
    QFile f(p);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write(doc.toJson(QJsonDocument::Compact));
    f.close();
    return true;
}
