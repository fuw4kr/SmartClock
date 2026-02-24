#include "timerwindow.h"
#include "ui_TimerWindow.h"
#include "timereditdialog.h"
#include "settingstimerdialog.h"
#include "historytimerwindow.h"
#include "../mainwindow.h"

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
#include <QShortcut>

#ifdef Q_OS_WIN
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shobjidl.h>

static void setTaskbarProgress(QWidget *widget, int value, int maximum)
{
    ITaskbarList3* pTaskbar = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL,
                                  IID_ITaskbarList3, (void**)&pTaskbar);
    if (SUCCEEDED(hr) && pTaskbar) {
        pTaskbar->HrInit();

        QWidget *topWidget = widget->window();
        HWND hwnd = reinterpret_cast<HWND>(topWidget->winId());

        if (maximum <= 0) {
            pTaskbar->SetProgressState(hwnd, TBPF_NOPROGRESS);
        } else {
            pTaskbar->SetProgressState(hwnd, TBPF_NORMAL);
            pTaskbar->SetProgressValue(hwnd, value, maximum);
        }

        pTaskbar->Release();
    }
}
#endif

TimerWindow::TimerWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimerWindow)
{
    ui->setupUi(this);

    setWindowTitle("Timer");

    ui->tableTimers->setColumnCount(4);
    ui->tableTimers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableTimers->setHorizontalHeaderLabels({"Name", "Remaining", "Status", "Type"});
    ui->tableTimers->setShowGrid(false);

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

    auto delShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(delShortcut, &QShortcut::activated, this, &TimerWindow::onDeleteTimer);

    auto spaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(spaceShortcut, &QShortcut::activated, this, &TimerWindow::onStartPauseTimer);

    connect(&manager, &TimerManager::timerFinished, this, [this](const QString &name) {
        QSettings settings("SmartTimerApp", "SmartTimer");
        bool soundEnabled = settings.value("playSound", true).toBool();

        QString melody = settings.value("melodySoundPath").toString();
        QString reminder = settings.value("reminderSoundPath").toString();

        QString type = "Normal";
        for (const auto &t : manager.getTimers()) {
            if (t.name == name) {
                type = t.type;
                break;
            }
        }

        if (soundEnabled) {
            QString soundPath = "qrc:/s/resources/sounds/soundtimer.wav";
            if (type == "Melody" && !melody.isEmpty())
                soundPath = QUrl::fromLocalFile(melody).toString();
            else if (type == "Reminder" && !reminder.isEmpty())
                soundPath = QUrl::fromLocalFile(reminder).toString();

            auto *sound = new QSoundEffect(this);
            sound->setSource(QUrl(soundPath));
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

        QString msg;
        if (type == "Melody")
            msg = QString("Timer \"%1\" finished with melody!").arg(name);
        else if (type == "Reminder")
            msg = QString("Reminder \"%1\" time is up!").arg(name);
        else
            msg = QString("Timer \"%1\" has finished!").arg(name);

        QMessageBox::information(this, "Timer Finished", msg);
    });

    connect(&manager, &TimerManager::recommendationAvailable, this, [this](const QString &nextName) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Recommendation");
        msgBox.setText(QString("Timer finished! Recommended to start timer '%1'.").arg(nextName));
        QPushButton *startBtn = msgBox.addButton("Start Recommended Timer", QMessageBox::AcceptRole);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.exec();

        if (msgBox.clickedButton() == startBtn) {
            auto timers = manager.getTimers();
            for (int i = 0; i < timers.size(); ++i) {
                if (timers[i].name == nextName) {
                    manager.startTimer(i);
                    QMessageBox::information(this, "Started", QString("Timer '%1' started.").arg(nextName));
                    break;
                }
            }
        }
    });

    manager.loadFromFile(timersFilePath());
    loadHistoryJson();

    connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
        manager.saveToFile(timersFilePath());
    });

    QSettings settings("SmartTimerApp", "SmartTimer");

    playSound = settings.value("playSound", true).toBool();
    runAction = settings.value("runAction", false).toBool();
    actionPath = settings.value("actionPath", "").toString();
    continueAfterExit = settings.value("continueAfterExit", false).toBool();

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &TimerWindow::updateTable);

    connect(ui->btnStartGroup, &QPushButton::clicked, this, [this]() {
        const QString group = ui->comboGroups->currentText();
        if (!group.isEmpty()) {
            manager.startGroup(group);
            QMessageBox::information(this, "Group started",
                                     QString("Started all timers in group '%1'").arg(group));
        }
    });

    connect(&manager, &TimerManager::timersUpdated, this, &TimerWindow::updateNextUpLabel);

}

TimerWindow::~TimerWindow()
{
    delete ui;
}

void TimerWindow::onAddTimer()
{
    TimerEditDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        manager.addTimer(dialog.getName(), dialog.getTotalSeconds(), dialog.getType(), dialog.getGroup());
        updateTable();
    }
}

