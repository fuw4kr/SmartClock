#include "alarmitemwidget.h"
#include "ui_alarmitemwidget.h"
#include <QIcon>

AlarmItemWidget::AlarmItemWidget(const AlarmData &data, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlarmItemWidget)
    , alarm(data)
{
    ui->setupUi(this);

    ui->labelTime->setText(alarm.time.toString("HH:mm"));
    ui->labelName->setText(alarm.name);
    ui->labelRepeat->setText(
        alarm.repeatMode == "Never" ? "Once" : alarm.repeatMode);

    QIcon icon(alarm.enabled
                   ? QIcon(":/resources/icons/toggle_on.png")
                   : QIcon(":/resources/icons/toggle_off.png"));
    ui->btnToggle->setIcon(icon);
    ui->btnToggle->setIconSize(QSize(32, 32));
    ui->btnToggle->setFlat(true);

    connect(ui->btnToggle, &QPushButton::clicked, this, [this]() {
        alarm.enabled = !alarm.enabled;

        QIcon icon(alarm.enabled
                       ? QIcon(":/resources/icons/toggle_on.png")
                       : QIcon(":/resources/icons/toggle_off.png"));
        ui->btnToggle->setIcon(icon);

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
    else if (alarm.repeatMode == "Specific days")
        repeatText = alarm.days.join(", ");
    else if (alarm.repeatMode == "Never")
        repeatText = "Once";

    ui->labelRepeat->setText(repeatText);

    QIcon icon(alarm.enabled
                   ? QIcon(":/resources/icons/toggle_on.png")
                   : QIcon(":/resources/icons/toggle_off.png"));
    ui->btnToggle->setIcon(icon);
}
