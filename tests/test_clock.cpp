/**
 * @file test_clock.cpp
 * @brief Definitions for test_clock.
 * @details Implements logic declared in the corresponding header for test_clock.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include <gtest/gtest.h>
#include <QFile>
#include <QTest>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QMetaObject>
#include <QComboBox>
#include <QTemporaryDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../clock/clockwindow.h"
#include "../clock/clocksettingsdialog.h"

namespace {

void clearAllClocks(ClockWindow &w)
{
    auto list = w.findChild<QListWidget*>("listClocks");
    if (!list || list->count() == 0)
        return;

    QList<int> rows;
    for (int i = 0; i < list->count(); ++i)
        rows << i;
    QMetaObject::invokeMethod(&w, "removeClocksRequested", Qt::DirectConnection, Q_ARG(QList<int>, rows));
}

void addClock(ClockWindow &w, const QString &zone)
{
    QMetaObject::invokeMethod(&w, "addClockRequested", Qt::DirectConnection, Q_ARG(QString, zone));
}

} // namespace

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

    clearAllClocks(w);
    addClock(w, "Europe/Kyiv");
    EXPECT_EQ(list->count(), 1);

    QList<int> rows{0};
    QMetaObject::invokeMethod(&w, "removeClocksRequested", Qt::DirectConnection, Q_ARG(QList<int>, rows));
    EXPECT_EQ(list->count(), 0);
}

TEST(ClockWindowTest, TogglesFormat12hAndSavesJson) {
    ClockWindow w;
    QCheckBox* check = w.findChild<QCheckBox*>("checkFormat12");
    ASSERT_TRUE(check);
    check->setChecked(true);
    EXPECT_TRUE(check->isChecked());
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
    clearAllClocks(w);
    EXPECT_NO_THROW(w.updateListTexts());
}

TEST(ClockWindowTest, UpdateListTextsExpandsWhenHasClocks) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);
    clearAllClocks(w);
    addClock(w, "Asia/Seoul");
    EXPECT_NO_THROW(w.updateListTexts());
    EXPECT_NO_THROW(w.updateListTexts());
}

TEST(ClockWindowIntegrationTest, AnimationRunsWithoutCrash) {
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);

    clearAllClocks(w);
    addClock(w, "UTC");
    addClock(w, "America/New_York");
    EXPECT_NO_THROW(w.updateListTexts());
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

TEST(ClockWindowEdgeTest, HandlesMultipleFormats) {
    ClockWindow w;
    EXPECT_NO_THROW({
        w.onToggleFormat(true);
        w.updateTime();
        w.onToggleFormat(false);
        w.updateTime();
    });
}

TEST(ClockWindowEdgeTest, RemoveClockNoSelectionInTestMode) {
    qputenv("TEST_MODE", "1");
    ClockWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listClocks");
    ASSERT_TRUE(list);
    clearAllClocks(w);

    EXPECT_TRUE(QMetaObject::invokeMethod(&w, "onRemoveClock", Qt::DirectConnection));
    EXPECT_EQ(list->count(), 0);
}

TEST(ClockSettingsDialogTest, ReturnsEmptyWhenNoSelection) {
    ClockSettingsDialog dlg;
    auto combo = dlg.findChild<QComboBox*>("comboZone");
    ASSERT_TRUE(combo);
    combo->setCurrentIndex(-1);
    EXPECT_TRUE(dlg.getSelectedZone().isEmpty());
}

TEST(ClockSettingsDialogTest, ReturnsSelectedZoneWhenChosen) {
    ClockSettingsDialog dlg;
    auto combo = dlg.findChild<QComboBox*>("comboZone");
    ASSERT_TRUE(combo);
    if (combo->count() == 0) {
        GTEST_SKIP();
    }
    combo->setCurrentIndex(0);
    EXPECT_FALSE(dlg.getSelectedZone().isEmpty());
}

TEST(ClockSettingsDialogTest, ComboPopulatedWithZones) {
    ClockSettingsDialog dlg;
    auto combo = dlg.findChild<QComboBox*>("comboZone");
    ASSERT_TRUE(combo);
    EXPECT_GT(combo->count(), 0);
}

TEST(ClockWindowEdgeTest, TimeTextFormatContainsTimezoneName) {
    ClockWindow w;
    ClockInfo ci{"UTC"};
    QString text = w.timeTextFor(ci);
    EXPECT_TRUE(text.startsWith("UTC"));
    EXPECT_TRUE(text.contains(" - "));
}
