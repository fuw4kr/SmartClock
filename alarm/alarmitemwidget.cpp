/**
 * @file alarmitemwidget.cpp
 * @brief Definitions for alarmitemwidget.
 * @details Implements logic declared in the corresponding header for alarmitemwidget.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "alarmitemwidget.h"
#include "ui_alarmitemwidget.h"
#include <QIcon>
#include <QDebug>
#include <QEvent>
#include <QStyle>

namespace {
QString repeatTextFor(const AlarmData &alarm)
{
    const RepeatMode repeat = alarm.repeatMode;
    if (repeat == RepeatMode::SpecificDays)
        return alarm.days.isEmpty() ? "Once" : alarm.days.join(", ");
    if (repeat == RepeatMode::Never || repeat == RepeatMode::Once)
        return "Once";

    return repeatModeToString(repeat);
}
} // namespace

AlarmItemWidget::AlarmItemWidget(const AlarmData &data, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlarmItemWidget)
    , alarm(data)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);

    const auto children = findChildren<QWidget*>();
    for (auto *w : children) {
        w->setMouseTracking(true);
        w->installEventFilter(this);
    }

    setProperty("hovered", false);
    setAlarmData(alarm);

    ui->btnToggle->setIconSize(QSize(32, 32));
    ui->btnToggle->setFlat(true);

    connect(ui->btnToggle, &QPushButton::clicked, this, [this]() {
        alarm.enabled = !alarm.enabled;
        ui->btnToggle->setIcon(QIcon(alarm.enabled
                                         ? ":/resources/icons/toggle_on.png"
                                         : ":/resources/icons/toggle_off.png"));
        emit toggled(alarm.enabled);
    });
}

AlarmItemWidget::~AlarmItemWidget()
{
    delete ui;
}

void AlarmItemWidget::setAlarmData(const AlarmData &data)
{
    alarm = data;

    ui->labelTime->setText(alarm.time.toString("HH:mm"));
    ui->labelName->setText(alarm.name.isEmpty() ? "Alarm" : alarm.name);

    ui->labelRepeat->setText(repeatTextFor(alarm));

    QIcon icon(alarm.enabled
                   ? QIcon(":/resources/icons/toggle_on.png")
                   : QIcon(":/resources/icons/toggle_off.png"));
    ui->btnToggle->setIcon(icon);
    qDebug() << "RepeatMode:" << repeatModeToString(alarm.repeatMode) << "Days:" << alarm.days;

}

void AlarmItemWidget::setHovered(bool hovered)
{
    setProperty("hovered", hovered);
    style()->unpolish(this);
    style()->polish(this);
    update();
}

bool AlarmItemWidget::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::Enter:
    case QEvent::HoverEnter:
        setHovered(true);
        break;
    case QEvent::Leave:
    case QEvent::HoverLeave:
        setHovered(false);
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

bool AlarmItemWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::Enter || e->type() == QEvent::HoverEnter) {
        setHovered(true);
    } else if (e->type() == QEvent::Leave || e->type() == QEvent::HoverLeave) {
        setHovered(false);
    }
    return QWidget::eventFilter(obj, e);
}
