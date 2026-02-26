#ifndef STOPWATCHMODEL_H
#define STOPWATCHMODEL_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QTime>
#include <memory>
#include "istopwatchstorage.h"

class StopwatchModel : public QObject
{
    Q_OBJECT
public:
    explicit StopwatchModel(QObject *parent = nullptr, std::unique_ptr<IStopwatchStorage> storage = {});

    bool isRunning() const;
    int elapsedMs() const;
    QTime elapsedTime() const;
    QString formattedElapsed() const;
    const QList<int>& lapDurations() const;
    QStringList lapTexts() const;

    void start();
    void stop();
    void tick(int ms);
    void reset();
    void addLap();

    bool load();
    bool save() const;
    void setStorage(std::unique_ptr<IStopwatchStorage> storage);

signals:
    void stateChanged();
    void lapsChanged();

private:
    int elapsed = 0;
    bool running = false;
    QList<int> laps;
    std::unique_ptr<IStopwatchStorage> storage;
};

#endif // STOPWATCHMODEL_H
