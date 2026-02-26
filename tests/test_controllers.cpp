#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QDateTime>
#include "../alarm/alarmwindow.h"
#include "../timer/timerwindow.h"
#include "../clock/clockwindow.h"
#include "../stopwatch/stopwatchwindow.h"

TEST(AlarmControllerTest, AddToggleRemoveAndSnoozeViaViewSignals) {
    AlarmWindow w;
    AlarmManager *manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    AlarmData a;
    a.name = "A";
    a.time = QTime::currentTime().addSecs(3600);
    a.repeatMode = RepeatMode::Never;
    a.enabled = true;
    a.nextTrigger = QDateTime();

    emit w.addAlarmRequested(a);
    ASSERT_EQ(manager->getAlarms().size(), 1);

    bool before = manager->getAlarms().first().enabled;
    emit w.alarmToggled(0, !before);
    bool after = manager->getAlarms().first().enabled;
    EXPECT_NE(before, after);

    emit w.snoozeRequested(manager->getAlarms().first(), 1);
    auto snoozed = manager->getAlarms().first();
    EXPECT_TRUE(snoozed.enabled);
    EXPECT_TRUE(snoozed.nextTrigger.isValid());

    emit w.removeAlarmsRequested(QList<int>{0});
    EXPECT_EQ(manager->getAlarms().size(), 0);
}

TEST(TimerControllerTest, AddEditStartPauseAndDeleteViaViewSignals) {
    TimerWindow w;
    TimerManager *manager = w.getManager();
    ASSERT_TRUE(manager);

    emit w.addTimerRequested("T1", 30, "Normal", "Default");
    ASSERT_EQ(manager->getTimers().size(), 1);

    emit w.startPauseRequested(QList<int>{0});
    EXPECT_TRUE(manager->getTimers()[0].running);

    emit w.startPauseRequested(QList<int>{0});
    EXPECT_FALSE(manager->getTimers()[0].running);

    emit w.editTimerRequested(0, "T2", 20, "Reminder", "Work");
    EXPECT_EQ(manager->getTimers()[0].name, "T2");
    EXPECT_EQ(manager->getTimers()[0].type, "Reminder");

    emit w.deleteTimersRequested(QList<int>{0});
    EXPECT_EQ(manager->getTimers().size(), 0);
}

TEST(ClockControllerTest, AddRemoveAndToggleFormatViaViewSignals) {
    ClockWindow w;
    ClockModel *model = w.findChild<ClockModel*>();
    ASSERT_TRUE(model);

    emit w.addClockRequested("UTC");
    ASSERT_EQ(model->clocks().size(), 1);

    emit w.formatToggled(true);
    EXPECT_TRUE(model->format12h());

    emit w.removeClocksRequested(QList<int>{0});
    EXPECT_EQ(model->clocks().size(), 0);
}

TEST(StopwatchControllerTest, StartStopAndLapViaViewSignals) {
    StopwatchWindow w;
    StopwatchModel *model = w.findChild<StopwatchModel*>();
    ASSERT_TRUE(model);

    emit w.startStopRequested();
    EXPECT_TRUE(model->isRunning());

    emit w.lapRequested();
    EXPECT_EQ(model->lapDurations().size(), 1);

    emit w.startStopRequested();
    EXPECT_FALSE(model->isRunning());

    emit w.lapRequested();
    EXPECT_TRUE(model->lapDurations().isEmpty());
}
