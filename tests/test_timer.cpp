/**
 * @file test_timer.cpp
 * @brief Definitions for test_timer.
 * @details Implements logic declared in the corresponding header for test_timer.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include <gtest/gtest.h>
#include <QTest>
#include <QSignalSpy>
#include <QCoreApplication>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QItemSelectionModel>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QCloseEvent>
#include "../timer/timermanager.h"
#include "../timer/historytimerwindow.h"
#include "../timer/settingstimerdialog.h"
#include "../timer/timereditdialog.h"

class TimerManagerTest : public ::testing::Test {
protected:
    void SetUp() override { manager = new TimerManager(); }
    void TearDown() override { delete manager; }
    TimerManager* manager;
};

TEST_F(TimerManagerTest, AddTimerIncreasesCount) {
    int before = manager->getTimers().size();
    manager->addTimer("TestTimer", 60);
    EXPECT_EQ(manager->getTimers().size(), before + 1);
}

TEST_F(TimerManagerTest, AddedTimerHasCorrectData) {
    manager->addTimer("Pomodoro", 1500, "Normal", "Work");
    const auto& t = manager->getTimers().last();
    EXPECT_EQ(t.name, "Pomodoro");
    EXPECT_EQ(t.duration, 1500);
    EXPECT_EQ(t.remaining, 1500);
    EXPECT_EQ(t.groupName, "Work");
    EXPECT_FALSE(t.running);
    EXPECT_EQ(t.status, TimerStatus::Paused);
}

TEST_F(TimerManagerTest, AddTimerEmptyGroupDefaultsToDefault) {
    manager->addTimer("NoGroup", 10, "Normal", "");
    const auto& t = manager->getTimers().last();
    EXPECT_EQ(t.groupName, "Default");
}

TEST_F(TimerManagerTest, StartAndPauseChangesState) {
    manager->addTimer("T", 5);
    manager->startTimer(0);
    auto t = manager->getTimers()[0];
    EXPECT_TRUE(t.running);
    EXPECT_EQ(t.status, TimerStatus::Running);
    manager->pauseTimer(0);
    t = manager->getTimers()[0];
    EXPECT_FALSE(t.running);
    EXPECT_EQ(t.status, TimerStatus::Paused);
}

TEST_F(TimerManagerTest, EditTimerUpdatesValues) {
    manager->addTimer("Old", 10);
    manager->editTimer(0, "New", 20, "Reminder", "Study");
    const auto& t = manager->getTimers()[0];
    EXPECT_EQ(t.name, "New");
    EXPECT_EQ(t.duration, 20);
    EXPECT_EQ(t.type, "Reminder");
    EXPECT_EQ(t.groupName, "Study");
}

TEST_F(TimerManagerTest, EditTimerEmptyGroupDefaultsToDefault) {
    manager->addTimer("Old", 10);
    manager->editTimer(0, "New", 20, "Normal", "");
    const auto& t = manager->getTimers()[0];
    EXPECT_EQ(t.groupName, "Default");
}

TEST_F(TimerManagerTest, PauseTimerWhenAlreadyPausedDoesNothing) {
    manager->addTimer("Paused", 5);
    const int before = manager->getTimers()[0].remaining;
    manager->pauseTimer(0);
    const auto& t = manager->getTimers()[0];
    EXPECT_FALSE(t.running);
    EXPECT_EQ(t.status, TimerStatus::Paused);
    EXPECT_EQ(t.remaining, before);
}

TEST_F(TimerManagerTest, RemoveTimerReducesCount) {
    manager->addTimer("Del", 10);
    int before = manager->getTimers().size();
    manager->removeTimer(0);
    EXPECT_EQ(manager->getTimers().size(), before - 1);
}

TEST_F(TimerManagerTest, RecommendationSetAndRemove) {
    manager->setRecommendation("A", "B");
    EXPECT_EQ(manager->getRecommendation("A"), "B");
    manager->removeRecommendation("A");
    EXPECT_TRUE(manager->getRecommendation("A").isEmpty());
}

TEST_F(TimerManagerTest, GetNextTimerFindsShortestRunning) {
    manager->addTimer("Long", 100);
    manager->addTimer("Short", 5);
    manager->startTimer(0);
    manager->startTimer(1);
    EXPECT_EQ(manager->getNextTimer().name, "Short");
}

TEST_F(TimerManagerTest, DeletedTimersAddAndClear) {
    TimerData t;
    t.name = "Old";
    t.duration = 10;
    manager->addDeletedTimer(t);
    EXPECT_EQ(manager->getDeletedTimers().size(), 1);
    manager->clearDeletedTimers();
    EXPECT_TRUE(manager->getDeletedTimers().isEmpty());
}

TEST_F(TimerManagerTest, HasTimerReturnsFalseWhenMissing) {
    manager->addTimer("Exists", 10);
    EXPECT_FALSE(manager->hasTimer("Missing"));
}

TEST(TimerManagerExtraTest, SaveAndLoadRoundTrip) {
    TimerManager m;
    m.addTimer("Study", 120);
    m.addTimer("Break", 300, "Melody", "Rest");
    m.setRecommendation("Study", "Break");

    QString path = QDir::temp().absoluteFilePath("test_timers.json");
    m.saveToFile(path);

    TimerManager m2;
    m2.loadFromFile(path);

    ASSERT_EQ(m2.getTimers().size(), 2);
    EXPECT_EQ(m2.getTimers()[0].name, "Study");
    EXPECT_EQ(m2.getTimers()[1].type, "Melody");
    EXPECT_EQ(m2.getRecommendation("Study"), "Break");
}

TEST(TimerManagerExtraTest, LoadDefaultsTypeAndGroupAndFinishedStatus) {
    QTemporaryDir dir;
    QString path = dir.path() + "/timers_custom.json";

    QJsonObject timerObj;
    timerObj["name"] = "T1";
    timerObj["duration"] = 5;
    timerObj["remaining"] = 0;
    timerObj["running"] = false;
    timerObj["lastUpdated"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    timerObj["type"] = "";
    timerObj["groupName"] = "";

    QJsonArray arr;
    arr.append(timerObj);
    QJsonObject root;
    root["timers"] = arr;
    root["recommendations"] = QJsonObject();
    root["deletedTimers"] = QJsonArray();

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    f.close();

    TimerManager m;
    m.loadFromFile(path);

    ASSERT_EQ(m.getTimers().size(), 1);
    const auto& t = m.getTimers()[0];
    EXPECT_EQ(t.type, "Normal");
    EXPECT_EQ(t.groupName, "Default");
    EXPECT_EQ(t.status, TimerStatus::Finished);
    EXPECT_FALSE(t.running);
}

TEST(TimerManagerExtraTest, StartTimerResetsRemainingWhenFinished) {
    QTemporaryDir dir;
    QString path = dir.path() + "/timers_finished.json";

    QJsonObject timerObj;
    timerObj["name"] = "Done";
    timerObj["duration"] = 7;
    timerObj["remaining"] = 0;
    timerObj["running"] = false;
    timerObj["lastUpdated"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    timerObj["type"] = "Normal";
    timerObj["groupName"] = "Default";

    QJsonArray arr;
    arr.append(timerObj);
    QJsonObject root;
    root["timers"] = arr;
    root["recommendations"] = QJsonObject();
    root["deletedTimers"] = QJsonArray();

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    f.close();

    TimerManager m;
    m.loadFromFile(path);
    m.startTimer(0);

    const auto& t = m.getTimers()[0];
    EXPECT_EQ(t.remaining, t.duration);
    EXPECT_TRUE(t.running);
    EXPECT_EQ(t.status, TimerStatus::Running);
}

TEST(TimerManagerExtraTest, GetFilteredAndGroupedTimers) {
    TimerManager m;
    m.addTimer("A", 10, "Normal", "Work");
    m.addTimer("B", 5, "Normal", "Play");
    m.startTimer(1);
    m.pauseTimer(1);

    auto all = m.getFilteredTimers("All timers");
    auto paused = m.getFilteredTimers("Paused");
    auto group = m.getGroupTimers("Work");

    EXPECT_GE(all.size(), 2);
    EXPECT_FALSE(paused.isEmpty());
    EXPECT_EQ(group[0].groupName, "Work");
}

TEST(HistoryTimerWindowTest, SignalsWork) {
    QList<TimerData> deleted = {
        {"T1", 60, 60, false, QDateTime::currentDateTime(), TimerStatus::Paused, "Normal", "Default"}
    };
    HistoryTimerWindow window(&deleted);
    QSignalSpy spyRestore(&window, &HistoryTimerWindow::restoreTimer);
    QSignalSpy spyChanged(&window, &HistoryTimerWindow::historyChanged);

    emit window.restoreTimer(deleted.first());
    emit window.historyChanged();

    EXPECT_EQ(spyRestore.count(), 1);
    EXPECT_EQ(spyChanged.count(), 1);
}

TEST(SettingsTimerDialogTest, TogglesWorkCorrectly) {
    TimerManager manager;
    SettingsTimerDialog dialog(&manager);

    dialog.setSoundEnabled(true);
    dialog.setActionEnabled(true);
    EXPECT_TRUE(dialog.isSoundEnabled());
    EXPECT_TRUE(dialog.isActionEnabled());

    dialog.setContinueAfterExit(true);
    EXPECT_TRUE(dialog.continueAfterExit());

    QString path = "/usr/bin/test";
    dialog.setActionPath(path);
    EXPECT_EQ(dialog.getActionPath(), path);
}

TEST(SettingsTimerDialogExtraTest, AddAndRemoveRecommendations) {
    TimerManager m;
    m.addTimer("X", 60);
    m.addTimer("Y", 30);

    SettingsTimerDialog dialog(&m);
    dialog.setRecommendationsEnabled(true);

    m.setRecommendation("X", "Y");

    auto recs = m.getAllRecommendations();
    ASSERT_TRUE(recs.contains("X"));
    EXPECT_EQ(recs["X"], "Y");

    m.removeRecommendation("X");
    EXPECT_TRUE(m.getRecommendation("X").isEmpty());
}

TEST(SettingsTimerDialogExtraTest, BrowseAndMelodyPaths) {
    TimerManager m;
    SettingsTimerDialog dialog(&m);

    dialog.setMelodySoundPath("melody.wav");
    dialog.setReminderSoundPath("rem.wav");

    EXPECT_EQ(dialog.melodySoundPath(), "melody.wav");
    EXPECT_EQ(dialog.reminderSoundPath(), "rem.wav");
}

TEST(SettingsTimerDialogExtraTest, RecommendationTableDropsInvalidEntries) {
    TimerManager m;
    m.addTimer("ValidA", 10);
    m.addTimer("ValidB", 20);
    m.setRecommendation("ValidA", "ValidB");
    m.setRecommendation("Missing", "ValidB");

    SettingsTimerDialog dialog(&m);
    auto *table = dialog.findChild<QTableWidget*>("tableRecommendations");
    ASSERT_TRUE(table);

    EXPECT_EQ(table->rowCount(), 1);
    EXPECT_EQ(table->item(0, 0)->text(), "ValidA");
    EXPECT_EQ(table->item(0, 1)->text(), "ValidB");
}

TEST(SettingsTimerDialogExtraTest, ToggleSectionsVisibility) {
    TimerManager m;
    SettingsTimerDialog dialog(&m);

    dialog.setSoundEnabled(true);
    auto *soundWidget = dialog.findChild<QWidget*>("widget_3");
    ASSERT_TRUE(soundWidget);
    EXPECT_FALSE(soundWidget->isHidden());

    dialog.setSoundEnabled(false);
    EXPECT_TRUE(soundWidget->isHidden());

    dialog.setActionEnabled(true);
    auto *actionWidget = dialog.findChild<QWidget*>("widgetFileSelect");
    ASSERT_TRUE(actionWidget);
    EXPECT_FALSE(actionWidget->isHidden());

    dialog.setActionEnabled(false);
    EXPECT_TRUE(actionWidget->isHidden());

    dialog.setRecommendationsEnabled(true);
    auto *recWidget = dialog.findChild<QWidget*>("widgetRecommendations");
    ASSERT_TRUE(recWidget);
    EXPECT_FALSE(recWidget->isHidden());

    dialog.setRecommendationsEnabled(false);
    EXPECT_TRUE(recWidget->isHidden());
}

TEST(SettingsTimerDialogExtraTest, RecommendationEnabledFlagMatchesCheckbox) {
    TimerManager m;
    SettingsTimerDialog dialog(&m);

    dialog.setRecommendationsEnabled(true);
    EXPECT_TRUE(dialog.isRecommendationsEnabled());

    dialog.setRecommendationsEnabled(false);
    EXPECT_FALSE(dialog.isRecommendationsEnabled());
}

TEST(TimerEditDialogTest, FieldsReturnCorrectValues) {
    TimerEditDialog dialog;

    QLineEdit* nameEdit = dialog.findChild<QLineEdit*>("nameEdit");
    QSpinBox* h = dialog.findChild<QSpinBox*>("spinHours");
    QSpinBox* m = dialog.findChild<QSpinBox*>("spinMinutes");
    QSpinBox* s = dialog.findChild<QSpinBox*>("spinSeconds");
    QComboBox* typeCombo = dialog.findChild<QComboBox*>("comboType");
    QComboBox* groupCombo = dialog.findChild<QComboBox*>("comboGroup");

    ASSERT_TRUE(nameEdit && h && m && s && typeCombo && groupCombo);

    nameEdit->setText("Focus");
    h->setValue(0);
    m->setValue(25);
    s->setValue(0);
    typeCombo->setCurrentText("Reminder");
    groupCombo->setCurrentText("Work");

    EXPECT_EQ(dialog.getName(), "Focus");
    EXPECT_EQ(dialog.getType(), "Reminder");
    EXPECT_EQ(dialog.getGroup(), "Work");
    EXPECT_GT(dialog.getTotalSeconds(), 0);
}

TEST(TimerEditDialogExtraTest, TargetTimeFromNowIsInFuture) {
    TimerEditDialog dialog;
    QRadioButton* radioTarget = dialog.findChild<QRadioButton*>("radioTargetTime");
    QDateTimeEdit* targetEdit = dialog.findChild<QDateTimeEdit*>("targetTimeEdit");
    ASSERT_TRUE(radioTarget && targetEdit);

    radioTarget->setChecked(true);
    targetEdit->setTime(QTime::currentTime());
    int secs = dialog.getTotalSeconds();
    EXPECT_GT(secs, 0);
    EXPECT_LE(secs, 24 * 3600);
}

TEST(TimerEditDialogExtraTest, ToggleInputModeShowsTargetTimeEdit) {
    TimerEditDialog dialog;
    QRadioButton* radioTarget = dialog.findChild<QRadioButton*>("radioTargetTime");
    QRadioButton* radioDuration = dialog.findChild<QRadioButton*>("radioDuration");
    QDateTimeEdit* targetEdit = dialog.findChild<QDateTimeEdit*>("targetTimeEdit");
    QWidget* durationWidget = dialog.findChild<QWidget*>("widget");
    ASSERT_TRUE(radioTarget && radioDuration && targetEdit && durationWidget);

    radioTarget->setChecked(true);
    QTest::qWait(300);
    EXPECT_FALSE(targetEdit->isHidden());
    EXPECT_TRUE(durationWidget->isHidden());

    radioDuration->setChecked(true);
    QTest::qWait(300);
    EXPECT_FALSE(durationWidget->isHidden());
}

TEST(TimerEditDialogExtraTest, TargetTimeCalculation) {
    TimerEditDialog dialog;
    QRadioButton* radioTarget = dialog.findChild<QRadioButton*>("radioTargetTime");
    ASSERT_TRUE(radioTarget);

    radioTarget->setChecked(true);

    int secs = dialog.getTotalSeconds();
    EXPECT_GT(secs, 0);
    EXPECT_LE(secs, 24 * 3600);
}

TEST(HistoryTimerWindowTest, WidgetActionsShowWhenSelectingMultiple) {
    QList<TimerData> deleted = {
        {"A", 60, 60, false, QDateTime::currentDateTime(), TimerStatus::Paused, "Normal", "Default"},
        {"B", 120, 120, false, QDateTime::currentDateTime(), TimerStatus::Paused, "Normal", "Default"}
    };
    HistoryTimerWindow window(&deleted);

    auto *table = window.findChild<QTableWidget*>("tableHistory");
    auto *actions = window.findChild<QWidget*>("widgetActions");
    ASSERT_TRUE(table && actions);

    auto *sel = table->selectionModel();
    ASSERT_TRUE(sel);
    sel->select(table->model()->index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
    sel->select(table->model()->index(1, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
    QCoreApplication::processEvents();

    EXPECT_FALSE(actions->isHidden());
}

TEST(HistoryTimerWindowTest, RestoreSelectedEarlyReturnWhenSingle) {
    QList<TimerData> deleted = {
        {"A", 60, 60, false, QDateTime::currentDateTime(), TimerStatus::Paused, "Normal", "Default"}
    };
    HistoryTimerWindow window(&deleted);

    auto *table = window.findChild<QTableWidget*>("tableHistory");
    ASSERT_TRUE(table);
    table->selectRow(0);

    EXPECT_TRUE(QMetaObject::invokeMethod(&window, "onRestoreSelected", Qt::DirectConnection));
    EXPECT_EQ(deleted.size(), 1);
}

TEST(HistoryTimerWindowTest, DeleteSelectedEarlyReturnWhenSingle) {
    QList<TimerData> deleted = {
        {"A", 60, 60, false, QDateTime::currentDateTime(), TimerStatus::Paused, "Normal", "Default"}
    };
    HistoryTimerWindow window(&deleted);

    auto *table = window.findChild<QTableWidget*>("tableHistory");
    ASSERT_TRUE(table);
    table->selectRow(0);

    EXPECT_TRUE(QMetaObject::invokeMethod(&window, "onDeleteSelected", Qt::DirectConnection));
    EXPECT_EQ(deleted.size(), 1);
}

TEST(SettingsTimerDialogCoverage, AddRecommendationEmptyOrSameShowsWarning) {
    TimerManager m;
    m.addTimer("A", 10);
    SettingsTimerDialog dialog(&m);

    auto *comboFrom = dialog.findChild<QComboBox*>("comboFrom");
    auto *comboTo = dialog.findChild<QComboBox*>("comboTo");
    ASSERT_TRUE(comboFrom && comboTo);

    comboFrom->setCurrentText("A");
    comboTo->setCurrentText("A"); // Однакові таймери

    // Авто-закриття QMessageBox::warning
    QTimer::singleShot(50, []() {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            if (auto *mb = qobject_cast<QMessageBox*>(widget)) mb->accept();
        }
    });

    EXPECT_NO_THROW(QMetaObject::invokeMethod(&dialog, "onAddRecommendationClicked", Qt::DirectConnection));
    EXPECT_TRUE(m.getAllRecommendations().isEmpty()); // Рекомендація не мала додатися
}

TEST(SettingsTimerDialogCoverage, DeleteRecommendationWorks) {
    TimerManager m;
    m.addTimer("A", 10);
    m.addTimer("B", 10);
    m.setRecommendation("A", "B");

    SettingsTimerDialog dialog(&m);
    auto *table = dialog.findChild<QTableWidget*>("tableRecommendations");
    ASSERT_TRUE(table);

    table->selectRow(0); // Вибираємо рекомендацію

    EXPECT_NO_THROW(QMetaObject::invokeMethod(&dialog, "onDeleteRecommendationClicked", Qt::DirectConnection));
    EXPECT_TRUE(m.getAllRecommendations().isEmpty()); // Має бути видалено
}

TEST(HistoryTimerWindowCoverage, CloseButtonAcceptsDialog) {
    QList<TimerData> deleted;
    HistoryTimerWindow window(&deleted);

    auto *btnClose = window.findChild<QPushButton*>("btnClose");
    ASSERT_TRUE(btnClose);

    QTimer::singleShot(50, [&btnClose]() {
        btnClose->click();
    });

    // exec() завершиться, коли натиснеться btnClose
    EXPECT_EQ(window.exec(), QDialog::Accepted);
}
