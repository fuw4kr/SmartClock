#include "stopwatchwindow.h"
#include "ui_stopwatchwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QSettings>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStackedWidget>

StopwatchWindow::StopwatchWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StopwatchWindow)
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    setWindowTitle("Stopwatch");

    ui->btnLap->setEnabled(false);
    ui->listLaps->setVisible(false);
    ui->labelTime->setText("00:00.00");

    elapsed = QTime(0, 0);
    connect(timer, &QTimer::timeout, this, &StopwatchWindow::updateDisplay);
    connect(ui->btnStartStop, &QPushButton::clicked, this, &StopwatchWindow::onStartStopClicked);
    connect(ui->btnLap, &QPushButton::clicked, this, &StopwatchWindow::onLapClicked);

    analogDial = new AnalogStopwatchDial(this);
    analogDial->setFixedSize(250, 250);

    stackedView = new QStackedWidget(this);
    stackedView->addWidget(ui->labelTime);
    stackedView->addWidget(analogDial);

    QVBoxLayout *layoutMain = qobject_cast<QVBoxLayout*>(this->layout());
    if (!layoutMain) {
        layoutMain = new QVBoxLayout(this);
        setLayout(layoutMain);
    }

    layoutMain->insertWidget(0, stackedView, 0, Qt::AlignCenter);

    QPushButton *btnSwitch = new QPushButton(this);
    btnSwitch->setIcon(QIcon(":/resources/icons/datatransfer.png"));
    btnSwitch->setIconSize(QSize(24, 24));
    btnSwitch->setFixedSize(32, 32);
    btnSwitch->setToolTip("Switch stopwatch view");
    btnSwitch->setStyleSheet("QPushButton { border: none; } QPushButton:hover { background-color: #f0f0f0; border-radius: 6px; }");

    layoutMain->insertWidget(1, btnSwitch, 0, Qt::AlignHCenter);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(ui->btnLap);
    buttonsLayout->addWidget(ui->btnStartStop);
    layoutMain->addLayout(buttonsLayout);
    layoutMain->addWidget(ui->listLaps);

    QSettings settings("SmartClockApp", "Stopwatch");
    analogMode = settings.value("analogMode", false).toBool();
    stackedView->setCurrentIndex(analogMode ? 1 : 0);
    if (analogMode)
        analogDial->setElapsed(elapsed);

    QGraphicsOpacityEffect *fadeEffect = new QGraphicsOpacityEffect(this);
    stackedView->setGraphicsEffect(fadeEffect);

    connect(btnSwitch, &QPushButton::clicked, this, [=]() mutable {
        analogMode = !analogMode;

        QPropertyAnimation *fadeOut = new QPropertyAnimation(fadeEffect, "opacity");
        fadeOut->setDuration(200);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);

        QPropertyAnimation *fadeIn = new QPropertyAnimation(fadeEffect, "opacity");
        fadeIn->setDuration(200);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);

        connect(fadeOut, &QPropertyAnimation::finished, this, [=]() mutable {
            stackedView->setCurrentIndex(analogMode ? 1 : 0);
            if (analogMode)
                analogDial->setElapsed(elapsed);
            fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
        });

        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);

        QSettings settings("SmartClockApp", "Stopwatch");
        settings.setValue("analogMode", analogMode);
    });

    loadFromFile();
    connect(qApp, &QCoreApplication::aboutToQuit, this, &StopwatchWindow::saveToFile);
}

StopwatchWindow::~StopwatchWindow()
{
    saveToFile();
    delete ui;
}

void StopwatchWindow::onStartStopClicked()
{
    if (!running) {
        timer->start(10);
        running = true;
        ui->btnStartStop->setText("Stop");
        ui->btnLap->setEnabled(true);
        ui->btnLap->setText("Lap");
    } else {
        timer->stop();
        running = false;
        ui->btnStartStop->setText("Start");
        ui->btnLap->setText("Reset");
    }
    saveToFile();
}

void StopwatchWindow::updateDisplay()
{
    elapsed = elapsed.addMSecs(10);
    ui->labelTime->setText(elapsed.toString("mm:ss.zzz").left(8));
    if (analogMode)
        analogDial->setElapsed(elapsed);
}

