#ifndef ALARMWINDOW_H
#define ALARMWINDOW_H

#include <QDialog>
#include "alarmmanager.h"

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

private slots:
    void onAddAlarm();
    void onRemoveAlarm();
    void updateList();

private:
    Ui::AlarmWindow *ui;
    AlarmManager *manager;

    QString alarmsFilePath() const;
};

#endif // ALARMWINDOW_H
