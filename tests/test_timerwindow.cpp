#include <gtest/gtest.h>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QApplication>
#include <QCoreApplication>
#include <QMetaObject>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include "../timer/timerwindow.h"
#include "../timer/historytimerwindow.h"

TEST(TimerWindowTest, InitialWidgetsPresent) {
    TimerWindow w;
    auto *table = w.findChild<QTableWidget*>("tableTimers");
    auto *filter = w.findChild<QComboBox*>("comboBox");
    auto *nextUp = w.findChild<QLabel*>("labelNextUp");
    ASSERT_TRUE(table);
    ASSERT_TRUE(filter);
    ASSERT_TRUE(nextUp);

    EXPECT_GE(filter->count(), 3);
}

TEST(TimerWindowTest, AddingTimerUpdatesTable) {
    TimerWindow w;
    auto *table = w.findChild<QTableWidget*>("tableTimers");
    ASSERT_TRUE(table);

    TimerManager *manager = w.getManager();
    ASSERT_TRUE(manager);

    manager->addTimer("UI", 10, "Normal", "Default");
    QCoreApplication::processEvents();

    EXPECT_EQ(table->rowCount(), 1);
}

TEST(TimerWindowTest, UpdateNextUpLabelWhenEmpty) {
    TimerWindow w;
    auto *label = w.findChild<QLabel*>("labelNextUp");
    ASSERT_TRUE(label);

    EXPECT_TRUE(QMetaObject::invokeMethod(&w, "updateNextUpLabel", Qt::DirectConnection));
    EXPECT_TRUE(label->text().contains("None"));
}

TEST(TimerWindowTest, StartPauseWarningIfNoSelection) {
    TimerWindow w;
    // Налаштовуємо автоматичне закриття попередження
    QTimer::singleShot(100, []() {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            if (auto *mb = qobject_cast<QMessageBox*>(widget)) mb->accept();
        }
    });
    // Викликаємо метод без виділених рядків
    EXPECT_NO_THROW(QMetaObject::invokeMethod(&w, "onStartPauseTimer", Qt::DirectConnection));
}

TEST(TimerWindowTest, CloseEventPausesTimersIfContinueAfterExitIsFalse) {
    TimerWindow w;
    TimerManager *manager = w.getManager();
    manager->addTimer("Run", 60);
    manager->startTimer(0);

    // За замовчуванням continueAfterExit = false
    QCloseEvent closeEvent;
    QApplication::sendEvent(&w, &closeEvent);

    EXPECT_TRUE(closeEvent.isAccepted());
    // Таймер має поставитися на паузу при закритті
    EXPECT_EQ(manager->getTimers()[0].status, TimerStatus::Paused);
}

TEST(TimerWindowTest, FilterComboBoxUpdatesTable) {
    TimerWindow w;
    TimerManager *manager = w.getManager();
    manager->addTimer("RunningTimer", 60);
    manager->startTimer(0);
    manager->addTimer("PausedTimer", 60);

    auto *combo = w.findChild<QComboBox*>("comboBox");
    auto *table = w.findChild<QTableWidget*>("tableTimers");
    ASSERT_TRUE(combo && table);

    combo->setCurrentText("Running");
    EXPECT_EQ(table->rowCount(), 1);

    combo->setCurrentText("Paused");
    EXPECT_EQ(table->rowCount(), 1);
}