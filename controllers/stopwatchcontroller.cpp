#include "stopwatchcontroller.h"
#include "../stopwatch/stopwatchwindow.h"
#include "../stopwatch/jsonstopwatchstorage.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <memory>

StopwatchController::StopwatchController(StopwatchModel *model, StopwatchWindow *view, QObject *parent)
    : QObject(parent)
    , model(model)
    , view(view)
{
    connect(view, &StopwatchWindow::startStopRequested, this, &StopwatchController::onStartStopRequested);
    connect(view, &StopwatchWindow::lapRequested, this, &StopwatchController::onLapRequested);

    if (qEnvironmentVariableIsSet("TEST_MODE")) {
        const QString path = QDir::tempPath() + "/smartclock_stopwatch_test.json";
        QFile::remove(path);
        model->setStorage(std::make_unique<JsonStopwatchStorage>(path));
    }

    model->load();
    view->syncFromModel();

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        this->model->save();
    });
    connect(view, &QObject::destroyed, this, [this]() {
        this->model->save();
    });
}

void StopwatchController::onStartStopRequested()
{
    if (model->isRunning())
        model->stop();
    else
        model->start();
    model->save();
    view->syncFromModel();
}

void StopwatchController::onLapRequested()
{
    if (model->isRunning())
        model->addLap();
    else
        model->reset();
    model->save();
    view->syncFromModel();
}
