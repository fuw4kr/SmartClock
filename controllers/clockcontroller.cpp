#include "clockcontroller.h"
#include "../clock/clockwindow.h"
#include "../clock/jsonclockstorage.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <algorithm>
#include <memory>

ClockController::ClockController(ClockModel *model, ClockWindow *view, QObject *parent)
    : QObject(parent)
    , model(model)
    , view(view)
{
    connect(view, &ClockWindow::addClockRequested, this, &ClockController::onAddClockRequested);
    connect(view, &ClockWindow::removeClocksRequested, this, &ClockController::onRemoveClocksRequested);
    connect(view, &ClockWindow::formatToggled, this, &ClockController::onFormatToggled);

    if (qEnvironmentVariableIsSet("TEST_MODE")) {
        const QString path = QDir::tempPath() + "/smartclock_clocks_test.json";
        QFile::remove(path);
        model->setStorage(std::make_unique<JsonClockStorage>(path));
    }

    model->load();
    view->syncFromModel();

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        this->model->save();
    });
}

void ClockController::onAddClockRequested(const QString &zone)
{
    model->addClock(zone);
    model->save();
    view->syncFromModel();
}

void ClockController::onRemoveClocksRequested(const QList<int> &rows)
{
    QList<int> sorted = rows;
    std::sort(sorted.begin(), sorted.end(), std::greater<int>());
    for (int row : sorted)
        model->removeClock(row);
    model->save();
    view->syncFromModel();
}

void ClockController::onFormatToggled(bool enabled)
{
    model->setFormat12h(enabled);
    model->save();
    view->syncFromModel();
}
