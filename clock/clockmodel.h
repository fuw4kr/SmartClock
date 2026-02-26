#ifndef CLOCKMODEL_H
#define CLOCKMODEL_H

#include <QObject>
#include <QList>
#include <memory>
#include "iclockstorage.h"

struct ClockInfo {
    QString zone;
};

class IClockStorage;

class ClockModel : public QObject
{
    Q_OBJECT
public:
    explicit ClockModel(QObject *parent = nullptr, std::unique_ptr<IClockStorage> storage = {});

    const QList<ClockInfo>& clocks() const;
    bool format12h() const;
    void setFormat12h(bool enabled);

    void addClock(const QString &zone);
    void removeClock(int index);

    QString timeTextFor(const ClockInfo &ci) const;

    bool load();
    bool save() const;
    void setStorage(std::unique_ptr<IClockStorage> storage);

signals:
    void clocksChanged();
    void formatChanged(bool enabled);

private:
    QList<ClockInfo> clocksList;
    bool format12 = false;
    std::unique_ptr<IClockStorage> storage;
};

#endif // CLOCKMODEL_H
