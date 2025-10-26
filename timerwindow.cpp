#include "timerwindow.h"
#include "ui_timerwindow.h"
#include "timereditdialog.h"
#include "settingstimerdialog.h"
#include "historytimerwindow.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QSpinBox>
#include <QSoundEffect>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <algorithm>
#include <QCloseEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>

TimerWindow::TimerWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimerWindow)
{
    ui->setupUi(this);

    ui->tableTimers->setHorizontalHeaderLabels({"Name","Remaining time","Status"});
    ui->tableTimers->horizontalHeader()->setStretchLastSection(true);
    ui->tableTimers->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableTimers->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->comboBox->clear();
    ui->comboBox->addItems({"All timers", "Running", "Paused", "Finished"});

    connect(ui->btnAdd, &QPushButton::clicked, this, &TimerWindow::onAddTimer);
    connect(ui->btnSettings, &QPushButton::clicked, this, &TimerWindow::onSettings);
    connect(ui->btnHistory, &QPushButton::clicked, this, &TimerWindow::onHistory);
    connect(ui->btnStartPause, &QPushButton::clicked, this, &TimerWindow::onStartPauseTimer);
    connect(ui->btnDelete, &QPushButton::clicked, this, &TimerWindow::onDeleteTimer);
    connect(&manager, &TimerManager::timersUpdated, this, &TimerWindow::updateTable);
    connect(ui->tableTimers, &QTableWidget::itemDoubleClicked, this, &TimerWindow::onEditTimer);

    connect(&manager, &TimerManager::timerFinished, this, [this](const QString &name){
        if (playSound) {
            auto *sound = new QSoundEffect(this);
            sound->setSource(QUrl("qrc:/s/resources/sounds/soundtimer.wav"));
            sound->setVolume(1.0);
            sound->play();
            QTimer::singleShot(3000, sound, &QObject::deleteLater);
        }

        if (runAction && !actionPath.isEmpty() && QFileInfo::exists(actionPath)) {
            bool ok = QDesktopServices::openUrl(QUrl::fromLocalFile(actionPath));
            if (!ok)
                QMessageBox::warning(this, "Action Failed",
                                     "Could not open:\n" + actionPath);
        }

        QMessageBox::information(this, "Timer Finished",
                                 QString("Timer \"%1\" has finished!").arg(name));
    });

    manager.loadFromFile(timersFilePath());
    loadHistoryJson();

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        manager.saveToFile(timersFilePath());
    });

    QSettings settings("SmartTimerApp", "SmartTimer");
    continueAfterExit = settings.value("continueAfterExit", false).toBool();

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &TimerWindow::updateTable);

}

TimerWindow::~TimerWindow()
{
    delete ui;
}

void TimerWindow::onAddTimer()
{
    TimerEditDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        manager.addTimer(dialog.getName(), dialog.getTotalSeconds());
    }
}

void TimerWindow::onSettings()
{
    SettingsTimerDialog dialog(this);

    dialog.setSoundEnabled(playSound);
    dialog.setActionEnabled(runAction);
    dialog.setActionPath(actionPath);
    dialog.setContinueAfterExit(continueAfterExit);  // 🔹 передаємо поточне значення

    if (dialog.exec() == QDialog::Accepted) {
        playSound = dialog.isSoundEnabled();
        runAction = dialog.isActionEnabled();
        actionPath = dialog.getActionPath();
        continueAfterExit = dialog.continueAfterExit(); // 🔹 отримуємо вибір користувача

        QMessageBox::information(this, "Settings Saved",
                                 QString("Sound: %1\nAction: %2\nContinue timers: %3")
                                     .arg(playSound ? "ON" : "OFF")
                                     .arg(runAction ? "ON" : "OFF")
                                     .arg(continueAfterExit ? "YES" : "NO"));
    }
}

void TimerWindow::onHistory()
{
    HistoryTimerWindow dialog(&deletedTimers, this);

    connect(&dialog, &HistoryTimerWindow::restoreTimer, this, [this](const TimerData &t) {
        manager.addTimer(t.name, t.duration);
        saveHistoryJson();
        updateTable();
    });

    dialog.exec();
}

void TimerWindow::onStartPauseTimer()
{
    auto selectedItems = ui->tableTimers->selectionModel()->selectedRows();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "No selection", "Please select at least one timer.");
        return;
    }

    for (const auto &item : selectedItems) {
        int row = item.row();
        QList<TimerData> timers = manager.getTimers();
        if (row >= timers.size()) continue;

        if (timers[row].running)
            manager.pauseTimer(row);
        else
            manager.startTimer(row);
    }
}

