#include "settingstimerdialog.h"
#include "ui_settingstimerdialog.h"
#include <QFileDialog>


SettingsTimerDialog::SettingsTimerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsTimerDialog)
{
    ui->setupUi(this);
    setWindowTitle("Timer Settings");

    // ховаємо панель вибору файлу під час запуску
    ui->widgetFileSelect->setVisible(false);

    // показуємо/ховаємо залежно від стану чекбоксу
    connect(ui->checkAction, &QCheckBox::toggled,
            ui->widgetFileSelect, &QWidget::setVisible);

    connect(ui->btnBrowse, &QPushButton::clicked, this, &SettingsTimerDialog::onBrowseClicked);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsTimerDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingsTimerDialog::reject);
}

SettingsTimerDialog::~SettingsTimerDialog()
{
    delete ui;
}

void SettingsTimerDialog::onBrowseClicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select file or program to run");
    if (!file.isEmpty())
        ui->lineEditPath->setText(file);
}

bool SettingsTimerDialog::isSoundEnabled() const
{
    return ui->checkSound->isChecked();
}

bool SettingsTimerDialog::isActionEnabled() const
{
    return ui->checkAction->isChecked();
}

QString SettingsTimerDialog::getActionPath() const
{
    return ui->lineEditPath->text();
}

void SettingsTimerDialog::setSoundEnabled(bool enabled)
{
    ui->checkSound->setChecked(enabled);
}

void SettingsTimerDialog::setActionEnabled(bool enabled)
{
    ui->checkAction->setChecked(enabled);
    ui->widgetFileSelect->setVisible(enabled);
}

void SettingsTimerDialog::setActionPath(const QString &path)
{
    ui->lineEditPath->setText(path);
}


bool SettingsTimerDialog::continueAfterExit() const
{
    return ui->checkContinueOnExit->isChecked();
}

void SettingsTimerDialog::setContinueAfterExit(bool enabled)
{
    ui->checkContinueOnExit->setChecked(enabled);
}
