#ifndef ICLOCKSTORAGE_H
#define ICLOCKSTORAGE_H

#include <QList>

struct ClockInfo;

struct ClockSnapshot {
    QList<ClockInfo> clocks;
    bool format12h = false;
};

class IClockStorage
{
public:
    virtual ~IClockStorage() = default;
    virtual bool load(ClockSnapshot &out) = 0;
    virtual bool save(const ClockSnapshot &in) = 0;
};

#endif // ICLOCKSTORAGE_H
