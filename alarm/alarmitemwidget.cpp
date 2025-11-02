#include "alarmitemwidget.h"
#include "ui_alarmitemwidget.h"
#include <QIcon>
#include <QDebug>
#include <QEvent>
#include <QStyle>

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

    QString repeatText;
    if (alarm.repeatMode == "Every day")
        repeatText = "Every day";
    else if (alarm.repeatMode == "Weekdays")
        repeatText = "Weekdays";
    else if (alarm.repeatMode == "Weekends")
        repeatText = "Weekends";
    else if (alarm.repeatMode == "Specific days" || alarm.repeatMode == "Specific Days") {
        if (!alarm.days.isEmpty())
            repeatText = alarm.days.join(", ");
        else
            repeatText = "Once";
    }
    else if (alarm.repeatMode == "Never" || alarm.repeatMode == "Once")
        repeatText = "Once";

    ui->labelRepeat->setText(repeatText);

    QIcon icon(alarm.enabled
                   ? QIcon(":/resources/icons/toggle_on.png")
                   : QIcon(":/resources/icons/toggle_off.png"));
    ui->btnToggle->setIcon(icon);
    qDebug() << "RepeatMode:" << alarm.repeatMode << "Days:" << alarm.days;

}

bool AlarmItemWidget::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::Enter:
    case QEvent::HoverEnter:
        setProperty("hovered", true);
        style()->unpolish(this); style()->polish(this); update();
        break;
    case QEvent::Leave:
    case QEvent::HoverLeave:
        setProperty("hovered", false);
        style()->unpolish(this); style()->polish(this); update();
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

bool AlarmItemWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::Enter || e->type() == QEvent::HoverEnter) {
        setProperty("hovered", true);
        style()->unpolish(this); style()->polish(this); update();
    } else if (e->type() == QEvent::Leave || e->type() == QEvent::HoverLeave) {
        setProperty("hovered", false);
        style()->unpolish(this); style()->polish(this); update();
    }
    return QWidget::eventFilter(obj, e);
}
