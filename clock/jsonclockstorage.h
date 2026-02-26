#ifndef JSONCLOCKSTORAGE_H
#define JSONCLOCKSTORAGE_H

#include "iclockstorage.h"
#include <QString>

class JsonClockStorage : public IClockStorage
{
public:
    explicit JsonClockStorage(const QString &path = QString());

    bool load(ClockSnapshot &out) override;
    bool save(const ClockSnapshot &in) override;

private:
    QString resolvePath() const;

    QString path;
};

#endif // JSONCLOCKSTORAGE_H
