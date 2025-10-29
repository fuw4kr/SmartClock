#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QList>
#include <QSoundEffect>

struct AlarmData {
    QString name;
    QTime time;
    QString repeatMode;
    QStringList days;
    QString soundPath;
    bool snooze;
    bool enabled;
    QDateTime nextTrigger;
};

class AlarmManager : public QObject
{
    Q_OBJECT

public:
    explicit AlarmManager(QObject *parent = nullptr);
    void addAlarm(const AlarmData &data);
    void removeAlarm(int index);
    void toggleAlarm(int index);
    QList<AlarmData> getAlarms() const;
    void saveToFile(const QString &path);
    void loadFromFile(const QString &path);

signals:
    void alarmsUpdated();
    void alarmTriggered(const AlarmData &alarm);

private slots:
    void checkAlarms();

private:
    static QDateTime computeInitialTrigger(const QTime &t);
    static QDateTime computeNextTrigger(const AlarmData &a, const QDateTime &after);

    QList<AlarmData> alarms;
    QTimer checkTimer;
};

#endif // ALARMMANAGER_H
