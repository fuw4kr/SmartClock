#include "stopwatchmodel.h"
#include "jsonstopwatchstorage.h"
#include <algorithm>

StopwatchModel::StopwatchModel(QObject *parent, std::unique_ptr<IStopwatchStorage> storage)
    : QObject(parent)
    , storage(storage ? std::move(storage) : std::make_unique<JsonStopwatchStorage>())
{
}

bool StopwatchModel::isRunning() const
{
    return running;
}

int StopwatchModel::elapsedMs() const
{
    return elapsed;
}

QTime StopwatchModel::elapsedTime() const
{
    return QTime(0, 0).addMSecs(elapsed);
}

QString StopwatchModel::formattedElapsed() const
{
    return elapsedTime().toString("mm:ss.zzz").left(8);
}

const QList<int>& StopwatchModel::lapDurations() const
{
    return laps;
}

QStringList StopwatchModel::lapTexts() const
{
    QStringList out;
    int accMs = 0;
    for (int i = 0; i < laps.size(); ++i) {
        accMs += laps[i];
        QString lapTimeStr = QTime(0, 0).addMSecs(accMs).toString("mm:ss.zzz").left(8);
        QString deltaStr = QTime(0, 0).addMSecs(laps[i]).toString("mm:ss.zzz").left(8);
        out << QString("Lap %1: %2 (+%3)").arg(i + 1).arg(lapTimeStr).arg(deltaStr);
    }
    return out;
}

void StopwatchModel::start()
{
    if (running)
        return;
    running = true;
    emit stateChanged();
}

void StopwatchModel::stop()
{
    if (!running)
        return;
    running = false;
    emit stateChanged();
}

void StopwatchModel::tick(int ms)
{
    if (!running || ms <= 0)
        return;
    elapsed = std::max(0, elapsed + ms);
}

void StopwatchModel::reset()
{
    running = false;
    elapsed = 0;
    laps.clear();
    emit stateChanged();
    emit lapsChanged();
}

void StopwatchModel::addLap()
{
    if (!running)
        return;
    int sum = 0;
    for (int d : laps)
        sum += d;
    int segMs = elapsed - sum;
    if (segMs < 0)
        segMs = 0;
    laps.append(segMs);
    emit lapsChanged();
}

bool StopwatchModel::load()
{
    if (!storage)
        return false;
    StopwatchSnapshot snap;
    if (!storage->load(snap))
        return false;
    elapsed = std::max(0, snap.elapsedMs);
    running = snap.running;
    laps = snap.lapDurations;
    emit stateChanged();
    emit lapsChanged();
    return true;
}

bool StopwatchModel::save() const
{
    if (!storage)
        return false;
    StopwatchSnapshot snap;
    snap.elapsedMs = elapsed;
    snap.running = running;
    snap.lapDurations = laps;
    return storage->save(snap);
}

void StopwatchModel::setStorage(std::unique_ptr<IStopwatchStorage> storage)
{
    this->storage = std::move(storage);
}
