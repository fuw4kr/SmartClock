#include "alarmwindow.h"
#include "ui_alarmwindow.h"
#include "alarmsettingsdialog.h"
#include "alarmitemwidget.h"
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <algorithm>
#include <QShortcut>

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
    connect(manager, &AlarmManager::alarmsUpdated, this, &AlarmWindow::updateList);

    auto delShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(delShortcut, &QShortcut::activated, this, &AlarmWindow::onRemoveAlarm);
    manager->loadFromFile(alarmsFilePath());
    updateList();

    connect(manager, &AlarmManager::alarmTriggered, this, [this](const AlarmData &a) {
        QMessageBox::information(this, "⏰ Alarm",
                                 QString("Alarm \"%1\" ringing at %2")
                                     .arg(a.name)
                                     .arg(a.time.toString("HH:mm")));
    });

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        manager->saveToFile(alarmsFilePath());
    });
}

AlarmWindow::~AlarmWindow()
{
    manager->saveToFile(alarmsFilePath());
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

        manager->addAlarm(data);
        manager->saveToFile(alarmsFilePath());
        updateList();
    }
}

void AlarmWindow::onRemoveAlarm()
{
    QList<QListWidgetItem*> selectedItems = ui->listAlarms->selectedItems();

    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "No selection", "Please select one or more alarms to delete.");
        return;
    }

    if (QMessageBox::question(this, "Delete Alarms",
                              QString("Delete %1 selected alarm(s)?").arg(selectedItems.size()),
                              QMessageBox::Yes | QMessageBox::No)
        != QMessageBox::Yes)
        return;

    QList<int> rowsToDelete;
    for (auto *item : selectedItems)
        rowsToDelete << ui->listAlarms->row(item);

    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

    for (int row : rowsToDelete) {
        manager->removeAlarm(row);
        delete ui->listAlarms->takeItem(row);
    }

    manager->saveToFile(alarmsFilePath());
    updateList();

    QMessageBox::information(this, "Deleted",
                             QString("Removed %1 alarm(s).").arg(rowsToDelete.size()));
}

void AlarmWindow::updateList()
{
    ui->listAlarms->clear();
    const auto alarms = manager->getAlarms();

    for (int i = 0; i < alarms.size(); ++i) {
        auto *itemWidget = new AlarmItemWidget(alarms[i]);
        QListWidgetItem *item = new QListWidgetItem(ui->listAlarms);
        item->setSizeHint(itemWidget->sizeHint());
        ui->listAlarms->addItem(item);
        ui->listAlarms->setItemWidget(item, itemWidget);

        connect(itemWidget, &AlarmItemWidget::toggled, this, [this, i](bool enabled) {
            auto list = manager->getAlarms();
            if (i < list.size()) {
                list[i].enabled = enabled;
                manager->toggleAlarm(i);
            }
        });
    }
}

QString AlarmWindow::alarmsFilePath() const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/alarms.json";
}

QString AlarmWindow::getNextAlarmString() const
{
const QList<AlarmData> list = manager->getAlarms();

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
