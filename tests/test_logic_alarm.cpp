/**
 * @file test_logic_alarm.cpp
 * @brief Definitions for test_logic_alarm.
 * @details Implements logic declared in the corresponding header for test_logic_alarm.
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
#include <QSignalSpy>
#include <QMetaObject>
#include "../alarm/alarmmanager.h"
#include "../alarm/jsonalarmstorage.h"

static AlarmData makeAlarm(const QString& name,
                           const QTime& t,
                           RepeatMode repeat = RepeatMode::Never,
                           const QStringList& days = {},
                           bool enabled = true,
                           bool snooze = false,
                           const QString& sound = {}) {
    AlarmData a;
    a.name = name;
    a.time = t;
    a.repeatMode = repeat;
    a.days = days;
    a.enabled = enabled;
    a.snooze = snooze;
    a.soundPath = sound;
    return a;
}

TEST(AlarmManagerLogicTest, AddRemoveToggleBasics) {
    AlarmManager m;
    m.addAlarm(makeAlarm("A", QTime(6,0)));
    m.addAlarm(makeAlarm("B", QTime(7,0)));
    EXPECT_EQ(m.getAlarms().size(), 2);

    m.removeAlarm(0);
    EXPECT_EQ(m.getAlarms().size(), 1);

    bool before = m.getAlarms().first().enabled;
    m.toggleAlarm(0);
    bool after = m.getAlarms().first().enabled;
    EXPECT_NE(before, after);
}

TEST(AlarmManagerLogicTest, ToggleAlarmInvalidIndexDoesNothing) {
    AlarmManager m;
    m.addAlarm(makeAlarm("A", QTime(6,0)));
    int count = m.getAlarms().size();
    m.toggleAlarm(5);
    EXPECT_EQ(m.getAlarms().size(), count);
}

TEST(AlarmManagerLogicTest, ToggleAlarmEnablesAndComputesTrigger) {
    AlarmManager m;
    AlarmData a = makeAlarm("Toggle", QTime::currentTime().addSecs(3600), RepeatMode::Never, {}, false);
    a.nextTrigger = QDateTime();
    m.addAlarm(a);

    m.toggleAlarm(0);
    auto after = m.getAlarms().first();
    EXPECT_TRUE(after.enabled);
    EXPECT_TRUE(after.nextTrigger.isValid());
    EXPECT_GE(after.nextTrigger.date(), QDate::currentDate());
}

TEST(AlarmManagerLogicTest, RemoveAlarmInvalidIndexDoesNothing) {
    AlarmManager m;
    m.addAlarm(makeAlarm("A", QTime(6,0)));
    int count = m.getAlarms().size();
    m.removeAlarm(-1);
    EXPECT_EQ(m.getAlarms().size(), count);
}

TEST(AlarmManagerLogicTest, AddAlarmComputesTriggerWhenInvalid) {
    AlarmManager m;
    AlarmData a = makeAlarm("Invalid", QTime::currentTime().addSecs(60), RepeatMode::EveryDay);
    a.nextTrigger = QDateTime();
    m.addAlarm(a);

    auto loaded = m.getAlarms().first();
    EXPECT_TRUE(loaded.nextTrigger.isValid());
}

TEST(AlarmManagerLogicTest, SnoozeUpdatesTriggerAndEnables) {
    AlarmManager m;
    AlarmData a = makeAlarm("Snooze", QTime::currentTime().addSecs(3600), RepeatMode::Never, {}, false);
    m.addAlarm(a);

    const int minutes = 2;
    const QDateTime now = QDateTime::currentDateTime();
    m.snoozeAlarm(m.getAlarms().first(), minutes);

    auto updated = m.getAlarms().first();
    EXPECT_TRUE(updated.enabled);
    EXPECT_TRUE(updated.nextTrigger.isValid());
    const int diff = now.secsTo(updated.nextTrigger);
    EXPECT_GE(diff, minutes * 60 - 2);
    EXPECT_LE(diff, minutes * 60 + 2);
}

TEST(AlarmManagerLogicTest, ComputeInitialTriggerIsFutureOrNextDay) {
    AlarmManager m;
    QTime pastTime = QTime::currentTime().addSecs(-60);
    m.addAlarm(makeAlarm("Past", pastTime));

    const auto alarms = m.getAlarms();
    ASSERT_FALSE(alarms.isEmpty());
    EXPECT_TRUE(alarms.first().nextTrigger.isValid());
    EXPECT_GE(alarms.first().nextTrigger.date(), QDate::currentDate());
}

TEST(AlarmManagerLogicTest, ComputeNextTriggerForRepeatModes) {
    AlarmManager m;

    AlarmData aED = makeAlarm("ED", QTime(10,0), RepeatMode::EveryDay);
    m.addAlarm(aED);
    auto ed = m.getAlarms().last();
    EXPECT_TRUE(ed.nextTrigger.isValid());

    AlarmData aWD = makeAlarm("WD", QTime(11,0), RepeatMode::Weekdays);
    m.addAlarm(aWD);
    auto wd = m.getAlarms().last();
    EXPECT_LE(wd.nextTrigger.date().dayOfWeek(), 5);

    AlarmData aWE = makeAlarm("WE", QTime(12,0), RepeatMode::Weekends);
    m.addAlarm(aWE);
    auto we = m.getAlarms().last();
    EXPECT_GE(we.nextTrigger.date().dayOfWeek(), 6);

    AlarmData aSD = makeAlarm("SD", QTime(9,30), RepeatMode::SpecificDays, {"Wed"});
    m.addAlarm(aSD);
    auto sd = m.getAlarms().last();
    EXPECT_EQ(sd.nextTrigger.date().dayOfWeek(), 3);
}

TEST(AlarmManagerLogicTest, ComputeNextTriggerFallbackWhenSpecificDaysEmpty) {
    AlarmManager m;
    AlarmData a = makeAlarm("Empty", QTime(9, 0), RepeatMode::SpecificDays, {});
    const QDateTime now = QDateTime::currentDateTime();
    const QDateTime next = QDateTime(now.date().addDays(1), a.time);
    a.nextTrigger = QDateTime();

    m.addAlarm(a);
    auto stored = m.getAlarms().first();
    EXPECT_TRUE(stored.nextTrigger.isValid());
    EXPECT_EQ(stored.nextTrigger.date(), next.date());
    EXPECT_EQ(stored.nextTrigger.time(), next.time());
}

TEST(AlarmManagerLogicTest, SaveAndLoadRoundTrip) {
    AlarmManager m;
    AlarmData a = makeAlarm("Trip", QTime(5,45), RepeatMode::SpecificDays, {"Mon","Fri"}, true, true, "C:/beep.wav");
    a.nextTrigger = QDateTime::currentDateTime().addDays(2);
    m.addAlarm(a);

    QTemporaryDir dir;
    const QString path = dir.path() + "/alarms.json";
    m.saveToFile(path);

    AlarmManager m2;
    m2.loadFromFile(path);
    ASSERT_EQ(m2.getAlarms().size(), 1);
    const auto r = m2.getAlarms().first();
    EXPECT_EQ(r.name, "Trip");
    EXPECT_EQ(r.repeatMode, RepeatMode::SpecificDays);
    EXPECT_TRUE(r.days.contains("Mon"));
    EXPECT_TRUE(r.days.contains("Fri"));
    EXPECT_EQ(r.soundPath, "C:/beep.wav");
    EXPECT_TRUE(r.enabled);
    EXPECT_TRUE(r.nextTrigger.isValid());
}

TEST(AlarmManagerLogicTest, LoadFromMissingFileDoesNotClearExisting) {
    AlarmManager m;
    m.addAlarm(makeAlarm("Keep", QTime(6,0)));
    const int before = m.getAlarms().size();

    m.loadFromFile("Z:/path/does/not/exist.json");
    EXPECT_EQ(m.getAlarms().size(), before);
}

TEST(AlarmManagerLogicTest, LoadFromInvalidJsonLeavesAlarmsEmpty) {
    QTemporaryDir dir;
    const QString path = dir.path() + "/bad.json";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write("{not an array}");
    f.close();

    AlarmManager m;
    m.loadFromFile(path);
    EXPECT_TRUE(m.getAlarms().isEmpty());
}

TEST(AlarmManagerLogicTest, LoadFromFileInvalidNextTriggerComputesInitial) {
    QTemporaryDir dir;
    const QString path = dir.path() + "/alarms.json";

    QJsonObject obj;
    obj["name"] = "Alarm";
    obj["time"] = QTime(9, 0).toString("HH:mm:ss");
    obj["repeatMode"] = "Never";
    obj["days"] = QJsonArray();
    obj["soundPath"] = "";
    obj["snooze"] = false;
    obj["enabled"] = true;
    obj["nextTrigger"] = "";

    QJsonArray arr;
    arr.append(obj);
    QJsonDocument doc(arr);
    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write(doc.toJson(QJsonDocument::Compact));
    f.close();

    AlarmManager m;
    m.loadFromFile(path);
    ASSERT_EQ(m.getAlarms().size(), 1);
    EXPECT_TRUE(m.getAlarms().first().nextTrigger.isValid());
}

TEST(AlarmManagerLogicTest, CheckAlarmsTriggersAndDisablesOneTime) {
    AlarmManager m;
    AlarmData a = makeAlarm("Once", QTime::currentTime(), RepeatMode::Never);
    a.nextTrigger = QDateTime::currentDateTime().addSecs(-1);
    m.addAlarm(a);

    QSignalSpy spy(&m, &AlarmManager::alarmTriggered);
    EXPECT_TRUE(QMetaObject::invokeMethod(&m, "checkAlarms", Qt::DirectConnection));

    EXPECT_EQ(spy.count(), 1);
    auto updated = m.getAlarms().first();
    EXPECT_FALSE(updated.enabled);
}

TEST(AlarmManagerLogicTest, CheckAlarmsReschedulesRepeating) {
    AlarmManager m;
    AlarmData a = makeAlarm("Daily", QTime::currentTime(), RepeatMode::EveryDay);
    a.nextTrigger = QDateTime::currentDateTime().addSecs(-1);
    m.addAlarm(a);

    QSignalSpy spy(&m, &AlarmManager::alarmTriggered);
    EXPECT_TRUE(QMetaObject::invokeMethod(&m, "checkAlarms", Qt::DirectConnection));

    EXPECT_EQ(spy.count(), 1);
    auto updated = m.getAlarms().first();
    EXPECT_TRUE(updated.enabled);
    EXPECT_TRUE(updated.nextTrigger.isValid());
    EXPECT_GT(updated.nextTrigger, QDateTime::currentDateTime().addSecs(-1));
}

TEST(AlarmManagerLogicTest, CheckAlarmsSkipsDisabled) {
    AlarmManager m;
    AlarmData a = makeAlarm("Disabled", QTime::currentTime(), RepeatMode::Never, {}, false);
    a.nextTrigger = QDateTime::currentDateTime().addSecs(-1);
    m.addAlarm(a);

    QSignalSpy spy(&m, &AlarmManager::alarmTriggered);
    EXPECT_TRUE(QMetaObject::invokeMethod(&m, "checkAlarms", Qt::DirectConnection));
    EXPECT_EQ(spy.count(), 0);
}

TEST(AlarmRepeatModeTest, FromStringParsesCommonValues) {
    EXPECT_EQ(repeatModeFromString("Never"), RepeatMode::Never);
    EXPECT_EQ(repeatModeFromString("once"), RepeatMode::Once);
    EXPECT_EQ(repeatModeFromString("Every day"), RepeatMode::EveryDay);
    EXPECT_EQ(repeatModeFromString("Weekdays"), RepeatMode::Weekdays);
    EXPECT_EQ(repeatModeFromString("Weekends"), RepeatMode::Weekends);
    EXPECT_EQ(repeatModeFromString("Specific Days"), RepeatMode::SpecificDays);
}

TEST(AlarmRepeatModeTest, FromStringHandlesWhitespaceAndUnknown) {
    EXPECT_EQ(repeatModeFromString("  Weekdays "), RepeatMode::Weekdays);
    EXPECT_EQ(repeatModeFromString("unknown"), RepeatMode::Never);
    EXPECT_EQ(repeatModeFromString(""), RepeatMode::Never);
}

TEST(AlarmRepeatModeTest, ToStringProducesStableLabels) {
    EXPECT_EQ(repeatModeToString(RepeatMode::Never), "Never");
    EXPECT_EQ(repeatModeToString(RepeatMode::Once), "Once");
    EXPECT_EQ(repeatModeToString(RepeatMode::EveryDay), "Every day");
    EXPECT_EQ(repeatModeToString(RepeatMode::Weekdays), "Weekdays");
    EXPECT_EQ(repeatModeToString(RepeatMode::Weekends), "Weekends");
    EXPECT_EQ(repeatModeToString(RepeatMode::SpecificDays), "Specific days");
}

TEST(JsonAlarmStorageTest, LoadMissingFileReturnsFalse) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/missing.json";

    JsonAlarmStorage storage(path);
    QList<AlarmData> out;
    EXPECT_FALSE(storage.load(out));
}

TEST(JsonAlarmStorageTest, LoadInvalidJsonReturnsTrueAndClears) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/bad.json";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("{not an array}");
    f.close();

    JsonAlarmStorage storage(path);
    QList<AlarmData> out;
    EXPECT_TRUE(storage.load(out));
    EXPECT_TRUE(out.isEmpty());
}
TEST(AlarmManagerLogicTest, SaveAndLoadWithNullStorageReturnsFalse) {
    AlarmManager m;
    m.setStorage(nullptr);

    EXPECT_FALSE(m.save());
    EXPECT_FALSE(m.load());
}

TEST(AlarmManagerLogicTest, SnoozeNonExistentAlarmDoesNothing) {
    AlarmManager m;
    AlarmData a1 = makeAlarm("A", QTime(6, 0));
    m.addAlarm(a1);

    AlarmData notAdded = makeAlarm("B", QTime(7, 0));

    m.snoozeAlarm(notAdded, 5);

    ASSERT_EQ(m.getAlarms().size(), 1);
    EXPECT_EQ(m.getAlarms().first().name, "A");
}

TEST(JsonAlarmStorageTest, SaveAndLoadValidData) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/good_alarms.json";

    JsonAlarmStorage storage(path);
    QList<AlarmData> alarmsToSave;
    alarmsToSave.append(makeAlarm("Test", QTime(14, 30), RepeatMode::EveryDay));

    EXPECT_TRUE(storage.save(alarmsToSave));

    QList<AlarmData> loadedAlarms;
    EXPECT_TRUE(storage.load(loadedAlarms));

    ASSERT_EQ(loadedAlarms.size(), 1);
    EXPECT_EQ(loadedAlarms.first().name, "Test");
    EXPECT_EQ(loadedAlarms.first().time, QTime(14, 30));
    EXPECT_EQ(loadedAlarms.first().repeatMode, RepeatMode::EveryDay);
}

TEST(AlarmRepeatModeTest, OnceConversions) {
    EXPECT_EQ(repeatModeFromString("Once"), RepeatMode::Once);
    EXPECT_EQ(repeatModeToString(RepeatMode::Once), "Once");
}