#ifndef ALARMCONTROLLER_H
#define ALARMCONTROLLER_H

#include <QObject>
#include <QList>
#include "../alarm/alarmmanager.h"

class AlarmWindow;

class AlarmController : public QObject
{
    Q_OBJECT
public:
    explicit AlarmController(AlarmManager *model, AlarmWindow *view, QObject *parent = nullptr);

    QString nextAlarmString() const;

private slots:
    void onAddAlarmRequested(const AlarmData &data);
    void onRemoveAlarmsRequested(const QList<int> &rows);
    void onAlarmToggled(int index, bool enabled);
    void onSnoozeRequested(const AlarmData &alarm, int minutes);
    void onModelUpdated();

private:
    AlarmManager *model;
    AlarmWindow *view;
};

#endif // ALARMCONTROLLER_H
