#include "clocksettingsdialog.h"
#include "ui_clocksettingsdialog.h"
#include <QTimeZone>
#include <QMessageBox>

ClockSettingsDialog::ClockSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ClockSettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Add Clock");

    const auto zones = QTimeZone::availableTimeZoneIds();
    for (const QByteArray &z : zones)
        ui->comboZone->addItem(QString::fromUtf8(z), z);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (ui->comboZone->currentIndex() < 0) {
            QMessageBox::warning(this, "Select Zone", "Please select a time zone before proceeding.");
            return;
        }
        accept();
    });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ClockSettingsDialog::reject);
}

ClockSettingsDialog::~ClockSettingsDialog()
{
    delete ui;
}

QString ClockSettingsDialog::getSelectedZone() const
{
    int idx = ui->comboZone->currentIndex();
    if (idx < 0)
        return QString();
    return ui->comboZone->currentData().toString();
}
