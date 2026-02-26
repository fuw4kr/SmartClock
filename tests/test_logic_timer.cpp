/**
 * @file test_logic_timer.cpp
 * @brief Definitions for test_logic_timer.
 * @details Implements logic declared in the corresponding header for test_logic_timer.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include <gtest/gtest.h>
#include <QDateTime>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../timer/timermanager.h"
#include "../timer/jsontimerstorage.h"

class TimerManagerLogicTest : public ::testing::Test {
protected:
    void SetUp() override { manager = new TimerManager(); }
    void TearDown() override { delete manager; }
    TimerManager* manager;
};

TEST_F(TimerManagerLogicTest, AddTimerIncreasesCount) {
    int before = manager->getTimers().size();
    manager->addTimer("TestTimer", 60);
    EXPECT_EQ(manager->getTimers().size(), before + 1);
}

TEST_F(TimerManagerLogicTest, AddedTimerHasCorrectData) {
    manager->addTimer("Pomodoro", 1500, "Normal", "Work");
    const auto& t = manager->getTimers().last();
    EXPECT_EQ(t.name, "Pomodoro");
    EXPECT_EQ(t.duration, 1500);
    EXPECT_EQ(t.remaining, 1500);
    EXPECT_EQ(t.groupName, "Work");
    EXPECT_FALSE(t.running);
    EXPECT_EQ(t.status, TimerStatus::Paused);
}

TEST_F(TimerManagerLogicTest, AddTimerEmptyGroupDefaultsToDefault) {
    manager->addTimer("NoGroup", 10, "Normal", "");
    const auto& t = manager->getTimers().last();
    EXPECT_EQ(t.groupName, "Default");
}

TEST_F(TimerManagerLogicTest, StartAndPauseChangesState) {
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

TEST_F(TimerManagerLogicTest, PauseTimerWhenAlreadyPausedDoesNothing) {
    manager->addTimer("Paused", 5);
    const int before = manager->getTimers()[0].remaining;
    manager->pauseTimer(0);
    const auto& t = manager->getTimers()[0];
    EXPECT_FALSE(t.running);
    EXPECT_EQ(t.status, TimerStatus::Paused);
    EXPECT_EQ(t.remaining, before);
}

TEST_F(TimerManagerLogicTest, EditTimerUpdatesValues) {
    manager->addTimer("Old", 10);
    manager->editTimer(0, "New", 20, "Reminder", "Study");
    const auto& t = manager->getTimers()[0];
    EXPECT_EQ(t.name, "New");
    EXPECT_EQ(t.duration, 20);
    EXPECT_EQ(t.type, "Reminder");
    EXPECT_EQ(t.groupName, "Study");
}

TEST_F(TimerManagerLogicTest, EditTimerEmptyGroupDefaultsToDefault) {
    manager->addTimer("Old", 10);
    manager->editTimer(0, "New", 20, "Normal", "");
    const auto& t = manager->getTimers()[0];
    EXPECT_EQ(t.groupName, "Default");
}

TEST_F(TimerManagerLogicTest, RemoveTimerReducesCount) {
    manager->addTimer("Del", 10);
    int before = manager->getTimers().size();
    manager->removeTimer(0);
    EXPECT_EQ(manager->getTimers().size(), before - 1);
}

TEST_F(TimerManagerLogicTest, RecommendationSetAndRemove) {
    manager->setRecommendation("A", "B");
    EXPECT_EQ(manager->getRecommendation("A"), "B");
    manager->removeRecommendation("A");
    EXPECT_TRUE(manager->getRecommendation("A").isEmpty());
}

TEST_F(TimerManagerLogicTest, GetNextTimerFindsShortestRunning) {
    manager->addTimer("Long", 100);
    manager->addTimer("Short", 5);
    manager->startTimer(0);
    manager->startTimer(1);
    EXPECT_EQ(manager->getNextTimer().name, "Short");
}

TEST_F(TimerManagerLogicTest, HasTimerReturnsFalseWhenMissing) {
    manager->addTimer("Exists", 10);
    EXPECT_FALSE(manager->hasTimer("Missing"));
}

TEST_F(TimerManagerLogicTest, GetFilteredAndGroupedTimers) {
    manager->addTimer("A", 10, "Normal", "Work");
    manager->addTimer("B", 5, "Normal", "Play");
    manager->startTimer(1);
    manager->pauseTimer(1);

    auto all = manager->getFilteredTimers("All timers");
    auto paused = manager->getFilteredTimers("Paused");
    auto group = manager->getGroupTimers("Work");

    EXPECT_GE(all.size(), 2);
    EXPECT_FALSE(paused.isEmpty());
    EXPECT_EQ(group[0].groupName, "Work");
}

TEST_F(TimerManagerLogicTest, StartGroupStartsMatchingTimers) {
    manager->addTimer("A", 10, "Normal", "Work");
    manager->addTimer("B", 10, "Normal", "Work");
    manager->addTimer("C", 10, "Normal", "Rest");

    manager->startGroup("Work");
    EXPECT_TRUE(manager->getTimers()[0].running);
    EXPECT_TRUE(manager->getTimers()[1].running);
    EXPECT_FALSE(manager->getTimers()[2].running);
}

TEST_F(TimerManagerLogicTest, DeletedTimersAddAndClear) {
    TimerData t;
    t.name = "Old";
    t.duration = 10;
    manager->addDeletedTimer(t);
    EXPECT_EQ(manager->getDeletedTimers().size(), 1);
    manager->clearDeletedTimers();
    EXPECT_TRUE(manager->getDeletedTimers().isEmpty());
}

TEST(TimerManagerLogicExtraTest, SaveAndLoadRoundTrip) {
    TimerManager m;
    m.addTimer("Study", 120);
    m.addTimer("Break", 300, "Melody", "Rest");
    m.setRecommendation("Study", "Break");

    QTemporaryDir dir;
    QString path = dir.path() + "/test_timers.json";
    m.saveToFile(path);

    TimerManager m2;
    m2.loadFromFile(path);

    ASSERT_EQ(m2.getTimers().size(), 2);
    EXPECT_EQ(m2.getTimers()[0].name, "Study");
    EXPECT_EQ(m2.getTimers()[1].type, "Melody");
    EXPECT_EQ(m2.getRecommendation("Study"), "Break");
}

TEST(TimerManagerLogicExtraTest, LoadDefaultsTypeAndGroupAndFinishedStatus) {
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

TEST(TimerManagerLogicExtraTest, StartTimerResetsRemainingWhenFinished) {
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

TEST(JsonTimerStorageTest, LoadMissingFileReturnsFalse) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/missing.json";

    JsonTimerStorage storage(path);
    TimerSnapshot out;
    EXPECT_FALSE(storage.load(out));
}

TEST(JsonTimerStorageTest, LoadInvalidJsonReturnsTrueAndClears) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/bad.json";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("{not valid json");
    f.close();

    JsonTimerStorage storage(path);
    TimerSnapshot out;
    EXPECT_TRUE(storage.load(out));
    EXPECT_TRUE(out.timers.isEmpty());
    EXPECT_TRUE(out.deletedTimers.isEmpty());
    EXPECT_TRUE(out.recommendations.isEmpty());
}
