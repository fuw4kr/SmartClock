/**
 * @file timercontroller.cpp
 * @brief Definitions for timercontroller.
 * @details Implements logic declared in the corresponding header for timercontroller.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "timercontroller.h"
#include "../timer/timerwindow.h"
#include <QCoreApplication>
#include <algorithm>

TimerController::TimerController(TimerManager *model, TimerWindow *view, QObject *parent)
    : QObject(parent)
    , model(model)
    , view(view)
{
    connect(view, &TimerWindow::addTimerRequested, this, &TimerController::onAddTimerRequested);
    connect(view, &TimerWindow::editTimerRequested, this, &TimerController::onEditTimerRequested);
    connect(view, &TimerWindow::deleteTimersRequested, this, &TimerController::onDeleteTimersRequested);
    connect(view, &TimerWindow::startPauseRequested, this, &TimerController::onStartPauseRequested);
    connect(view, &TimerWindow::saveRequested, this, &TimerController::onSaveRequested);

    model->load();
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        this->model->save();
    });
}

void TimerController::onAddTimerRequested(const QString &name, int seconds, const QString &type, const QString &group)
{
    model->addTimer(name, seconds, type, group);
    model->save();
}

void TimerController::onEditTimerRequested(int index, const QString &name, int seconds, const QString &type, const QString &group)
{
    model->editTimer(index, name, seconds, type, group);
    model->save();
}

void TimerController::onDeleteTimersRequested(const QList<int> &rows)
{
    QList<int> sorted = rows;
    std::sort(sorted.begin(), sorted.end(), std::greater<int>());
    for (int row : sorted) {
        model->removeTimer(row);
    }
    model->save();
}

void TimerController::onStartPauseRequested(const QList<int> &rows)
{
    for (int row : rows) {
        const auto timers = model->getTimers();
        if (row < 0 || row >= timers.size())
            continue;
        if (timers[row].running)
            model->pauseTimer(row);
        else
            model->startTimer(row);
    }
}

void TimerController::onSaveRequested()
{
    model->save();
}
