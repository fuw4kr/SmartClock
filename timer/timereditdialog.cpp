#include "timereditdialog.h"
#include "ui_timereditdialog.h"
#include <QPushButton>
#include <QMessageBox>
#include <QDateTime>
#include <QComboBox>
#include <QPropertyAnimation>
#include <QEasingCurve>

TimerEditDialog::TimerEditDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TimerEditDialog)
{
    ui->setupUi(this);
    setWindowTitle("Add Timer");

    ui->comboType->clear();
    ui->comboType->addItems({"Normal", "Melody", "Reminder"});

    ui->spinHours->setRange(0, 23);
    ui->spinMinutes->setRange(0, 59);
    ui->spinSeconds->setRange(0, 59);

    ui->targetTimeEdit->setVisible(false);
    ui->radioDuration->setChecked(true);

    ui->targetTimeEdit->setTime(QTime::currentTime());

    connect(ui->radioDuration, &QRadioButton::toggled, this, &TimerEditDialog::updateInputMode);
    connect(ui->radioTargetTime, &QRadioButton::toggled, this, &TimerEditDialog::updateInputMode);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (getName().isEmpty()) {
            QMessageBox::warning(this, "Invalid Name", "Please enter a timer name.");
            return;
        }
        if (getTotalSeconds() <= 0) {
            QMessageBox::warning(this, "Invalid Time", "Please enter a valid duration or future time.");
            return;
        }

        emit timerAdded(getName(), getTotalSeconds(), getType(), getGroup());
        accept();
    });

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &TimerEditDialog::reject);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::Key_Return);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setShortcut(Qt::Key_Escape);

    ui->comboGroup->clear();
    ui->comboGroup->addItems({"Default", "Study", "Work", "Rest", "Custom"});
}

TimerEditDialog::~TimerEditDialog()
{
    delete ui;
}

void TimerEditDialog::updateInputMode()
{
    bool durationMode = ui->radioDuration->isChecked();

    if (durationMode) {
        auto *fadeOut = new QPropertyAnimation(ui->targetTimeEdit, "maximumHeight");
        fadeOut->setDuration(200);
        fadeOut->setStartValue(ui->targetTimeEdit->height());
        fadeOut->setEndValue(0);
        fadeOut->setEasingCurve(QEasingCurve::InOutCubic);

        connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
            ui->targetTimeEdit->setVisible(false);
            ui->widget->setVisible(true);

            auto *anim = new QPropertyAnimation(ui->widget, "maximumHeight");
            anim->setDuration(250);
            anim->setStartValue(0);
            anim->setEndValue(ui->widget->sizeHint().height());
            anim->setEasingCurve(QEasingCurve::InOutCubic);
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        });

        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else {
        auto *anim = new QPropertyAnimation(ui->widget, "maximumHeight");
        anim->setDuration(200);
        anim->setStartValue(ui->widget->height());
        anim->setEndValue(0);
        anim->setEasingCurve(QEasingCurve::InOutCubic);

        connect(anim, &QPropertyAnimation::finished, this, [this]() {
            ui->widget->setVisible(false);
            ui->targetTimeEdit->setVisible(true);

            auto *fadeIn = new QPropertyAnimation(ui->targetTimeEdit, "maximumHeight");
            fadeIn->setDuration(250);
            fadeIn->setStartValue(0);
            fadeIn->setEndValue(ui->targetTimeEdit->sizeHint().height());
            fadeIn->setEasingCurve(QEasingCurve::InOutCubic);
            fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
        });

        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

QString TimerEditDialog::getName() const
{
    return ui->nameEdit->text();
}

int TimerEditDialog::getTotalSeconds() const
{
    if (ui->radioDuration->isChecked()) {
        int hours = ui->spinHours->value();
        int minutes = ui->spinMinutes->value();
        int seconds = ui->spinSeconds->value();
        return hours * 3600 + minutes * 60 + seconds;
    } else {
        QDateTime now = QDateTime::currentDateTime();

        QTime selectedTime = ui->targetTimeEdit->time();
        QDateTime target(now.date(), selectedTime);

        if (target <= now)
            target = target.addDays(1);

        return now.secsTo(target);
    }
}

QString TimerEditDialog::getType() const
{
    return ui->comboType->currentText();
}

QString TimerEditDialog::getGroup() const
{
    return ui->comboGroup->currentText();
}
