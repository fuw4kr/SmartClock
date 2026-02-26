#ifndef ITIMERSTORAGE_H
#define ITIMERSTORAGE_H

#include <QList>
#include <QMap>

struct TimerData;

struct TimerSnapshot {
    QList<TimerData> timers;
    QMap<QString, QString> recommendations;
    QList<TimerData> deletedTimers;
};

class ITimerStorage
{
public:
    virtual ~ITimerStorage() = default;
    virtual bool load(TimerSnapshot &out) = 0;
    virtual bool save(const TimerSnapshot &in) = 0;
};

#endif // ITIMERSTORAGE_H
