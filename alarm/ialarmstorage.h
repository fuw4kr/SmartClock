#ifndef IALARMSTORAGE_H
#define IALARMSTORAGE_H

#include <QList>

struct AlarmData;

class IAlarmStorage
{
public:
    virtual ~IAlarmStorage() = default;
    virtual bool load(QList<AlarmData> &out) = 0;
    virtual bool save(const QList<AlarmData> &alarms) = 0;
};

#endif // IALARMSTORAGE_H
