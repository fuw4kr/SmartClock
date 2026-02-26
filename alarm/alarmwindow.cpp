/**
 * @file alarmwindow.cpp
 * @brief Definitions for alarmwindow.
 * @details Implements logic declared in the corresponding header for alarmwindow.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "alarmwindow.h"
#include "ui_alarmwindow.h"
#include "alarmsettingsdialog.h"
#include "alarmfactory.h"
#include "soundalarmaction.h"
#include <QMessageBox>
#include <QListWidget>
#include <algorithm>
#include <QShortcut>
#include <memory>

AlarmWindow::AlarmWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AlarmWindow)
    , manager(new AlarmManager(this))
{
    ui->setupUi(this);
    setWindowTitle("Alarms");

    ui->listAlarms->setSpacing(6);
    ui->listAlarms->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listAlarms->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->listAlarms->setFocusPolicy(Qt::StrongFocus);

    connect(ui->btnAdd, &QPushButton::clicked, this, &AlarmWindow::onAddAlarm);
    connect(ui->btnRemove, &QPushButton::clicked, this, &AlarmWindow::onRemoveAlarm);
    // Model updates are handled by controller.

    auto delShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(delShortcut, &QShortcut::activated, this, &AlarmWindow::onRemoveAlarm);

    controller = new AlarmController(manager, this, this);
}

AlarmWindow::~AlarmWindow()
{
    delete ui;
}

void AlarmWindow::onAddAlarm()
{
    AlarmSettingsDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        AlarmData data;
        data.name = dlg.getLabel().isEmpty() ? "Alarm" : dlg.getLabel();
        data.time = dlg.getTime();
        data.repeatMode = dlg.getRepeatMode();
        data.days = dlg.getDays();
        data.soundPath = dlg.getSoundPath().isEmpty()
                             ? "qrc:/s/resources/sounds/soundalarm.wav"
                             : dlg.getSoundPath();
        data.snooze = dlg.getSnooze();
        data.enabled = true;

        emit addAlarmRequested(data);
    }
}

void AlarmWindow::onRemoveAlarm()
{
    const QList<int> rowsToDelete = selectedAlarmRows();
    if (rowsToDelete.isEmpty()) {
        showNoSelectionWarning();
        return;
    }

    if (!confirmDelete(rowsToDelete.size()))
        return;

    QList<int> sorted = rowsToDelete;
    std::sort(sorted.begin(), sorted.end(), std::greater<int>());

    emit removeAlarmsRequested(sorted);
    showDeleteInfo(sorted.size());
}

QList<int> AlarmWindow::selectedAlarmRows() const
{
    QList<int> rows;
    const QList<QListWidgetItem*> selectedItems = ui->listAlarms->selectedItems();
    for (auto *item : selectedItems)
        rows << ui->listAlarms->row(item);
    return rows;
}

void AlarmWindow::showNoSelectionWarning()
{
    QMessageBox::warning(this, "No selection", "Please select one or more alarms to delete.");
}

bool AlarmWindow::confirmDelete(int count)
{
    return QMessageBox::question(this, "Delete Alarms",
                                 QString("Delete %1 selected alarm(s)?").arg(count),
                                 QMessageBox::Yes | QMessageBox::No)
        == QMessageBox::Yes;
}

void AlarmWindow::showDeleteInfo(int count)
{
    QMessageBox::information(this, "Deleted",
                             QString("Removed %1 alarm(s).").arg(count));
}

void AlarmWindow::setAlarms(const QList<AlarmData> &alarms)
{
    ui->listAlarms->clear();
    for (int i = 0; i < alarms.size(); ++i) {
        auto *itemWidget = AlarmFactory::createAlarmWidget(alarms[i], ui->listAlarms);
        QListWidgetItem *item = new QListWidgetItem(ui->listAlarms);
        item->setSizeHint(itemWidget->sizeHint());
        ui->listAlarms->addItem(item);
        ui->listAlarms->setItemWidget(item, itemWidget);

        connect(itemWidget, &AlarmItemWidget::toggled, this, [this, i](bool enabled) {
            emit alarmToggled(i, enabled);
        });
    }
}

QString AlarmWindow::getNextAlarmString() const
{
    return controller ? controller->nextAlarmString() : "No alarms set";
}

void AlarmWindow::showAlarmTriggered(const AlarmData &a)
{
    std::unique_ptr<IAlarmAction> action = std::make_unique<SoundAlarmAction>(a.soundPath);
    action->execute();

    if (a.snooze) {
        QMessageBox msg;
        msg.setWindowTitle("Alarm");
        msg.setText(QString("Alarm \"%1\" ringing at %2")
                        .arg(a.name)
                        .arg(a.time.toString("HH:mm")));
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setButtonText(QMessageBox::Yes, "Snooze 5 min");
        msg.setButtonText(QMessageBox::No, "Stop");
        const int res = msg.exec();
        if (res == QMessageBox::Yes)
            emit snoozeRequested(a, 5);
    } else {
        QMessageBox::information(this, "Alarm",
                                 QString("Alarm \"%1\" ringing at %2")
                                     .arg(a.name)
                                     .arg(a.time.toString("HH:mm")));
    }
}
