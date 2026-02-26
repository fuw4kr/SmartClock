#include "alarmsettingsdialog.h"
#include "ui_alarmsettingsdialog.h"
#include <QFileDialog>

static const char* kDefaultSound = "qrc:/s/resources/sounds/soundalarm.wav";

AlarmSettingsDialog::AlarmSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AlarmSettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Add Alarm");

    ui->comboRepeat->addItems({"Never", "Every day", "Weekdays", "Weekends", "Specific Days"});
    ui->groupDays->setVisible(false);

    ui->comboSound->addItem("Default sound");

    connect(ui->comboRepeat, &QComboBox::currentTextChanged, this, &AlarmSettingsDialog::onRepeatChanged);
    connect(ui->btnBrowseSound, &QPushButton::clicked, this, &AlarmSettingsDialog::onBrowseSound);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

AlarmSettingsDialog::~AlarmSettingsDialog()
{
    delete ui;
}

QTime AlarmSettingsDialog::getTime() const
{
    return ui->timeEdit->time();
}

QString AlarmSettingsDialog::getLabel() const
{
    QString text = ui->lineLabel->text().trimmed();
    return text.isEmpty() ? "Alarm" : text;
}

RepeatMode AlarmSettingsDialog::getRepeatMode() const
{
    return repeatModeFromString(ui->comboRepeat->currentText());
}

QStringList AlarmSettingsDialog::getDays() const
{
    struct DayCheck {
        QCheckBox *box;
        const char *label;
    };

    const DayCheck items[] = {
        {ui->chkMon, "Mon"},
        {ui->chkTue, "Tue"},
        {ui->chkWed, "Wed"},
        {ui->chkThu, "Thu"},
        {ui->chkFri, "Fri"},
        {ui->chkSat, "Sat"},
        {ui->chkSun, "Sun"},
    };

    QStringList days;
    for (const auto &item : items) {
        if (item.box->isChecked())
            days << item.label;
    }
    return days;
}

QString AlarmSettingsDialog::getSoundPath() const
{
    if (ui->comboSound->currentText() == "Default sound")
        return QString::fromUtf8(kDefaultSound);
    return ui->comboSound->currentText();
}

bool AlarmSettingsDialog::getSnooze() const
{
    return ui->checkSnooze->isChecked();
}

void AlarmSettingsDialog::onBrowseSound()
{
    QString file = QFileDialog::getOpenFileName(this, "Select sound", "", "Audio Files (*.wav *.mp3)");
    if (!file.isEmpty()) {
        int idx = ui->comboSound->findText(file);
        if (idx < 0) {
            ui->comboSound->addItem(file);
            idx = ui->comboSound->count() - 1;
        }
        ui->comboSound->setCurrentIndex(idx);
    }
}

void AlarmSettingsDialog::onRepeatChanged(const QString &text)
{
    ui->groupDays->setVisible(repeatModeFromString(text) == RepeatMode::SpecificDays);
}
