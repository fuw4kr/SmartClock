#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QDateTime>

enum class TimerStatus {
    Running,
    Paused,
    Finished
};

struct TimerData {
    QString name;
    int duration;
    int remaining;
    bool running;
    QDateTime lastUpdated;
    TimerStatus status;
};

class TimerManager : public QObject
{
    Q_OBJECT

public:
    explicit TimerManager(QObject *parent = nullptr);

    void addTimer(const QString &name, int durationSeconds);
    void removeTimer(int index);
    void startTimer(int index);
    void pauseTimer(int index);
    void editTimer(int index, const QString &name, int durationSeconds);
    QList<TimerData> getTimers() const;
    void saveToFile(const QString &path);
    void loadFromFile(const QString &path);
    void setTimers(const QList<TimerData> &list);
    QList<TimerData> getFilteredTimers(const QString &filterType) const;

signals:
    void timersUpdated();
    void timerFinished(const QString &name);

private slots:
    void updateTimers();

private:
    QList<TimerData> timers;
    QTimer *tickTimer;
};

#endif // TIMERMANAGER_H
