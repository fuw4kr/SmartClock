#include <gtest/gtest.h>
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
#include <QApplication>
#include <QDateTime>
#include <QTest>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QCoreApplication>
#include <QTimeEdit>
#include <QLineEdit>
#include <QStandardPaths>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include "../alarm/alarmmanager.h"
#include "../alarm/alarmsettingsdialog.h"
#include "../alarm/alarmitemwidget.h"
#include "../alarm/alarmwindow.h"
#include "../alarm/alarmfactory.h"
#include "../alarm/soundalarmaction.h"

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
class MockAlarmAction : public IAlarmAction {
public:
    bool executed = false;
    void execute() override {
        executed = true;
    }
    ~MockAlarmAction() override = default;
};

TEST(IAlarmActionTest, PolymorphismAndExecutionWorks) {
    std::unique_ptr<IAlarmAction> action = std::make_unique<MockAlarmAction>();
    action->execute();

    auto* mock = static_cast<MockAlarmAction*>(action.get());
    EXPECT_TRUE(mock->executed);
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
    EXPECT_TRUE(groupDays->isHidden());
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
    EXPECT_EQ(dlg.getRepeatMode(), RepeatMode::Weekdays);

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

TEST(AlarmSettingsDialogTest, SpecificDaysShowsGroupAndEmptyLabelDefaults) {
    AlarmSettingsDialog dlg;
    auto comboRepeat = dlg.findChild<QComboBox*>("comboRepeat");
    auto groupDays   = dlg.findChild<QGroupBox*>("groupDays");
    auto line         = dlg.findChild<QLineEdit*>("lineLabel");
    ASSERT_TRUE(comboRepeat && groupDays && line);

    comboRepeat->setCurrentText("Specific Days");
    QCoreApplication::processEvents();
    EXPECT_FALSE(groupDays->isHidden());

    line->clear();
    EXPECT_EQ(dlg.getLabel(), "Alarm");
}

TEST(AlarmSettingsDialogTest, SpecificDaysThenNeverHidesGroup) {
    AlarmSettingsDialog dlg;
    auto comboRepeat = dlg.findChild<QComboBox*>("comboRepeat");
    auto groupDays   = dlg.findChild<QGroupBox*>("groupDays");
    ASSERT_TRUE(comboRepeat && groupDays);

    comboRepeat->setCurrentText("Specific Days");
    QCoreApplication::processEvents();
    EXPECT_FALSE(groupDays->isHidden());

    comboRepeat->setCurrentText("Never");
    QCoreApplication::processEvents();
    EXPECT_TRUE(groupDays->isHidden());
}

TEST(AlarmSettingsDialogTest, RepeatModeReturnsCurrentEnum) {
    AlarmSettingsDialog dlg;
    auto comboRepeat = dlg.findChild<QComboBox*>("comboRepeat");
    ASSERT_TRUE(comboRepeat);

    comboRepeat->setCurrentText("Specific Days");
    EXPECT_EQ(dlg.getRepeatMode(), RepeatMode::SpecificDays);
}

TEST(AlarmSettingsDialogTest, DefaultDaysEmptyAndSoundDefaultPath) {
    AlarmSettingsDialog dlg;
    auto comboSound = dlg.findChild<QComboBox*>("comboSound");
    ASSERT_TRUE(comboSound);

    comboSound->setCurrentText("Default sound");
    EXPECT_TRUE(dlg.getDays().isEmpty());
    EXPECT_TRUE(dlg.getSoundPath().contains("soundalarm.wav"));
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
    AlarmData a = makeAlarm("Wake", QTime(8,15), RepeatMode::SpecificDays, {"Mon", "Wed"});
    AlarmItemWidget w(a);

    auto lTime   = w.findChild<QLabel*>("labelTime");
    auto lName   = w.findChild<QLabel*>("labelName");
    auto lRepeat = w.findChild<QLabel*>("labelRepeat");
    ASSERT_TRUE(lTime && lName && lRepeat);

    EXPECT_EQ(lTime->text(), "08:15");
    EXPECT_EQ(lName->text(), "Wake");
    EXPECT_EQ(lRepeat->text(), "Mon, Wed");

    a.repeatMode = RepeatMode::Weekdays;
    w.setAlarmData(a);
    EXPECT_EQ(lRepeat->text(), "Weekdays");

    a.repeatMode = RepeatMode::Weekends;
    w.setAlarmData(a);
    EXPECT_EQ(lRepeat->text(), "Weekends");

    a.repeatMode = RepeatMode::Never;
    w.setAlarmData(a);
    EXPECT_EQ(lRepeat->text(), "Once");
}

TEST(AlarmItemWidgetTest, ToggleButtonSwitchesStateAndEmitsSignal) {
    AlarmData a = makeAlarm("A", QTime(9,0), RepeatMode::Never, {}, false);
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

TEST(AlarmItemWidgetTest, EmptyNameDefaultsAndSpecificDaysEmptyShowsOnce) {
    AlarmData a = makeAlarm("", QTime(6,0), RepeatMode::SpecificDays, {});
    AlarmItemWidget w(a);

    auto lName = w.findChild<QLabel*>("labelName");
    auto lRepeat = w.findChild<QLabel*>("labelRepeat");
    ASSERT_TRUE(lName && lRepeat);

    EXPECT_EQ(lName->text(), "Alarm");
    EXPECT_EQ(lRepeat->text(), "Once");
}

TEST(AlarmItemWidgetTest, EventFilterHandlesChildHover) {
    AlarmData a = makeAlarm("A", QTime(9,0));
    AlarmItemWidget w(a);
    auto child = w.findChild<QLabel*>("labelTime");
    ASSERT_TRUE(child);

    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(child, &enter);
    EXPECT_TRUE(w.property("hovered").toBool());

    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(child, &leave);
    EXPECT_FALSE(w.property("hovered").toBool());
}

TEST(AlarmFactoryTest, DisabledAlarmAddsOpacityEffect) {
    AlarmData a = makeAlarm("Off", QTime(7,0), RepeatMode::Never, {}, false);
    auto *w = AlarmFactory::createAlarmWidget(a, nullptr);
    ASSERT_TRUE(w);
    auto *effect = qobject_cast<QGraphicsOpacityEffect*>(w->graphicsEffect());
    ASSERT_TRUE(effect);
    EXPECT_LT(effect->opacity(), 1.0);
    delete w;
}


TEST(AlarmWindowTest, UpdateListBuildsWidgetsAndConnectsToggles) {
    AlarmWindow w;
    AlarmManager* manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    manager->addAlarm(makeAlarm("A", QTime(6,10), RepeatMode::Never));
    manager->addAlarm(makeAlarm("B", QTime(7,20), RepeatMode::Weekdays));

    QListWidget* list = w.findChild<QListWidget*>("listAlarms");
    ASSERT_TRUE(list);
    EXPECT_GE(list->count(), 1);

    QWidget* w0 = list->itemWidget(list->item(0));
    auto aiw = qobject_cast<AlarmItemWidget*>(w0);
    ASSERT_TRUE(aiw);

    QSignalSpy spy(&w, &AlarmWindow::alarmToggled);
    emit aiw->toggled(false);
    EXPECT_GE(spy.count(), 1);
}

TEST(AlarmWindowTest, SetAlarmsAppliesOpacityForDisabled) {
    AlarmWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listAlarms");
    ASSERT_TRUE(list);

    AlarmData on = makeAlarm("On", QTime(6, 0), RepeatMode::Never, {}, true);
    AlarmData off = makeAlarm("Off", QTime(7, 0), RepeatMode::Never, {}, false);
    w.setAlarms(QList<AlarmData>{on, off});

    ASSERT_EQ(list->count(), 2);
    QWidget* w0 = list->itemWidget(list->item(0));
    QWidget* w1 = list->itemWidget(list->item(1));
    ASSERT_TRUE(w0 && w1);

    EXPECT_EQ(w0->graphicsEffect(), nullptr);
    auto *effect = qobject_cast<QGraphicsOpacityEffect*>(w1->graphicsEffect());
    ASSERT_TRUE(effect);
    EXPECT_LT(effect->opacity(), 1.0);
}

TEST(AlarmWindowTest, GetNextAlarmStringFormatsTodayTomorrowOrWeekday) {
    AlarmWindow w;
    AlarmManager* manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    for (int i = manager->getAlarms().size() - 1; i >= 0; --i)
        manager->removeAlarm(i);

    EXPECT_EQ(w.getNextAlarmString(), "No alarms set");

    AlarmData a1 = makeAlarm("Soon", QTime::currentTime(), RepeatMode::Never, {}, true);
    a1.nextTrigger = QDateTime::currentDateTime().addSecs(60);
    AlarmData a2 = makeAlarm("Later", QTime::currentTime(), RepeatMode::Never, {}, true);
    a2.nextTrigger = QDateTime::currentDateTime().addDays(1).addSecs(30);

    manager->addAlarm(a1);
    manager->addAlarm(a2);

    QString s = w.getNextAlarmString();
    EXPECT_FALSE(s.isEmpty());
    EXPECT_TRUE(s.contains(":"));
}

TEST(AlarmWindowTest, GetNextAlarmStringNoUpcomingWhenAllDisabled) {
    AlarmWindow w;
    AlarmManager* manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    for (int i = manager->getAlarms().size() - 1; i >= 0; --i)
        manager->removeAlarm(i);

    AlarmData a = makeAlarm("Off", QTime(8,0), RepeatMode::Never, {}, false);
    a.nextTrigger = QDateTime::currentDateTime().addDays(1);
    manager->addAlarm(a);

    EXPECT_EQ(w.getNextAlarmString(), "No upcoming alarms");
}

TEST(AlarmWindowTest, GetNextAlarmStringUsesTomorrowLabel) {
    AlarmWindow w;
    AlarmManager* manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    for (int i = manager->getAlarms().size() - 1; i >= 0; --i)
        manager->removeAlarm(i);

    AlarmData a = makeAlarm("Tomorrow", QTime(7,30), RepeatMode::Never, {}, true);
    a.nextTrigger = QDateTime::currentDateTime().addDays(1).addSecs(60);
    manager->addAlarm(a);

    QString s = w.getNextAlarmString();
    EXPECT_TRUE(s.startsWith("Tomorrow"));
}

TEST(AlarmWindowTest, AlarmsFilePathLooksValid) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = dir + "/alarms.json";

    EXPECT_FALSE(dir.isEmpty());
    EXPECT_TRUE(path.endsWith("/alarms.json") || path.endsWith("\\alarms.json"));
}
TEST(AlarmWindowTest, ListSelectionStateIsTracked) {
    AlarmWindow w;
    AlarmManager* manager = w.findChild<AlarmManager*>();
    ASSERT_TRUE(manager);

    for (int i = manager->getAlarms().size() - 1; i >= 0; --i) {
        manager->removeAlarm(i);
    }

    // Тепер додаємо рівно 3 будильники для нашого тесту
    manager->addAlarm(makeAlarm("A", QTime(6, 0)));
    manager->addAlarm(makeAlarm("B", QTime(7, 0)));
    manager->addAlarm(makeAlarm("C", QTime(8, 0)));

    QListWidget* list = w.findChild<QListWidget*>("listAlarms");
    ASSERT_TRUE(list);

    ASSERT_EQ(list->count(), 3);

    list->item(0)->setSelected(true);
    list->item(2)->setSelected(true);

    EXPECT_EQ(list->selectedItems().count(), 2);

    EXPECT_TRUE(list->item(0)->isSelected());
    EXPECT_FALSE(list->item(1)->isSelected());
    EXPECT_TRUE(list->item(2)->isSelected());
}
TEST(AlarmSettingsDialogTest, EmptyLabelDefaultsToAlarm) {
    AlarmSettingsDialog dlg;
    auto line = dlg.findChild<QLineEdit*>("lineLabel");
    ASSERT_TRUE(line);

    line->setText("   ");
    EXPECT_EQ(dlg.getLabel(), "Alarm");
}

TEST(AlarmWindowTest, RemoveAlarmWithNoSelectionShowsWarning) {
    AlarmWindow w;
    QPushButton* btnRemove = w.findChild<QPushButton*>("btnRemove");
    ASSERT_TRUE(btnRemove);

    QTimer::singleShot(100, []() {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            if (auto *mb = qobject_cast<QMessageBox*>(widget)) {
                mb->accept();
            }
        }
    });

    QSignalSpy spy(&w, &AlarmWindow::removeAlarmsRequested);

    btnRemove->click();

    EXPECT_EQ(spy.count(), 0);
}
TEST(SoundAlarmActionTest, CanConstructAndSetPath) {
    SoundAlarmAction action("qrc:/custom/sound.wav");


    EXPECT_NO_THROW(action.setSoundPath("qrc:/another/sound.wav"));
    EXPECT_NO_THROW(action.setSoundPath(""));
}
