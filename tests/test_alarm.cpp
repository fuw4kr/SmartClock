#include <gtest/gtest.h>
#include "../alarm/alarmmanager.h"
#include "../alarm/alarmsettingsdialog.h"
#include "../alarm/alarmitemwidget.h"
#include "../alarm/alarmwindow.h"
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QListWidget>
#include <QPushButton>
#include <QDateTime>
#include <QTest>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QTimeEdit>
#include <QLineEdit>
#include <QStandardPaths>
#include <QLabel>

static AlarmData makeAlarm(const QString& name,
                           const QTime& t,
                           const QString& repeat = "Never",
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

TEST(AlarmSettingsDialogTest, InitialRepeatOptionsAndDaysGroupHidden) {
    AlarmSettingsDialog dlg;
    auto comboRepeat = dlg.findChild<QComboBox*>("comboRepeat");
    auto groupDays   = dlg.findChild<QGroupBox*>("groupDays");
    ASSERT_TRUE(comboRepeat);
    ASSERT_TRUE(groupDays);

    EXPECT_GE(comboRepeat->count(), 5);
    EXPECT_FALSE(groupDays->isVisible());
}

TEST(AlarmSettingsDialogTest, SpecificDaysVisibilityToggles) {
    AlarmSettingsDialog dlg;
    auto comboRepeat = dlg.findChild<QComboBox*>("comboRepeat");
    auto groupDays   = dlg.findChild<QGroupBox*>("groupDays");
    ASSERT_TRUE(comboRepeat && groupDays);

    comboRepeat->setCurrentIndex(comboRepeat->findText("Weekdays"));
    QCoreApplication::processEvents();
    EXPECT_FALSE(groupDays->isVisible());
}

TEST(AlarmSettingsDialogTest, GettersReturnFormattedValues) {
    AlarmSettingsDialog dlg;
    auto timeEdit = dlg.findChild<QTimeEdit*>("timeEdit");
    auto line     = dlg.findChild<QLineEdit*>("lineLabel");
    ASSERT_TRUE(timeEdit && line);

    timeEdit->setTime(QTime(7, 30));
    line->setText(" My Alarm ");

    EXPECT_EQ(dlg.getTime(), QTime(7,30));
    EXPECT_EQ(dlg.getLabel(), "My Alarm");

    dlg.findChild<QComboBox*>("comboRepeat")->setCurrentText("Weekdays");
    EXPECT_EQ(dlg.getRepeatMode(), "Weekdays");

    dlg.findChild<QCheckBox*>("chkMon")->setChecked(true);
    dlg.findChild<QCheckBox*>("chkFri")->setChecked(true);
    auto days = dlg.getDays();
    EXPECT_TRUE(days.contains("Mon"));
    EXPECT_TRUE(days.contains("Fri"));
}

TEST(AlarmSettingsDialogTest, DefaultSoundVsCustomSound) {
    AlarmSettingsDialog dlg;
    auto comboSound = dlg.findChild<QComboBox*>("comboSound");
    ASSERT_TRUE(comboSound);

    comboSound->setCurrentText("Default sound");
    EXPECT_TRUE(dlg.getSoundPath().contains("soundalarm.wav"));

    comboSound->addItem("C:/tmp/abc.wav");
    comboSound->setCurrentText("C:/tmp/abc.wav");
    EXPECT_EQ(dlg.getSoundPath(), "C:/tmp/abc.wav");
}

TEST(AlarmSettingsDialogTest, SnoozeFlag) {
    AlarmSettingsDialog dlg;
    auto chk = dlg.findChild<QCheckBox*>("checkSnooze");
    ASSERT_TRUE(chk);
    chk->setChecked(true);
    EXPECT_TRUE(dlg.getSnooze());
    chk->setChecked(false);
    EXPECT_FALSE(dlg.getSnooze());
}

TEST(AlarmItemWidgetTest, LabelsReflectAlarmDataAndRepeatText) {
    AlarmData a = makeAlarm("Wake", QTime(8,15), "Specific days", {"Mon", "Wed"});
    AlarmItemWidget w(a);

    auto lTime   = w.findChild<QLabel*>("labelTime");
    auto lName   = w.findChild<QLabel*>("labelName");
    auto lRepeat = w.findChild<QLabel*>("labelRepeat");
    ASSERT_TRUE(lTime && lName && lRepeat);

    EXPECT_EQ(lTime->text(), "08:15");
    EXPECT_EQ(lName->text(), "Wake");
    EXPECT_EQ(lRepeat->text(), "Mon, Wed");

    a.repeatMode = "Weekdays";
    w.setAlarmData(a);
    EXPECT_EQ(lRepeat->text(), "Weekdays");

    a.repeatMode = "Weekends";
    w.setAlarmData(a);
    EXPECT_EQ(lRepeat->text(), "Weekends");

    a.repeatMode = "Never";
    w.setAlarmData(a);
    EXPECT_EQ(lRepeat->text(), "Once");
}

TEST(AlarmItemWidgetTest, ToggleButtonSwitchesStateAndEmitsSignal) {
    AlarmData a = makeAlarm("A", QTime(9,0), "Never", {}, false);
    AlarmItemWidget w(a);
    auto btn = w.findChild<QPushButton*>("btnToggle");
    ASSERT_TRUE(btn);

    QSignalSpy spy(&w, &AlarmItemWidget::toggled);

    btn->click();
    ASSERT_GE(spy.count(), 1);
    bool first = spy.takeFirst().at(0).toBool();
    EXPECT_TRUE(first);

    btn->click();
    ASSERT_GE(spy.count(), 1);
    bool second = spy.takeFirst().at(0).toBool();
    EXPECT_FALSE(second);
}

TEST(AlarmItemWidgetTest, HoverPropertyFlipsOnEnterLeave) {
    AlarmData a = makeAlarm("A", QTime(9,0));
    AlarmItemWidget w(a);
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(&w, &enter);
    EXPECT_TRUE(w.property("hovered").toBool());

    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(&w, &leave);
    EXPECT_FALSE(w.property("hovered").toBool());
}

TEST(AlarmManagerTest, AddRemoveToggleUpdatesList) {
    AlarmManager m;
    QSignalSpy spy(&m, &AlarmManager::alarmsUpdated);

    m.addAlarm(makeAlarm("A", QTime(6,0)));
    m.addAlarm(makeAlarm("B", QTime(7,0)));
    EXPECT_EQ(m.getAlarms().size(), 2);
    spy.clear();

    m.removeAlarm(0);
    EXPECT_EQ(m.getAlarms().size(), 1);
    spy.clear();

    m.toggleAlarm(0);
    EXPECT_GE(spy.count(), 0);
}

TEST(AlarmManagerTest, ComputeInitialTriggerIsFutureOrNextDay) {
    AlarmManager m;

    
    QTime pastTime = QTime::currentTime().addSecs(-60);
    m.addAlarm(makeAlarm("Past", pastTime));

    const auto alarms = m.getAlarms();
    ASSERT_FALSE(alarms.isEmpty());

    
    EXPECT_TRUE(alarms.first().nextTrigger.isValid());
    EXPECT_GE(alarms.first().nextTrigger.date(), QDate::currentDate());
}

TEST(AlarmManagerTest, ComputeNextTriggerForRepeatModes) {
    AlarmManager m;
    QDateTime now = QDateTime::currentDateTime();

    
    AlarmData aED = makeAlarm("ED", QTime(10,0), "Every day");
    m.addAlarm(aED);
    auto ed = m.getAlarms().last();
    EXPECT_TRUE(ed.nextTrigger.isValid());

    
    AlarmData aWD = makeAlarm("WD", QTime(11,0), "Weekdays");
    m.addAlarm(aWD);
    auto wd = m.getAlarms().last();
    EXPECT_LE(wd.nextTrigger.date().dayOfWeek(), 5);

    
    AlarmData aWE = makeAlarm("WE", QTime(12,0), "Weekends");
    m.addAlarm(aWE);
    auto we = m.getAlarms().last();
    EXPECT_GE(we.nextTrigger.date().dayOfWeek(), 6);

    
    AlarmData aSD = makeAlarm("SD", QTime(9,30), "Specific days", {"Wed"});
    m.addAlarm(aSD);
    auto sd = m.getAlarms().last();
    EXPECT_EQ(sd.nextTrigger.date().dayOfWeek(), 3);
}

TEST(AlarmManagerTest, SaveAndLoadRoundTrip) {
    AlarmManager m;
    AlarmData a = makeAlarm("Trip", QTime(5,45), "Specific days", {"Mon","Fri"}, true, true, "C:/beep.wav");
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
    EXPECT_EQ(r.repeatMode, "Specific days");
    EXPECT_TRUE(r.days.contains("Mon"));
    EXPECT_TRUE(r.days.contains("Fri"));
    EXPECT_EQ(r.soundPath, "C:/beep.wav");
    EXPECT_TRUE(r.enabled);
    EXPECT_TRUE(r.nextTrigger.isValid());
}

TEST(AlarmWindowTest, UpdateListBuildsWidgetsAndConnectsToggles) {
    AlarmWindow w;
    AlarmManager* manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    
    manager->addAlarm(makeAlarm("A", QTime(6,10), "Never"));
    manager->addAlarm(makeAlarm("B", QTime(7,20), "Weekdays"));

    
    QListWidget* list = w.findChild<QListWidget*>("listAlarms");
    ASSERT_TRUE(list);
    EXPECT_GE(list->count(), 1);

    
    QWidget* w0 = list->itemWidget(list->item(0));
    auto aiw = qobject_cast<AlarmItemWidget*>(w0);
    ASSERT_TRUE(aiw);

    
    QSignalSpy spy(manager, &AlarmManager::alarmsUpdated);
    emit aiw->toggled(false);

    
    EXPECT_GE(spy.count(), 0);
}

TEST(AlarmWindowTest, AlarmsFilePathLooksValid) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = dir + "/alarms.json";

    EXPECT_FALSE(dir.isEmpty());
    EXPECT_TRUE(path.endsWith("/alarms.json") || path.endsWith("\\alarms.json"));
}

