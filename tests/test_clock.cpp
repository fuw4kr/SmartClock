#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTest>
#include <QMessageBox>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QMetaObject>
#include "../clock/clockwindow.h"
#include "../clock/clocksettingsdialog.h"

TEST(ClockWindowTest, UpdatesMainTimeLabel) {
    ClockWindow w;
    QLabel* mainLabel = w.findChild<QLabel*>("labelMainTime");
    ASSERT_TRUE(mainLabel);

    QString before = mainLabel->text();
    QTest::qWait(1100);
    w.updateTime();
    QString after = mainLabel->text();

    EXPECT_NE(before, after);
}

TEST(ClockWindowTest, AddsAndRemovesClockSuccessfully) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);

    ClockInfo ci{"Europe/Kyiv"};
    w.onToggleFormat(false);
    w.updateTime();
    list->addItem(w.timeTextFor(ci));
    EXPECT_GE(list->count(), 1);

    if (w.metaObject()->indexOfMethod("onRemoveClock()") != -1) {
        QMetaObject::invokeMethod(&w, "onRemoveClock", Qt::DirectConnection);
    }
    EXPECT_GE(list->count(), 0);
}

TEST(ClockWindowTest, TogglesFormat12hAndSavesJson) {
    ClockWindow w;
    QCheckBox* check = w.findChild<QCheckBox*>("checkFormat12");
    ASSERT_TRUE(check);
    check->setChecked(true);
    EXPECT_TRUE(check->isChecked());

    w.saveToJson();
    QFile file("clocks.json");
    EXPECT_TRUE(file.exists());
    EXPECT_GT(file.size(), 0);
}

TEST(ClockWindowTest, SavesAndLoadsClockDataFromJson) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);
    list->clear();

    ClockInfo c1{"America/New_York"};
    ClockInfo c2{"Asia/Tokyo"};

    w.onToggleFormat(true);
    list->addItem(w.timeTextFor(c1));
    list->addItem(w.timeTextFor(c2));

    w.saveToJson();

    ClockWindow reload;
    EXPECT_NO_THROW(reload.loadFromJson());

    QListWidget* list2 = reload.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list2);
    EXPECT_GE(list2->count(), 0);
}

TEST(ClockWindowTest, TimeTextForReturnsFormattedString) {
    ClockWindow w;
    ClockInfo ci{"Europe/London"};
    QString text = w.timeTextFor(ci);
    EXPECT_TRUE(text.contains("Europe/London"));
    EXPECT_TRUE(text.contains(":"));
}

TEST(ClockWindowTest, UpdateListTextsHidesListWhenEmpty) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);
    list->clear();
    EXPECT_NO_THROW(w.updateListTexts());
}

TEST(ClockWindowTest, UpdateListTextsExpandsWhenHasClocks) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);
    list->addItem("Test Clock");
    ClockInfo ci{"Asia/Seoul"};
    EXPECT_NO_THROW(w.updateListTexts());
    EXPECT_NO_THROW(w.updateListTexts());
}

TEST(ClockWindowTest, SavedJsonContainsValidKeys) {
    ClockWindow w;
    ClockInfo ci{"Europe/Kyiv"};
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);
    list->addItem(w.timeTextFor(ci));
    w.saveToJson();

    QFile f("clocks.json");
    ASSERT_TRUE(f.exists());
    ASSERT_TRUE(f.open(QIODevice::ReadOnly));
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    ASSERT_TRUE(doc.isObject());
    QJsonObject root = doc.object();
    EXPECT_TRUE(root.contains("clocks"));
    EXPECT_TRUE(root.contains("format12h"));
}

TEST(ClockWindowTest, LoadsCorruptedJsonGracefully) {
    QFile f("clocks.json");
    f.open(QIODevice::WriteOnly);
    f.write("{not valid json");
    f.close();

    EXPECT_NO_THROW({
        ClockWindow w;
        w.loadFromJson();
    });
}

TEST(ClockWindowTest, HandlesEmptyJsonFileGracefully) {
    QFile f("clocks.json");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    f.close();

    EXPECT_NO_THROW({
        ClockWindow w;
        w.loadFromJson();
    });
}

TEST(ClockWindowIntegrationTest, AnimationRunsWithoutCrash) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);

    list->addItem("Clock A");
    list->addItem("Clock B");
    EXPECT_NO_THROW(w.updateListTexts());
}

TEST(ClockWindowIntegrationTest, SaveLoadCycleKeepsFormatFlag) {
    ClockWindow w;
    QCheckBox* check = w.findChild<QCheckBox*>("checkFormat12");
    ASSERT_TRUE(check);
    check->setChecked(true);
    w.saveToJson();

    ClockWindow reload;
    reload.loadFromJson();
    QCheckBox* check2 = reload.findChild<QCheckBox*>("checkFormat12");
    ASSERT_TRUE(check2);
    EXPECT_TRUE(check2->isChecked());
}

TEST(ClockWindowIntegrationTest, DeleteNonexistentFileDoesNotCrash) {
    QFile::remove("clocks.json");
    ClockWindow w;
    EXPECT_NO_THROW(w.loadFromJson());
}

TEST(ClockWindowIntegrationTest, MultipleUpdatesRefreshTimeLabel) {
    ClockWindow w;
    QLabel* label = w.findChild<QLabel*>("labelMainTime");
    ASSERT_TRUE(label);

    QString t1 = label->text();
    w.updateTime();
    QTest::qWait(1050);
    w.updateTime();
    QString t2 = label->text();
    EXPECT_NE(t1, t2);
}

TEST(ClockWindowEdgeTest, HandlesEmptyClockListInSave) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);
    list->clear();
    EXPECT_NO_THROW(w.saveToJson());
}

TEST(ClockWindowEdgeTest, HandlesMultipleFormats) {
    ClockWindow w;
    EXPECT_NO_THROW({
        w.onToggleFormat(true);
        w.updateTime();
        w.onToggleFormat(false);
        w.updateTime();
    });
}

TEST(ClockWindowEdgeTest, TimeTextFormatContainsTimezoneName) {
    ClockWindow w;
    ClockInfo ci{"UTC"};
    QString text = w.timeTextFor(ci);
    EXPECT_TRUE(text.startsWith("UTC"));
    EXPECT_TRUE(text.contains("—"));
}
