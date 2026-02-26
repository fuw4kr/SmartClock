/**
 * @file stopwatchwindow.cpp
 * @brief Definitions for stopwatchwindow.
 * @details Implements logic declared in the corresponding header for stopwatchwindow.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "stopwatchwindow.h"
#include "ui_stopwatchwindow.h"
#include <QSettings>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QDebug>

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
    , model(new StopwatchModel(this))
{
    ui->setupUi(this);
    setWindowTitle("Stopwatch");

    ui->btnLap->setEnabled(false);
    ui->listLaps->setVisible(false);
    ui->labelTime->setText("00:00.00");

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
        analogDial->setElapsed(model->elapsedTime());

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
                analogDial->setElapsed(model->elapsedTime());
            fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
        });

        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);

        QSettings settings("SmartClockApp", "Stopwatch");
        settings.setValue("analogMode", analogMode);
    });

    controller = new StopwatchController(model, this, this);
    ui->listLaps->setItemDelegate(new LapHighlightDelegate(ui->listLaps));
}

StopwatchWindow::~StopwatchWindow()
{
    delete ui;
}

void StopwatchWindow::syncFromModel()
{
    ui->labelTime->setText(model->formattedElapsed());
    if (analogMode)
        analogDial->setElapsed(model->elapsedTime());

    ui->listLaps->clear();
    const QStringList texts = model->lapTexts();
    for (const QString &text : texts)
        ui->listLaps->insertItem(0, text);
    ui->listLaps->setVisible(!texts.isEmpty());

    const bool hasData = model->elapsedMs() > 0 || !model->lapDurations().isEmpty();
    ui->btnLap->setEnabled(model->isRunning() || hasData);
    ui->btnLap->setText(model->isRunning() ? "Lap" : (hasData ? "Reset" : "Lap"));
    ui->btnStartStop->setText(model->isRunning() ? "Stop" : "Start");

    if (model->isRunning()) {
        if (!timer->isActive())
            timer->start(10);
    } else if (timer->isActive()) {
        timer->stop();
    }

    updateLapColors();
}

void StopwatchWindow::onStartStopClicked()
{
    emit startStopRequested();
}

void StopwatchWindow::updateDisplay()
{
    model->tick(10);
    ui->labelTime->setText(model->formattedElapsed());
    if (analogMode)
        analogDial->setElapsed(model->elapsedTime());
}

void StopwatchWindow::onLapClicked()
{
    emit lapRequested();
}

void StopwatchWindow::updateLapColors()
{
    const auto &lapDurations = model->lapDurations();
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

QString StopwatchWindow::getCurrentLapTimeString() const
{
    const auto &lapDurations = model->lapDurations();
    if (!lapDurations.isEmpty()) {
        const int segMs = lapDurations.last();
        return QTime(0,0).addMSecs(segMs).toString("mm:ss.zzz").left(8);
    }
    return "-";
}

QString StopwatchWindow::getTotalTimeString() const
{
    return ui && ui->labelTime ? ui->labelTime->text() : "-";
}
