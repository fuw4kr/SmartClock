/**
 * @file clockmodel.cpp
 * @brief Definitions for clockmodel.
 * @details Implements logic declared in the corresponding header for clockmodel.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "clockmodel.h"
#include "jsonclockstorage.h"
#include <QDateTime>
#include <QTimeZone>
#include <QLocale>
#include <utility>

ClockModel::ClockModel(QObject *parent, std::unique_ptr<IClockStorage> storage)
    : QObject(parent)
    , storage(storage ? std::move(storage) : std::make_unique<JsonClockStorage>())
{
}

const QList<ClockInfo>& ClockModel::clocks() const
{
    return clocksList;
}

bool ClockModel::format12h() const
{
    return format12;
}

void ClockModel::setFormat12h(bool enabled)
{
    if (format12 == enabled)
        return;
    format12 = enabled;
    emit formatChanged(format12);
}

void ClockModel::addClock(const QString &zone)
{
    if (zone.isEmpty())
        return;
    clocksList.append(ClockInfo{zone});
    emit clocksChanged();
}

void ClockModel::removeClock(int index)
{
    if (index < 0 || index >= clocksList.size())
        return;
    clocksList.removeAt(index);
    emit clocksChanged();
}

QString ClockModel::timeTextFor(const ClockInfo &ci) const
{
    QTimeZone tz(ci.zone.toUtf8());
    QDateTime nowTz = QDateTime::currentDateTimeUtc().toTimeZone(tz);
    QLocale en(QLocale::English);
    QString fmt = format12 ? "hh:mm:ss AP" : "HH:mm:ss";
    QString timeText = en.toString(nowTz.time(), fmt);
    return QString("%1 - %2").arg(QString::fromUtf8(tz.id()), timeText);
}

bool ClockModel::load()
{
    if (!storage)
        return false;
    ClockSnapshot snap;
    if (!storage->load(snap))
        return false;
    clocksList = snap.clocks;
    format12 = snap.format12h;
    emit formatChanged(format12);
    emit clocksChanged();
    return true;
}

bool ClockModel::save() const
{
    if (!storage)
        return false;
    ClockSnapshot snap;
    snap.clocks = clocksList;
    snap.format12h = format12;
    return storage->save(snap);
}

void ClockModel::setStorage(std::unique_ptr<IClockStorage> storage)
{
    this->storage = std::move(storage);
}