void TimerWindow::onDeleteTimer()
{
    auto selected = ui->tableTimers->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No selection", "Please select one or more timers to delete.");
        return;
    }

    if (QMessageBox::question(this, "Delete Timers",
                              "Delete selected timers?\nThey will be added to History.",
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    QList<int> rows;
    for (auto &i : selected) rows << i.row();
    std::sort(rows.begin(), rows.end(), std::greater<int>());

    auto timers = manager.getTimers();
    for (int r : rows) {
        if (r >= 0 && r < timers.size()) {
            deletedTimers.append(timers[r]);
            manager.removeTimer(r);
        }
    }

    saveHistoryJson();
    updateTable();

    QMessageBox::information(this, "Deleted",
                             QString("Moved %1 timer(s) to History.").arg(rows.size()));
}


void TimerWindow::onEditTimer()
{
    int row = ui->tableTimers->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "No selection", "Select a timer to edit.");
        return;
    }

    QList<TimerData> timers = manager.getTimers();
    if (row >= timers.size()) return;

    const TimerData &t = timers[row];
    TimerEditDialog dialog(this);

    dialog.findChild<QLineEdit*>("nameEdit")->setText(t.name);
    dialog.findChild<QSpinBox*>("spinHours")->setValue(t.duration / 3600);
    dialog.findChild<QSpinBox*>("spinMinutes")->setValue((t.duration % 3600) / 60);
    dialog.findChild<QSpinBox*>("spinSeconds")->setValue(t.duration % 60);

    if (dialog.exec() == QDialog::Accepted) {
        QString newName = dialog.getName();
        int newSeconds = dialog.getTotalSeconds();

        if (newName.isEmpty() || newSeconds <= 0) {
            QMessageBox::warning(this, "Invalid input", "Please enter valid values.");
            return;
        }

        manager.editTimer(row, newName, newSeconds);
    }
}

void TimerWindow::updateTable()
{
    QString filter = ui->comboBox ? ui->comboBox->currentText() : "All timers";

    QList<TimerData> timers;
    if (filter != "All timers")
        timers = manager.getFilteredTimers(filter);
    else
        timers = manager.getTimers();

    ui->tableTimers->setRowCount(timers.size());

    for (int i = 0; i < timers.size(); ++i) {
        const TimerData &t = timers[i];

        int hours = t.remaining / 3600;
        int minutes = (t.remaining % 3600) / 60;
        int seconds = t.remaining % 60;
        QString remainingStr = QString("%1:%2:%3")
                                   .arg(hours, 2, 10, QChar('0'))
                                   .arg(minutes, 2, 10, QChar('0'))
                                   .arg(seconds, 2, 10, QChar('0'));

        QString statusText;
        switch (t.status) {
        case TimerStatus::Running:
            statusText = "Running";
            break;
        case TimerStatus::Paused:
            statusText = "Paused";
            break;
        case TimerStatus::Finished:
            statusText = "Finished";
            break;
        }

        ui->tableTimers->setItem(i, 0, new QTableWidgetItem(t.name));
        ui->tableTimers->setItem(i, 1, new QTableWidgetItem(remainingStr));
        ui->tableTimers->setItem(i, 2, new QTableWidgetItem(statusText));

        if (t.status == TimerStatus::Finished) {
            for (int col = 0; col < 3; ++col) {
                ui->tableTimers->item(i, col)->setBackground(QColor(220, 220, 220));
                ui->tableTimers->item(i, col)->setForeground(QBrush(Qt::darkGray));
            }
        }
    }
}


QString TimerWindow::timersFilePath() const
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/timers.json";
}

QString TimerWindow::historyFilePath() const
{
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/history.json";
}

void TimerWindow::saveHistoryJson() const
{
    QJsonArray arr;
    for (const auto &t : deletedTimers) {
        QJsonObject o;
        o["name"] = t.name;
        o["duration"] = t.duration;
        arr.append(o);
    }
    QFile f(historyFilePath());
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(arr).toJson(QJsonDocument::Compact));
        f.close();
    }
}

void TimerWindow::loadHistoryJson()
{
    QFile f(historyFilePath());
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) return;

    const auto doc = QJsonDocument::fromJson(f.readAll());
    f.close();
    if (!doc.isArray()) return;

    deletedTimers.clear();
    for (const auto &v : doc.array()) {
        QJsonObject o = v.toObject();
        TimerData t;
        t.name = o["name"].toString();
        t.duration = o["duration"].toInt();
        t.remaining = t.duration;
        deletedTimers.append(t);
    }
}

void TimerWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("SmartTimerApp", "SmartTimer");
    settings.setValue("continueAfterExit", continueAfterExit);

    if (!continueAfterExit) {
        QList<TimerData> timers = manager.getTimers();
        for (int i = 0; i < timers.size(); ++i) {
            manager.pauseTimer(i);
        }
    }

    manager.saveToFile(timersFilePath());
    saveHistoryJson();
    event->accept();
}