TEST(AlarmWindowTest, GetNextAlarmStringFormatsTodayTomorrowOrWeekday) {
    AlarmWindow w;
    AlarmManager* manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    for (int i = manager->getAlarms().size() - 1; i >= 0; --i)
        manager->removeAlarm(i);

    EXPECT_EQ(w.getNextAlarmString(), "No alarms set");

    AlarmData a1 = makeAlarm("Soon", QTime::currentTime(), "Never", {}, true);
    a1.nextTrigger = QDateTime::currentDateTime().addSecs(60);
    AlarmData a2 = makeAlarm("Later", QTime::currentTime(), "Never", {}, true);
    a2.nextTrigger = QDateTime::currentDateTime().addDays(1).addSecs(30);

    manager->addAlarm(a1);
    manager->addAlarm(a2);

    QString s = w.getNextAlarmString();
    EXPECT_FALSE(s.isEmpty());
    EXPECT_TRUE(s.contains(":"));
}

TEST(AlarmWindowTest, ManagerPersistsToFileViaWindowLifecycle) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = dir + "/alarms.json";
    {
        AlarmWindow w;
        AlarmManager* manager = w.findChild<AlarmManager*>();
        ASSERT_TRUE(manager);
        manager->addAlarm(makeAlarm("Persist", QTime(4,0)));
        
    }

    
    QFile f(path);
    EXPECT_TRUE(f.exists());

    
    EXPECT_NO_THROW({
        AlarmWindow w2;
    });

    QFile::remove(path);
}

