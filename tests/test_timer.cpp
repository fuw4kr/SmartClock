#include <gtest/gtest.h>
#include <QTest>
#include <QSignalSpy>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QRadioButton>
#include <QDateTime>
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

TEST(TimerEditDialogExtraTest, TargetTimeCalculation) {
    TimerEditDialog dialog;
    QRadioButton* radioTarget = dialog.findChild<QRadioButton*>("radioTargetTime");
    ASSERT_TRUE(radioTarget);

    radioTarget->setChecked(true);

    int secs = dialog.getTotalSeconds();
    EXPECT_GT(secs, 0);
    EXPECT_LE(secs, 24 * 3600);
}