#ifndef JSONSTOPWATCHSTORAGE_H
#define JSONSTOPWATCHSTORAGE_H

#include "istopwatchstorage.h"
#include <QString>

class JsonStopwatchStorage : public IStopwatchStorage
{
public:
    explicit JsonStopwatchStorage(const QString &path = QString());

    bool load(StopwatchSnapshot &out) override;
    bool save(const StopwatchSnapshot &in) override;

private:
    QString resolvePath() const;

    QString path;
};

#endif // JSONSTOPWATCHSTORAGE_H
