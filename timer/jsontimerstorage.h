#ifndef JSONTIMERSTORAGE_H
#define JSONTIMERSTORAGE_H

#include "itimerstorage.h"
#include <QString>

class JsonTimerStorage : public ITimerStorage
{
public:
    explicit JsonTimerStorage(const QString &path = QString());

    bool load(TimerSnapshot &out) override;
    bool save(const TimerSnapshot &in) override;

private:
    QString resolvePath() const;

    QString path;
};

#endif // JSONTIMERSTORAGE_H