void TimerWindow::onSettings()
{
    SettingsTimerDialog dialog(&manager, this);

    QSettings settings("SmartTimerApp", "SmartTimer");

    dialog.setSoundEnabled(settings.value("playSound", true).toBool());
    dialog.setActionEnabled(settings.value("runAction", false).toBool());
    dialog.setActionPath(settings.value("actionPath", "").toString());
    dialog.setContinueAfterExit(settings.value("continueAfterExit", false).toBool());
    dialog.setMelodySoundPath(settings.value("melodySoundPath", "").toString());
    dialog.setReminderSoundPath(settings.value("reminderSoundPath", "").toString());
    dialog.setRecommendationsEnabled(settings.value("recommendationsEnabled", false).toBool());

    if (dialog.exec() == QDialog::Accepted) {
        playSound = dialog.isSoundEnabled();
        runAction = dialog.isActionEnabled();
        actionPath = dialog.getActionPath();
        continueAfterExit = dialog.continueAfterExit();

        settings.setValue("playSound", playSound);
        settings.setValue("runAction", runAction);
        settings.setValue("actionPath", actionPath);
        settings.setValue("melodySoundPath", dialog.melodySoundPath());
        settings.setValue("reminderSoundPath", dialog.reminderSoundPath());
        settings.setValue("continueAfterExit", continueAfterExit);
        settings.setValue("recommendationsEnabled", dialog.isRecommendationsEnabled());

        manager.saveToFile(timersFilePath());
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

    connect(&dialog, &HistoryTimerWindow::historyChanged, this, [this]() {
        saveHistoryJson();
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
    if (row >= timers.size())
        return;

    const TimerData &t = timers[row];
    TimerEditDialog dialog(this);

    dialog.findChild<QLineEdit*>("nameEdit")->setText(t.name);
    dialog.findChild<QSpinBox*>("spinHours")->setValue(t.duration / 3600);
    dialog.findChild<QSpinBox*>("spinMinutes")->setValue((t.duration % 3600) / 60);
    dialog.findChild<QSpinBox*>("spinSeconds")->setValue(t.duration % 60);

    QComboBox *combo = dialog.findChild<QComboBox*>("comboType");
    if (combo) {
        int idx = combo->findText(t.type);
        if (idx >= 0)
            combo->setCurrentIndex(idx);
    }

    if (dialog.exec() == QDialog::Accepted) {
        QString newName = dialog.getName();
        int newSeconds = dialog.getTotalSeconds();
        QString newType = dialog.getType();

        if (newName.isEmpty() || newSeconds <= 0) {
            QMessageBox::warning(this, "Invalid input", "Please enter valid values.");
            return;
        }

        manager.editTimer(row, newName, newSeconds, newType, "Default");
        updateTable();
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
        ui->tableTimers->setItem(i, 3, new QTableWidgetItem(t.type));

        if (t.status == TimerStatus::Finished) {
            for (int col = 0; col < 3; ++col) {
                ui->tableTimers->item(i, col)->setBackground(QColor(220, 220, 220));
                ui->tableTimers->item(i, col)->setForeground(QBrush(Qt::darkGray));
            }
        }
    }
    ui->comboGroups->clear();
    QStringList groups;
    for (const auto &t : manager.getTimers()) {
        if (!groups.contains(t.groupName))
            groups << t.groupName;
    }
    ui->comboGroups->addItems(groups);
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
    settings.setValue("playSound", playSound);
    settings.setValue("runAction", runAction);
    settings.setValue("actionPath", actionPath);

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

void TimerWindow::updateNextUpLabel()
{
    TimerData next = manager.getNextTimer();

    if (!next.name.isEmpty()) {
        int hours = next.remaining / 3600;
        int minutes = (next.remaining % 3600) / 60;
        int seconds = next.remaining % 60;

        QString timeText;
        if (hours > 0)
            timeText = QString("%1:%2:%3")
                           .arg(hours, 2, 10, QLatin1Char('0'))
                           .arg(minutes, 2, 10, QLatin1Char('0'))
                           .arg(seconds, 2, 10, QLatin1Char('0'));
        else
            timeText = QString("%1:%2")
                           .arg(minutes, 2, 10, QLatin1Char('0'))
                           .arg(seconds, 2, 10, QLatin1Char('0'));

        ui->labelNextUp->setText(QString("Next up: %1 (in %2)")
                                     .arg(next.name)
                                     .arg(timeText));

#ifdef Q_OS_WIN
        if (!next.name.isEmpty() && next.duration > 0) {
            int percent = int(100.0 * (1.0 - double(next.remaining) / next.duration));
            setTaskbarProgress(window(), percent, 100);
        } else {
            ui->labelNextUp->setText("Next up: None");
            setTaskbarProgress(window(), 0, 0);
        }
#endif
    } else {
        ui->labelNextUp->setText("Next up: None");
#ifdef Q_OS_WIN
        setTaskbarProgress(window(), 0, 0);
#endif
    }
}
