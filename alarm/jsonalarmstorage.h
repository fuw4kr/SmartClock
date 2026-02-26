#ifndef JSONALARMSTORAGE_H
#define JSONALARMSTORAGE_H

#include "ialarmstorage.h"
#include <QString>

class JsonAlarmStorage : public IAlarmStorage
{
public:
    explicit JsonAlarmStorage(const QString &path = QString());

    bool load(QList<AlarmData> &out) override;
    bool save(const QList<AlarmData> &alarms) override;

private:
    QString resolvePath() const;

    QString path;
};

#endif // JSONALARMSTORAGE_H
