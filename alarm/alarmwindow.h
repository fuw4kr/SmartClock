#ifndef ALARMWINDOW_H
#define ALARMWINDOW_H

#include <QDialog>
#include "alarmmanager.h"
#include "../controllers/alarmcontroller.h"

namespace Ui {
class AlarmWindow;
}

class AlarmWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmWindow(QWidget *parent = nullptr);
    ~AlarmWindow();
    QString getNextAlarmString() const;

signals:
    void addAlarmRequested(const AlarmData &data);
    void removeAlarmsRequested(const QList<int> &rows);
    void alarmToggled(int index, bool enabled);
    void snoozeRequested(const AlarmData &alarm, int minutes);

public slots:
    void setAlarms(const QList<AlarmData> &alarms);
    void showAlarmTriggered(const AlarmData &alarm);

private slots:
    void onAddAlarm();
    void onRemoveAlarm();

private:
    QList<int> selectedAlarmRows() const;
    void showNoSelectionWarning();
    bool confirmDelete(int count);
    void showDeleteInfo(int count);

    Ui::AlarmWindow *ui;
    AlarmManager *manager;
    AlarmController *controller;
};

#endif // ALARMWINDOW_H
