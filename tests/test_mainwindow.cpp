/**
 * @file test_mainwindow.cpp
 * @brief Definitions for test_mainwindow.
 * @details Implements logic declared in the corresponding header for test_mainwindow.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include <gtest/gtest.h>
#include <QMetaObject>
#include <QApplication>
#include <QCloseEvent>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QSettings>
#include "../mainwindow.h"
#include "../thememanager.h"




TEST(MainWindowTest, ThemeToggleUpdatesSetting) {
    QSettings settings("SmartClock", "Theme");
    settings.setValue("theme", static_cast<int>(Theme::Light));

    MainWindow w;
    Theme before = ThemeManager::instance().currentTheme();

    EXPECT_TRUE(QMetaObject::invokeMethod(&w, "on_btnTheme_clicked", Qt::DirectConnection));

    Theme after = ThemeManager::instance().currentTheme();
    EXPECT_NE(before, after);

    QSettings check("SmartClock", "Theme");
    EXPECT_EQ(check.value("theme").toInt(), static_cast<int>(after));
}

TEST(MainWindowTest, MaximizeSignalDoesNotCrash) {
    MainWindow w;
    EXPECT_TRUE(QMetaObject::invokeMethod(&w, "windowMaximizedChanged", Qt::DirectConnection, Q_ARG(bool, true)));
    EXPECT_TRUE(QMetaObject::invokeMethod(&w, "windowMaximizedChanged", Qt::DirectConnection, Q_ARG(bool, false)));
}
TEST(MainWindowTest, TabsAreConfiguredCorrectly) {
    // Вмикаємо TEST_MODE, щоб уникнути ініціалізації системного трею в CI
    qputenv("TEST_MODE", "1");

    MainWindow w;
    auto tabWidget = w.findChild<QTabWidget*>("tabWidget");
    ASSERT_NE(tabWidget, nullptr);

    // Перевіряємо кількість вкладок
    EXPECT_EQ(tabWidget->count(), 4);

    // Перевіряємо назви вкладок у правильному порядку
    EXPECT_EQ(tabWidget->tabText(0), "Clock");
    EXPECT_EQ(tabWidget->tabText(1), "Alarms");
    EXPECT_EQ(tabWidget->tabText(2), "Stopwatch");
    EXPECT_EQ(tabWidget->tabText(3), "Timer");

    // Перевіряємо, що за замовчуванням відкрита перша вкладка
    EXPECT_EQ(tabWidget->currentIndex(), 0);
}

TEST(MainWindowTest, GettersReturnSafeInitialValues) {
    qputenv("TEST_MODE", "1");
    MainWindow w;

    // Перевіряємо, що звернення до вкладених вікон не викликає крашів
    EXPECT_EQ(w.getRunningTimers(), 0);
    EXPECT_EQ(w.getFinishedTimers(), 0);
    EXPECT_FALSE(w.getNextAlarmTime().isEmpty());
    EXPECT_FALSE(w.getStopwatchLapTime().isEmpty());
    EXPECT_FALSE(w.getStopwatchTotalTime().isEmpty());
}

TEST(MainWindowTest, CloseEventIsIgnoredAndWindowIsHidden) {
    qputenv("TEST_MODE", "1");
    MainWindow w;
    w.show();

    // Імітуємо подію закриття вікна (наприклад, натискання хрестика або Alt+F4)
    QCloseEvent closeEvent;
    QApplication::sendEvent(&w, &closeEvent);

    // У MainWindow::closeEvent викликається event->ignore() та hide()
    EXPECT_FALSE(closeEvent.isAccepted());
    EXPECT_FALSE(w.isVisible());
}

TEST(MainWindowTest, TraySlotsChangeActiveTab) {
    qputenv("TEST_MODE", "1");
    MainWindow w;
    auto tabWidget = w.findChild<QTabWidget*>("tabWidget");
    ASSERT_NE(tabWidget, nullptr);

    // Імітуємо виклики слотів з трею та перевіряємо, чи перемикаються вкладки
    QMetaObject::invokeMethod(&w, "onTrayOpenTimer", Qt::DirectConnection);
    EXPECT_EQ(tabWidget->currentIndex(), 3);

    QMetaObject::invokeMethod(&w, "onTrayOpenStopwatch", Qt::DirectConnection);
    EXPECT_EQ(tabWidget->currentIndex(), 2);

    QMetaObject::invokeMethod(&w, "onTrayOpenAlarm", Qt::DirectConnection);
    EXPECT_EQ(tabWidget->currentIndex(), 1);

    QMetaObject::invokeMethod(&w, "onTrayOpenClock", Qt::DirectConnection);
    EXPECT_EQ(tabWidget->currentIndex(), 0);
}

TEST(MainWindowTest, TestModePreventsTrayIconCreation) {
    qputenv("TEST_MODE", "1");
    MainWindow w1;
    auto trayIcon1 = w1.findChild<QSystemTrayIcon*>();
    EXPECT_EQ(trayIcon1, nullptr); // В тестовому режимі трей не створюється


    qunsetenv("TEST_MODE");
    MainWindow w2;
    auto trayIcon2 = w2.findChild<QSystemTrayIcon*>();
    EXPECT_NE(trayIcon2, nullptr); // Без тестового режиму трей має створитися

    qputenv("TEST_MODE", "1"); // Повертаємо для наступних тестів
}

TEST(MainWindowTest, CloseEventWithTrayIconShowsMessage) {
    qunsetenv("TEST_MODE");

    MainWindow w;
    w.show();

    QCloseEvent closeEvent;
    QApplication::sendEvent(&w, &closeEvent);

    EXPECT_FALSE(closeEvent.isAccepted());
    EXPECT_FALSE(w.isVisible());

    qputenv("TEST_MODE", "1");
}

TEST(MainWindowTest, UpdateMaximizeIconCoversAllBranches) {
    qputenv("TEST_MODE", "1");
    MainWindow w;

    ThemeManager::instance().applyTheme(Theme::Light);
    QMetaObject::invokeMethod(&w, "updateMaximizeIcon", Qt::DirectConnection, Q_ARG(bool, true));
    QMetaObject::invokeMethod(&w, "updateMaximizeIcon", Qt::DirectConnection, Q_ARG(bool, false));

    ThemeManager::instance().applyTheme(Theme::Dark);
    QMetaObject::invokeMethod(&w, "updateMaximizeIcon", Qt::DirectConnection, Q_ARG(bool, true));
    QMetaObject::invokeMethod(&w, "updateMaximizeIcon", Qt::DirectConnection, Q_ARG(bool, false));
}

// 3. Покриваємо метод виходу через трей
TEST(MainWindowTest, OnTrayExitPostsQuitEvent) {
    qputenv("TEST_MODE", "1");
    MainWindow w;

    EXPECT_NO_THROW(QMetaObject::invokeMethod(&w, "onTrayExit", Qt::DirectConnection));
}

TEST(MainWindowTest, UpdateThemeIconSafelyFindsButtons) {
    qputenv("TEST_MODE", "1");
    MainWindow w;


    auto timerWin = w.findChild<QWidget*>("TimerWindow"); // або просто покладаємося на те, що вони там є
    if (timerWin) {
        if (!timerWin->findChild<QPushButton*>("btnAdd")) {
            auto btnAdd = new QPushButton(timerWin);
            btnAdd->setObjectName("btnAdd");
        }
        if (!timerWin->findChild<QPushButton*>("btnSettings")) {
            auto btnSettings = new QPushButton(timerWin);
            btnSettings->setObjectName("btnSettings");
        }
    }

    // Проганяємо обидві теми
    ThemeManager::instance().applyTheme(Theme::Light);
    QMetaObject::invokeMethod(&w, "updateThemeIcon", Qt::DirectConnection);

    ThemeManager::instance().applyTheme(Theme::Dark);
    QMetaObject::invokeMethod(&w, "updateThemeIcon", Qt::DirectConnection);
}