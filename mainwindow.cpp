/**
 * @file mainwindow.cpp
 * @brief Definitions for mainwindow.
 * @details Implements logic declared in the corresponding header for mainwindow.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thememanager.h"

#include <QToolBar>
#include <QSettings>
#include <QIcon>
#include <QDebug>

#include <QToolTip>
#include <QCursor>
#include <QTabBar>
#include <QAction>
#include <QCloseEvent>
#include <QTime>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : FramelessWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Smart Clock");

    clockWindow = new ClockWindow(this);
    alarmWindow = new AlarmWindow(this);
    stopwatchWindow = new StopwatchWindow(this);
    timerWindow = new TimerWindow(this);

    ui->tabWidget->clear();
    ui->tabWidget->addTab(clockWindow, QIcon(":/resources/icons/clock.png"), "Clock");
    ui->tabWidget->addTab(alarmWindow, QIcon(":/resources/icons/alarmclock.png"), "Alarms");
    ui->tabWidget->addTab(stopwatchWindow, QIcon(":/resources/icons/stopwatch.png"), "Stopwatch");
    ui->tabWidget->addTab(timerWindow, QIcon(":/resources/icons/timer.png"), "Timer");

    ui->tabWidget->tabBar()->setIconSize(QSize(20, 20));
    ui->tabWidget->setCurrentIndex(0);

    setupTitleBar();
    snapPreview = new SnapPreviewWindow(this);
    connect(this, &FramelessWindow::windowMaximizedChanged, this, &MainWindow::updateMaximizeIcon);

    if (!qEnvironmentVariableIsSet("TEST_MODE")) {
        setupTrayIcon();
    }

    QSettings settings("SmartClock", "Theme");
    Theme saved = static_cast<Theme>(settings.value("theme", (int)Theme::Light).toInt());
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            qApp, &QApplication::setStyleSheet);
    ThemeManager::instance().applyTheme(saved);
    updateThemeIcon();
}

MainWindow::~MainWindow()
{
    if (snapPreview) {
        snapPreview->hidePreview();
        snapPreview->deleteLater();
    }
    delete ui;
}

void MainWindow::setupTrayIcon()
{
    trayIcon = new QSystemTrayIcon(QIcon(":/resources/icons/windowControl/iconsmartclockw.png"), this);
    trayIcon->setToolTip("Smart Clock");

    trayMenu = new QMenu(this);

    QAction *openTimer = new QAction(QIcon(":/resources/icons/timer.png"), " Timer", this);
    QAction *openStopwatch = new QAction(QIcon(":/resources/icons/stopwatch.png"), " Stopwatch", this);
    QAction *openAlarm = new QAction(QIcon(":/resources/icons/alarmclock.png"), " Alarm", this);
    QAction *openClock = new QAction(QIcon(":/resources/icons/clock.png"), " Clock", this);
    QAction *exitAction = new QAction(QIcon(":/resources/icons/exittrayb.png"), " Exit", this);

    openTimer->setToolTip("Show timer information");
    openStopwatch->setToolTip("Show stopwatch information");
    openAlarm->setToolTip("Show alarm information");
    openClock->setToolTip("Show clock information");
    exitAction->setToolTip("Exit Smart Clock");

    trayMenu->addAction(openTimer);
    trayMenu->addAction(openStopwatch);
    trayMenu->addAction(openAlarm);
    trayMenu->addAction(openClock);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    connect(openTimer, &QAction::triggered, this, [this]() { timerWindow->show(); });
    connect(openStopwatch, &QAction::triggered, this, [this]() { stopwatchWindow->show(); });
    connect(openAlarm, &QAction::triggered, this, [this]() { alarmWindow->show(); });
    connect(openClock, &QAction::triggered, this, [this]() { clockWindow->show(); });
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    connect(openTimer, &QAction::hovered, this, [this]() {
        QString info = QString(
                                 "<b>⏱ Timer</b><br>"
                                 "🟢 Running: <b>%1</b><br>"
                                 "⚪ Finished: <b>%2</b>"
                                 ).arg(QString::number(getRunningTimers()))
                                 .arg(QString::number(getFinishedTimers()));
        QToolTip::showText(QCursor::pos(), info);
    });

    connect(openStopwatch, &QAction::hovered, this, [this]() {
        QString info = QString(
                           "<b>⏲ Stopwatch</b><br>"
                           "🏁 Lap: <b>%1</b><br>"
                           "⏰ Total: <b>%2</b>"
                           ).arg(getStopwatchLapTime()).arg(getStopwatchTotalTime());
        QToolTip::showText(QCursor::pos(), info);
    });

    connect(openAlarm, &QAction::hovered, this, [this]() {
        QString info = QString(
                           "<b>⏰ Alarm</b><br>"
                           "Next: <b>%1</b>"
                           ).arg(getNextAlarmTime());
        QToolTip::showText(QCursor::pos(), info);
    });

    connect(openClock, &QAction::hovered, this, [this]() {
        QString info = QString(
                           "<b>🕒 Clock</b><br>"
                           "Current time: <b>%1</b>"
                           ).arg(QTime::currentTime().toString("hh:mm:ss"));
        QToolTip::showText(QCursor::pos(), info);
    });


    connect(exitAction, &QAction::hovered, this, []() {
        QToolTip::showText(QCursor::pos(), "Exit Smart Clock");
    });

    connect(trayMenu, &QMenu::aboutToHide, this, []() {
        QToolTip::hideText();
    });

    trayIcon->showMessage("🕓 Smart Clock",
                          "Application minimized to tray.\nRight-click for quick actions.",
                          QSystemTrayIcon::Information, 3000);

    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason){
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
            showNormal();
            raise();
            activateWindow();
        }
    });

    ui->btnMinimize->setIcon(QIcon(":/resources/icons/windowControl/minimizeL.png"));
    ui->btnClose->setIcon(QIcon(":/resources/icons/windowControl/closeL.png"));
}

void MainWindow::onTrayOpenTimer()
{
    showNormal();
    raise();
    activateWindow();
    ui->tabWidget->setCurrentWidget(timerWindow);
}

void MainWindow::onTrayOpenStopwatch()
{
    showNormal();
    raise();
    activateWindow();
    ui->tabWidget->setCurrentWidget(stopwatchWindow);
}

void MainWindow::onTrayOpenAlarm()
{
    showNormal();
    raise();
    activateWindow();
    ui->tabWidget->setCurrentWidget(alarmWindow);
}

void MainWindow::onTrayOpenClock()
{
    showNormal();
    raise();
    activateWindow();
    ui->tabWidget->setCurrentWidget(clockWindow);
}


void MainWindow::onTrayExit()
{
    qApp->quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    if (trayIcon) {
        trayIcon->showMessage("Smart Clock", "Application minimized to tray");
    }
    event->ignore();
}

int MainWindow::getRunningTimers() const
{
    return timerWindow ? timerWindow->getManager()->getFilteredTimers("Running").size() : 0;
}

int MainWindow::getFinishedTimers() const
{
    return timerWindow ? timerWindow->getManager()->getFilteredTimers("Finished").size() : 0;
}

QString MainWindow::getNextAlarmTime() const
{
    return alarmWindow ? alarmWindow->getNextAlarmString() : "No alarms";
}

QString MainWindow::getStopwatchLapTime() const
{
    return stopwatchWindow ? stopwatchWindow->getCurrentLapTimeString() : "—";
}

QString MainWindow::getStopwatchTotalTime() const
{
    return stopwatchWindow ? stopwatchWindow->getTotalTimeString() : "—";
}

void MainWindow::setupTitleBar() {
    ui->titleBar->setMinimumHeight(36);
    ui->titleBar->setMaximumHeight(36);

    connect(ui->btnClose,    &QPushButton::clicked, this, &QWidget::close);
    connect(ui->btnMinimize, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->btnMaximize, &QPushButton::clicked, this, &MainWindow::toggleMaximizeRestore);

    if (auto *v = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout())) {
        v->setContentsMargins(0,0,0,0);
        v->setSpacing(0);
    }
}

void MainWindow::updateMaximizeIcon(bool maxed) {
    Theme current = ThemeManager::instance().currentTheme();
    bool isLight = (current == Theme::Light) ? 1 : 0;
    QString path;

    if (maxed)
        path = isLight
                   ? ":/resources/icons/windowControl/minimize2L.png"
                   : ":/resources/icons/windowControl/minimize2D.png";
    else
        path = isLight
                   ? ":/resources/icons/windowControl/maximizeL.png"
                   : ":/resources/icons/windowControl/maximizeD.png";

    ui->btnMaximize->setIcon(QIcon(path));
}


void MainWindow::on_btnTheme_clicked()
{
    qDebug()<<"on_btnTheme_clicked";
    auto &tm = ThemeManager::instance();
    Theme current = tm.currentTheme();
    Theme newTheme = (current == Theme::Light) ? Theme::Dark : Theme::Light;

    tm.applyTheme(newTheme);
    updateThemeIcon();
    updateMaximizeIcon(this->isMaximized);

    QSettings settings("SmartClock", "Theme");
    settings.setValue("theme", (int)newTheme);
}

void MainWindow::updateThemeIcon()
{
    bool isLight = (ThemeManager::instance().currentTheme() == Theme::Light);

    ui->btnTheme->setIcon(QIcon(isLight
                                    ? ":/resources/icons/sunicon.png"
                                    : ":/resources/icons/moon.png"));
    ui->btnMinimize->setIcon(QIcon(isLight
                                       ? ":/resources/icons/windowControl/minimizeL.png"
                                       : ":/resources/icons/windowControl/minimizeD.png"));
    ui->btnMaximize->setIcon(QIcon(isLight
                                       ? ":/resources/icons/windowControl/maximizeL.png"
                                       : ":/resources/icons/windowControl/maximizeD.png"));
    ui->btnClose->setIcon(QIcon(isLight
                                    ? ":/resources/icons/windowControl/closeL.png"
                                    : ":/resources/icons/windowControl/closeD.png"));

    ui->IconClock->setPixmap(QPixmap(isLight
                                         ? ":/resources/icons/windowControl/iconsmartclockb.png"
                                         : ":/resources/icons/windowControl/iconsmartclockw.png"));

    auto btnAdd = timerWindow->findChild<QPushButton*>("btnAdd");
    if (btnAdd) {
        btnAdd->setIcon(QIcon(isLight
            ? ":/resources/icons/plusL.png"
            : ":/resources/icons/plusD.png"));
        btnAdd->setIconSize(QSize(24, 24));
    }

    auto btnSettings = timerWindow->findChild<QPushButton*>("btnSettings");
    if (btnSettings) {
        btnSettings->setIcon(QIcon(isLight
            ? ":/resources/icons/settingsL.png"
            : ":/resources/icons/settingsD.png"));
        btnSettings->setIconSize(QSize(24, 24));
    }

    ui->tabWidget->setTabIcon(0, QIcon(isLight
                                           ? ":/resources/icons/clock.png"
                                           : ":/resources/icons/clockD.png"));
    ui->tabWidget->setTabIcon(1, QIcon(isLight
                                           ? ":/resources/icons/alarmclock.png"
                                           : ":/resources/icons/alarmclockD.png"));
    ui->tabWidget->setTabIcon(2, QIcon(isLight
                                           ? ":/resources/icons/stopwatch.png"
                                           : ":/resources/icons/stopwatchD.png"));
    ui->tabWidget->setTabIcon(3, QIcon(isLight
                                           ? ":/resources/icons/timer.png"
                                           : ":/resources/icons/timerD.png"));

    ui->btnTheme->setIconSize(QSize(28, 28));
}
