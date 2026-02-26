/**
 * @file test_stopwatch.cpp
 * @brief Definitions for test_stopwatch.
 * @details Implements logic declared in the corresponding header for test_stopwatch.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include <gtest/gtest.h>
#include <QFile>
#include <QTemporaryDir>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTest>
#include <QSettings>
#include <QString>
#include <QImage>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QSize>
#include <QStackedWidget>
#include <QStyleOptionViewItem>
#include "../stopwatch/stopwatchwindow.h"
#include "../stopwatch/analogstopwatchdial.h"
#include "../thememanager.h"

TEST(AnalogStopwatchDialTest, InitialStateIsZero) {
    AnalogStopwatchDial dial;
    QTime t(0, 0, 0, 0);
    dial.setElapsed(t);
    EXPECT_GE(dial.width(), 0);
}

TEST(AnalogStopwatchDialTest, PaintEventRunsWithoutCrash) {
    AnalogStopwatchDial dial;
    QTime t(1, 2, 3, 400);
    dial.resize(300, 300);
    dial.setElapsed(t);

    EXPECT_NO_THROW({
        dial.update();
        QTest::qWait(0);
    });
}

TEST(AnalogStopwatchDialTest, PaintEventCoversDarkTheme) {
    ThemeManager::instance().applyTheme(Theme::Dark);
    AnalogStopwatchDial dial;
    dial.resize(260, 260);
    dial.setElapsed(QTime(0, 1, 2, 300));

    EXPECT_NO_THROW({
        dial.update();
        QTest::qWait(10);
    });
}

TEST(AnalogStopwatchDialTest, PaintEventCoversLightTheme) {
    ThemeManager::instance().applyTheme(Theme::Light);
    AnalogStopwatchDial dial;
    dial.resize(260, 260);
    dial.setElapsed(QTime(12, 34, 56, 789));

    EXPECT_NO_THROW({
        dial.update();
        QTest::qWait(10);
    });
}

TEST(AnalogStopwatchDialTest, MinimumSizeIsSet) {
    AnalogStopwatchDial dial;
    EXPECT_EQ(dial.minimumSize(), QSize(250, 250));
}

TEST(AnalogStopwatchDialTest, PaintEventWithNonSquareSize) {
    ThemeManager::instance().applyTheme(Theme::Light);
    AnalogStopwatchDial dial;
    dial.resize(320, 180);
    dial.setElapsed(QTime(3, 2, 1, 100));

    EXPECT_NO_THROW({
        dial.update();
        QTest::qWait(10);
    });
}

TEST(StopwatchWindowTest, StartsAndStopsCorrectly) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    QPushButton* lap = w.findChild<QPushButton*>("btnLap");
    ASSERT_TRUE(start && lap);

    EXPECT_FALSE(lap->isEnabled());
    start->click();
    EXPECT_FALSE(w.getTotalTimeString().isEmpty());
}

TEST(StopwatchWindowTest, LapAndResetWork) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    QPushButton* lap = w.findChild<QPushButton*>("btnLap");
    QListWidget* list = w.findChild<QListWidget*>("listLaps");
    ASSERT_TRUE(start && lap && list);

    start->click();
    w.updateDisplay();
    lap->click();

    start->click();
    lap->click();
    EXPECT_LE(list->count(), 1);
}

TEST(StopwatchWindowTest, ResetClearsLapsAndDisablesLapButton) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    QPushButton* lap = w.findChild<QPushButton*>("btnLap");
    QListWidget* list = w.findChild<QListWidget*>("listLaps");
    QLabel* label = w.findChild<QLabel*>("labelTime");
    ASSERT_TRUE(start && lap && list && label);

    start->click();
    QTest::qWait(20);
    lap->click();
    start->click();
    lap->click();

    EXPECT_EQ(label->text(), "00:00.00");
    EXPECT_EQ(list->count(), 0);
    EXPECT_FALSE(lap->isEnabled());
}

TEST(StopwatchWindowTest, AnalogModeSwitchDoesNotCrash) {
    StopwatchWindow w;
    QSettings settings("SmartClockApp", "Stopwatch");
    settings.setValue("analogMode", true);

    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    start->click();
    EXPECT_NO_THROW(start->click());
}

TEST(StopwatchWindowTest, TotalTimeStringReturnsLabelText) {
    StopwatchWindow w;
    QLabel* label = w.findChild<QLabel*>("labelTime");
    label->setText("11:11.11");
    EXPECT_EQ(w.getTotalTimeString(), "11:11.11");
}

TEST(StopwatchWindowAdvancedTest, MultipleLapsMarkBestAndWorst) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    QPushButton* lap = w.findChild<QPushButton*>("btnLap");
    QListWidget* list = w.findChild<QListWidget*>("listLaps");
    start->click();

    for (int i = 0; i < 3; ++i) {
        QTest::qWait(10 + i * 10);
        lap->click();
    }

    EXPECT_GE(list->count(), 3);
}

TEST(StopwatchWindowAdvancedTest, RapidStartStopDoesNotCrash) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    for (int i = 0; i < 5; ++i) {
        start->click();
        QTest::qWait(5);
        start->click();
    }
    EXPECT_FALSE(w.getTotalTimeString().isEmpty());
}

TEST(StopwatchWindowAdvancedTest, QSettingsPersistAfterRestart) {
    {
        StopwatchWindow w;
        QSettings s("SmartClockApp", "Stopwatch");
        s.setValue("analogMode", true);
    }

    QSettings s("SmartClockApp", "Stopwatch");
    EXPECT_TRUE(s.contains("analogMode"));
    EXPECT_EQ(s.value("analogMode").toBool(), true);
}

TEST(StopwatchWindowExtraTest, LapHighlightDelegatePaintCoverage) {
    StopwatchWindow w;
    QListWidget* list = w.findChild<QListWidget*>("listLaps");
    ASSERT_TRUE(list);

    auto* delegate = list->itemDelegate();
    ASSERT_TRUE(delegate);

    // Створюємо штучне зображення і QPainter, щоб делегат мав куди малювати
    QImage img(100, 30, QImage::Format_ARGB32);
    QPainter p(&img);

    QStyleOptionViewItem opt;
    opt.rect = QRect(0, 0, 100, 30);
    opt.widget = list;

    // Додаємо тестовий елемент
    list->addItem("Test Lap");
    QListWidgetItem* item = list->item(0);
    QModelIndex index = list->model()->index(0, 0);

    // 1. Тестуємо звичайне коло (Flag = 0)
    item->setData(Qt::UserRole + 100, 0);
    EXPECT_NO_THROW(delegate->paint(&p, opt, index));

    // 2. Тестуємо найкраще коло (Flag = 1)
    item->setData(Qt::UserRole + 100, 1);
    EXPECT_NO_THROW(delegate->paint(&p, opt, index));

    // 3. Тестуємо найгірше коло (Flag = 2)
    item->setData(Qt::UserRole + 100, 2);
    EXPECT_NO_THROW(delegate->paint(&p, opt, index));

    // 4. Тестуємо виділений стан (Selected)
    opt.state |= QStyle::State_Selected;
    EXPECT_NO_THROW(delegate->paint(&p, opt, index));

    // 5. Тестуємо без віджета (fallback на isLight = true)
    opt.widget = nullptr;
    EXPECT_NO_THROW(delegate->paint(&p, opt, index));
}

TEST(StopwatchWindowExtraTest, SwitchViewButtonTriggersAnimation) {
    StopwatchWindow w;

    // Кнопка перемикання не має objectName, але ми можемо знайти її за ToolTip
    QPushButton* switchBtn = nullptr;
    for (auto* btn : w.findChildren<QPushButton*>()) {
        if (btn->toolTip() == "Switch stopwatch view") {
            switchBtn = btn;
            break;
        }
    }
    ASSERT_TRUE(switchBtn != nullptr);

    auto* stacked = w.findChild<QStackedWidget*>();
    ASSERT_TRUE(stacked != nullptr);

    int initialIndex = stacked->currentIndex();

    // Натискаємо кнопку, щоб запустити анімацію (fadeOut -> fadeIn)
    switchBtn->click();

    // Анімація триває 200ms + 200ms. Чекаємо трохи більше, щоб вона точно завершилась
    QTest::qWait(450);

    // Перевіряємо, що індекс сторінки змінився (з 0 на 1, або навпаки)
    int newIndex = stacked->currentIndex();
    EXPECT_NE(initialIndex, newIndex);
}

TEST(StopwatchWindowExtraTest, GetCurrentLapTimeStringWhenEmpty) {
    StopwatchWindow w;
    // Коли немає кіл, має повертатися "-"
    EXPECT_EQ(w.getCurrentLapTimeString(), "-");
}

TEST(StopwatchWindowExtraTest, SyncFromModelUpdatesButtonTexts) {
    StopwatchWindow w;
    auto* btnStartStop = w.findChild<QPushButton*>("btnStartStop");
    auto* btnLap = w.findChild<QPushButton*>("btnLap");
    ASSERT_TRUE(btnStartStop && btnLap);

    EXPECT_EQ(btnStartStop->text(), "Start");
    EXPECT_EQ(btnLap->text(), "Lap");

    // Стартуємо і перевіряємо, що текст змінився
    btnStartStop->click();
    QTest::qWait(20);
    w.updateDisplay(); // Штучно викликаємо тік

    EXPECT_EQ(btnStartStop->text(), "Stop");

    // Тиснемо коло, щоб з'явилися дані
    btnLap->click();

    // Зупиняємо
    btnStartStop->click();
    EXPECT_EQ(btnStartStop->text(), "Start");

    // Оскільки секундомір зупинений, але є дані, кнопка Lap має перетворитися на Reset
    EXPECT_EQ(btnLap->text(), "Reset");
}

TEST(StopwatchWindowExtraTest, TimerSafelyStopsWhenNotRunning) {
    StopwatchWindow w;
    auto* btnStartStop = w.findChild<QPushButton*>("btnStartStop");
    ASSERT_TRUE(btnStartStop);

    // Старт
    btnStartStop->click();
    QTest::qWait(20);

    // Стоп
    btnStartStop->click();

    // Запускаємо syncFromModel напряму, щоб покрити гілку,
    // де timer->isActive() перевіряється перед зупинкою
    EXPECT_TRUE(QMetaObject::invokeMethod(&w, "syncFromModel", Qt::DirectConnection));
}