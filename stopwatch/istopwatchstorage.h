#ifndef ISTOPWATCHSTORAGE_H
#define ISTOPWATCHSTORAGE_H

#include <QList>

struct StopwatchSnapshot {
    int elapsedMs = 0;
    bool running = false;
    QList<int> lapDurations;
};

class IStopwatchStorage
{
public:
    virtual ~IStopwatchStorage() = default;
    virtual bool load(StopwatchSnapshot &out) = 0;
    virtual bool save(const StopwatchSnapshot &in) = 0;
};

#endif // ISTOPWATCHSTORAGE_H
