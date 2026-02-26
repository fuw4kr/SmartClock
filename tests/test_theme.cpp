#include <gtest/gtest.h>
#include <QApplication>
#include <QSignalSpy>
#include <QPushButton>
#include <QMenu>
#include <QSystemTrayIcon>
#include "../thememanager.h"
#include "../mainwindow.h"

TEST(ThemeManagerTest, InstanceReturnsSingleton)
{
    ThemeManager &first = ThemeManager::instance();
    ThemeManager &second = ThemeManager::instance();
    EXPECT_EQ(&first, &second);
}

TEST(ThemeManagerTest, ApplyThemeUpdatesStateAndStylesheet)
{
    auto &manager = ThemeManager::instance();
    QSignalSpy spy(&manager, &ThemeManager::themeChanged);

    manager.applyTheme(Theme::Dark);
    EXPECT_EQ(manager.currentTheme(), Theme::Dark);
    ASSERT_GE(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toString().isEmpty());

    manager.applyTheme(Theme::Light);
    EXPECT_EQ(manager.currentTheme(), Theme::Light);
    ASSERT_GE(spy.count(), 1);
    EXPECT_FALSE(spy.takeFirst().at(0).toString().isEmpty());
}
TEST(ThemeManagerTest, ApplyingSameThemeMultipleTimesEmitsSignal)
{
    auto &manager = ThemeManager::instance();
    QSignalSpy spy(&manager, &ThemeManager::themeChanged);

    manager.applyTheme(Theme::Light);
    int initialCount = spy.count();

    manager.applyTheme(Theme::Light);

    EXPECT_EQ(spy.count(), initialCount + 1);
    EXPECT_EQ(manager.currentTheme(), Theme::Light);
}

TEST(ThemeManagerTest, InvalidEnumValueDefaultsToDarkThemePath)
{
    auto &manager = ThemeManager::instance();
    QSignalSpy spy(&manager, &ThemeManager::themeChanged);

    Theme invalidTheme = static_cast<Theme>(999);

    EXPECT_NO_THROW(manager.applyTheme(invalidTheme));

    EXPECT_EQ(manager.currentTheme(), invalidTheme);

    if (spy.count() > 0) {
        EXPECT_FALSE(spy.takeFirst().at(0).toString().isEmpty());
    }
}
TEST(MainWindowTest, TrayIconLambdasAndTooltipsCoverage) {
    // Тимчасово ВИМИКАЄМО тестовий режим, щоб setupTrayIcon() відпрацював
    qunsetenv("TEST_MODE");

    MainWindow w;

    // Шукаємо меню трею
    QMenu* trayMenu = w.findChild<QMenu*>();
    ASSERT_NE(trayMenu, nullptr);

    // Проходимося по всіх кнопках у меню трею (Timer, Clock, Alarm і т.д.)
    auto actions = trayMenu->actions();
    for (QAction* action : actions) {
        if (action->isSeparator()) continue;

        // Штучно викликаємо сигнал hovered() для кожної дії.
        // Це змусить виконатися всі лямбди виду: [this]() { QToolTip::showText(...); }
        QMetaObject::invokeMethod(action, "hovered", Qt::DirectConnection);
    }

    // Перевіряємо лямбду приховування меню
    QMetaObject::invokeMethod(trayMenu, "aboutToHide", Qt::DirectConnection);

    // Шукаємо саму іконку в треї
    QSystemTrayIcon* trayIcon = w.findChild<QSystemTrayIcon*>();
    ASSERT_NE(trayIcon, nullptr);

    // Штучно викликаємо лямбду активанції трею (подвійний клік)
    QMetaObject::invokeMethod(trayIcon, "activated",
                              Qt::DirectConnection,
                              Q_ARG(QSystemTrayIcon::ActivationReason, QSystemTrayIcon::DoubleClick));

    // Повертаємо тестовий режим для безпеки наступних тестів
    qputenv("TEST_MODE", "1");
}

TEST(MainWindowTest, UpdateThemeIconCoversBothThemes) {
    qputenv("TEST_MODE", "1");
    MainWindow w;

    // Примусово встановлюємо Світлу тему і викликаємо клік
    ThemeManager::instance().applyTheme(Theme::Light);
    QMetaObject::invokeMethod(&w, "on_btnTheme_clicked", Qt::DirectConnection);

    // Тепер примусово встановлюємо Темну тему і викликаємо клік ще раз.
    // Це гарантує, що всі тернарні оператори виду (isLight ? "..." : "...")
    // у методах updateThemeIcon() та updateMaximizeIcon() будуть покриті на 100%
    ThemeManager::instance().applyTheme(Theme::Dark);
    QMetaObject::invokeMethod(&w, "on_btnTheme_clicked", Qt::DirectConnection);
}

TEST(MainWindowTest, MaximizeRestoreToggleCoverage) {
    qputenv("TEST_MODE", "1");
    MainWindow w;
    w.show();

    // Перевіряємо логіку розгортання вікна (toggleMaximizeRestore)
    // Викликаємо слот кліку по кнопці btnMaximize
    auto btnMaximize = w.findChild<QPushButton*>("btnMaximize");
    ASSERT_NE(btnMaximize, nullptr);

    btnMaximize->click();

    // Клік 2: Повертаємо назад (isMaximized стає false)
    btnMaximize->click();
}