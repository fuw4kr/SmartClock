#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QDateTime>
#include <QMap>

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
    QString type;
    QString groupName;
};

class TimerManager : public QObject
{
    Q_OBJECT

public:
    explicit TimerManager(QObject *parent = nullptr);

    void addTimer(const QString &name, int durationSeconds, const QString &type = "Normal", const QString &group = "Default");
    void removeTimer(int index);
    void startTimer(int index);
    void pauseTimer(int index);
    void editTimer(int index, const QString &name, int durationSeconds, const QString &type, const QString &group);
    QList<TimerData> getTimers() const;
    void saveToFile(const QString &path);
    void loadFromFile(const QString &path);
    void setTimers(const QList<TimerData> &list);
    QList<TimerData> getFilteredTimers(const QString &filterType) const;

    void setRecommendation(const QString &fromName, const QString &toName);
    void removeRecommendation(const QString &fromName);
    QString getRecommendation(const QString &fromName) const;
    QMap<QString, QString> getAllRecommendations() const;

    QList<TimerData> getGroupTimers(const QString &groupName) const;

    void startGroup(const QString &groupName);

    TimerData getNextTimer() const;

    bool hasTimer(const QString &name) const;

    QList<TimerData> getDeletedTimers() const { return deletedTimers; }
    QList<TimerData>* getDeletedTimersPtr() { return &deletedTimers; }

    void addDeletedTimer(const TimerData &t);
    void clearDeletedTimers();

signals:
    void timersUpdated();
    void timerFinished(const QString &name);
    void recommendationAvailable(const QString &nextName);

private slots:
    void updateTimers();

private:
    QList<TimerData> timers;
    QTimer *tickTimer;

    QMap<QString, QString> recommendations;

    QList<TimerData> deletedTimers;

};

#endif // TIMERMANAGER_H
