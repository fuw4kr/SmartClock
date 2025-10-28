#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIcon>
#include <QTabBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Smart Clock");

    clockWindow     = new ClockWindow(this);
    alarmWindow    = new AlarmWindow(this);
    stopwatchWindow = new StopwatchWindow(this);
    timerWindow     = new TimerWindow(this);

    ui->tabWidget->clear();
    ui->tabWidget->addTab(clockWindow, QIcon(":/resources/icons/clock.png"),"Clock");
    ui->tabWidget->addTab(alarmWindow, QIcon(":/resources/icons/alarmclock.png"), "Alarms");
    ui->tabWidget->addTab(stopwatchWindow, QIcon(":/resources/icons/stopwatch.png"),"Stopwatch");
    ui->tabWidget->addTab(timerWindow,QIcon(":/resources/icons/timer.png"), "Timer");

    ui->tabWidget->tabBar()->setIconSize(QSize(20, 20));

    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}