void StopwatchWindow::onLapClicked()
{
    if (running) {
        QTime nowLap = elapsed;
        int nowMs = nowLap.msecsSinceStartOfDay();
        int lastMs = lapTimes.isEmpty() ? 0 : lapTimes.last().msecsSinceStartOfDay();
        int segMs = nowMs - lastMs;

        QString lapTimeStr = nowLap.toString("mm:ss.zzz").left(8);
        QString deltaStr = QTime(0, 0).addMSecs(segMs).toString("mm:ss.zzz").left(8);
        int lapNumber = lapTimes.size() + 1;
        QString text = QString("Lap %1: %2 (+%3)").arg(lapNumber).arg(lapTimeStr).arg(deltaStr);

        lapTimes.append(nowLap);
        lapDurations.append(segMs);
        lapTexts.append(text);

        ui->listLaps->insertItem(0, text);
        ui->listLaps->setVisible(true);

        updateLapColors();
    } else {
        resetStopwatch();
    }

    saveToFile();
}

void StopwatchWindow::updateLapColors()
{
    if (lapDurations.isEmpty())
        return;

    int minIndex = 0, maxIndex = 0;
    int minVal = lapDurations[0], maxVal = lapDurations[0];

    for (int i = 1; i < lapDurations.size(); ++i) {
        if (lapDurations[i] < minVal) { minVal = lapDurations[i]; minIndex = i; }
        if (lapDurations[i] > maxVal) { maxVal = lapDurations[i]; maxIndex = i; }
    }

    for (int i = 0; i < ui->listLaps->count(); ++i)
        ui->listLaps->item(i)->setBackground(Qt::NoBrush);

    int count = ui->listLaps->count();
    int invMin = count - 1 - minIndex;
    int invMax = count - 1 - maxIndex;

    if (invMin >= 0 && invMin < count)
        ui->listLaps->item(invMin)->setBackground(QBrush(QColor(170, 255, 170)));

    if (invMax >= 0 && invMax < count)
        ui->listLaps->item(invMax)->setBackground(QBrush(QColor(255, 170, 170)));
}

void StopwatchWindow::resetStopwatch()
{
    timer->stop();
    running = false;
    elapsed = QTime(0, 0);
    lapTimes.clear();
    lapTexts.clear();
    lapDurations.clear();

    ui->labelTime->setText("00:00.00");
    ui->listLaps->clear();
    ui->listLaps->setVisible(false);

    ui->btnStartStop->setText("Start");
    ui->btnLap->setText("Lap");
    ui->btnLap->setEnabled(false);

    saveToFile();
}

QString StopwatchWindow::filePath() const
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    return base + "/stopwatch.json";
}

void StopwatchWindow::saveToFile() const
{
    QJsonObject obj;
    obj["elapsed_ms"] = elapsed.msecsSinceStartOfDay();
    obj["running"] = running;

    QJsonArray lapsArr;
    for (const QString &s : lapTexts)
        lapsArr.append(s);
    obj["laps"] = lapsArr;

    QJsonArray durArr;
    for (int d : lapDurations)
        durArr.append(d);
    obj["durations"] = durArr;

    QFile f(filePath());
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));
        f.close();
    }
}

void StopwatchWindow::loadFromFile()
{
    QFile f(filePath());
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    if (!doc.isObject())
        return;

    QJsonObject obj = doc.object();
    int ms = obj["elapsed_ms"].toInt(0);
    running = obj["running"].toBool(false);
    elapsed = QTime(0, 0).addMSecs(ms);
    ui->labelTime->setText(elapsed.toString("mm:ss.zzz").left(8));

    lapTexts.clear();
    lapTimes.clear();
    lapDurations.clear();
    ui->listLaps->clear();

    QJsonArray lapsArr = obj["laps"].toArray();
    QJsonArray durArr = obj["durations"].toArray();

    for (int i = 0; i < lapsArr.size(); ++i) {
        QString text = lapsArr[i].toString();
        ui->listLaps->insertItem(0, text);
        lapTexts.append(text);
        if (i < durArr.size()) lapDurations.append(durArr[i].toInt());
    }

    if (!lapTexts.isEmpty())
        ui->listLaps->setVisible(true);

    ui->btnLap->setEnabled(running || !lapTexts.isEmpty());
    ui->btnLap->setText(running ? "Lap" : "Reset");
    ui->btnStartStop->setText(running ? "Stop" : "Start");

    if (running)
        timer->start(10);

    updateLapColors();
}
