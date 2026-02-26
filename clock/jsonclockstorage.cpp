/**
 * @file jsonclockstorage.cpp
 * @brief Definitions for jsonclockstorage.
 * @details Implements logic declared in the corresponding header for jsonclockstorage.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "jsonclockstorage.h"
#include "clockmodel.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

JsonClockStorage::JsonClockStorage(const QString &path)
    : path(path)
{
}

QString JsonClockStorage::resolvePath() const
{
    if (!path.isEmpty())
        return path;

    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/clocks.json";
}

bool JsonClockStorage::load(ClockSnapshot &out)
{
    const QString p = resolvePath();
    QFile f(p);
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return false;

    const auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    out.clocks.clear();
    out.format12h = false;
    if (!doc.isObject())
        return true;

    QJsonObject root = doc.object();
    out.format12h = root.value("format12h").toBool(false);

    QJsonArray arr = root.value("clocks").toArray();
    for (const QJsonValue &v : arr) {
        if (!v.isObject()) continue;
        QJsonObject o = v.toObject();
        ClockInfo ci;
        ci.zone = o.value("zone").toString();
        if (!ci.zone.isEmpty())
            out.clocks.append(ci);
    }
    return true;
}

bool JsonClockStorage::save(const ClockSnapshot &in)
{
    const QString p = resolvePath();

    QJsonObject root;
    root["format12h"] = in.format12h;

    QJsonArray arr;
    for (const ClockInfo &ci : in.clocks) {
        QJsonObject obj;
        obj["zone"] = ci.zone;
        arr.append(obj);
    }
    root["clocks"] = arr;

    QJsonDocument doc(root);
    QFile f(p);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
    return true;
}
