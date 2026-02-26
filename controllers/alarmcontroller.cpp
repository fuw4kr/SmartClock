/**
 * @file alarmcontroller.cpp
 * @brief Definitions for alarmcontroller.
 * @details Implements logic declared in the corresponding header for alarmcontroller.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "alarmcontroller.h"
#include "../alarm/alarmwindow.h"
#include <QCoreApplication>
#include <algorithm>

AlarmController::AlarmController(AlarmManager *model, AlarmWindow *view, QObject *parent)
    : QObject(parent)
    , model(model)
    , view(view)
{
    connect(view, &AlarmWindow::addAlarmRequested, this, &AlarmController::onAddAlarmRequested);
    connect(view, &AlarmWindow::removeAlarmsRequested, this, &AlarmController::onRemoveAlarmsRequested);
    connect(view, &AlarmWindow::alarmToggled, this, &AlarmController::onAlarmToggled);
    connect(view, &AlarmWindow::snoozeRequested, this, &AlarmController::onSnoozeRequested);

    connect(model, &AlarmManager::alarmsUpdated, this, &AlarmController::onModelUpdated);
    connect(model, &AlarmManager::alarmTriggered, view, &AlarmWindow::showAlarmTriggered);

    model->load();
    onModelUpdated();

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        this->model->save();
    });
}

QString AlarmController::nextAlarmString() const
{
    const QList<AlarmData> list = model->getAlarms();

    if (list.isEmpty())
        return "No alarms set";

    QDateTime best;
    bool found = false;

    for (const auto &a : list) {
        if (!a.enabled) continue;
        if (!a.nextTrigger.isValid()) continue;

        if (!found || a.nextTrigger < best) {
            best = a.nextTrigger;
            found = true;
        }
    }

    if (!found)
        return "No upcoming alarms";

    const QDate today = QDate::currentDate();
    QString dayStr;
    if (best.date() == today) {
        dayStr = "Today";
    } else if (best.date() == today.addDays(1)) {
        dayStr = "Tomorrow";
    } else {
        dayStr = best.date().toString("ddd");
    }

    return QString("%1 %2").arg(dayStr, best.time().toString("hh:mm"));
}

void AlarmController::onAddAlarmRequested(const AlarmData &data)
{
    model->addAlarm(data);
    model->save();
}

void AlarmController::onRemoveAlarmsRequested(const QList<int> &rows)
{
    QList<int> sorted = rows;
    std::sort(sorted.begin(), sorted.end(), std::greater<int>());
    for (int row : sorted) {
        model->removeAlarm(row);
    }
    model->save();
}

void AlarmController::onAlarmToggled(int index, bool /*enabled*/)
{
    model->toggleAlarm(index);
    model->save();
}

void AlarmController::onSnoozeRequested(const AlarmData &alarm, int minutes)
{
    model->snoozeAlarm(alarm, minutes);
    model->save();
}

void AlarmController::onModelUpdated()
{
    view->setAlarms(model->getAlarms());
}
