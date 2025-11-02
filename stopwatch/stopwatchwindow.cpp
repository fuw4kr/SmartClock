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

#include <QStyledItemDelegate>
#include <QPainter>

namespace {
enum { RoleLapFlag = Qt::UserRole + 100 };
}

class LapHighlightDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *p, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        const QWidget *w = opt.widget;
        const bool isLight =
            (w ? w->palette().color(QPalette::Window).lightness() > 180 : true);

        const QColor good = isLight ? QColor(182,235,195,255) : QColor(70,130,90,180);
        const QColor bad  = isLight ? QColor(247,196,192,255) : QColor(130,60,60,180);

        const int flag = index.data(RoleLapFlag).toInt();

        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);

        QRect r = opt.rect.adjusted(2, 2, -2, -2);
        if (flag == 1) {
            p->setBrush(good);
            p->setPen(Qt::NoPen);
            p->drawRoundedRect(r, 5, 5);
        } else if (flag == 2) {
            p->setBrush(bad);
            p->setPen(Qt::NoPen);
            p->drawRoundedRect(r, 5, 5);
        }

        if (opt.state.testFlag(QStyle::State_Selected)) {
            QColor sel = isLight ? QColor(213, 41, 65, 120) : QColor(213,41,65,160);
            p->setBrush(sel);
            p->setPen(Qt::NoPen);
            p->drawRoundedRect(r, 5, 5);
        }

        p->restore();

        QStyleOptionViewItem clean = opt;
        clean.backgroundBrush = Qt::NoBrush;
        QStyledItemDelegate::paint(p, clean, index);
    }
};

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
    ui->listLaps->setItemDelegate(new LapHighlightDelegate(ui->listLaps));

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
        const QTime nowLap = elapsed;

        int segMs = 0;
        if (!lapTimes.isEmpty())
            segMs = lapTimes.last().msecsTo(nowLap);
        else
            segMs = elapsed.msecsSinceStartOfDay();

        const QString lapTimeStr = nowLap.toString("mm:ss.zzz").left(8);
        const QString deltaStr   = QTime(0, 0).addMSecs(segMs).toString("mm:ss.zzz").left(8);
        const int lapNumber = lapTimes.size() + 1;
        const QString text = QString("Lap %1: %2 (+%3)").arg(lapNumber).arg(lapTimeStr).arg(deltaStr);

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

    for (int i = 0; i < ui->listLaps->count(); ++i) {
        if (auto *it = ui->listLaps->item(i)) {
            it->setData(RoleLapFlag, 0);
        }
    }

    const int count = ui->listLaps->count();
    const int invMin = count - 1 - minIndex;
    const int invMax = count - 1 - maxIndex;

    if (invMin >= 0 && invMin < count)
        ui->listLaps->item(invMin)->setData(RoleLapFlag, 1);
    if (invMax >= 0 && invMax < count)
        ui->listLaps->item(invMax)->setData(RoleLapFlag, 2);
    ui->listLaps->viewport()->update();
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

    if (!running && elapsed == QTime(0, 0)) {
        QFile f(filePath());
        f.remove();
        return;
    }

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

    lapTimes.clear();
    int accMs = 0;
    for (int d : lapDurations) {
        accMs += d;
        lapTimes.append(QTime(0, 0).addMSecs(accMs));
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

QString StopwatchWindow::getCurrentLapTimeString() const
{
    if (!lapDurations.isEmpty()) {
        const int segMs = lapDurations.last();
        return QTime(0,0).addMSecs(segMs).toString("mm:ss.zzz").left(8);
    }
    return "—";
}

QString StopwatchWindow::getTotalTimeString() const
{
    return ui && ui->labelTime ? ui->labelTime->text() : "—";
}
