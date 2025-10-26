#include "timereditdialog.h"
#include "ui_timereditdialog.h"
#include <QPushButton>
#include <QMessageBox>

TimerEditDialog::TimerEditDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TimerEditDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (getName().isEmpty()) {
            QMessageBox::warning(this, "Invalid Name", "Please enter a timer name.");
            return;
        }
        if (getTotalSeconds() <= 0) {
            QMessageBox::warning(this, "Invalid Time", "Please enter a duration greater than 0.");
            return;
        }
        accept(); // ✅ закриваємо діалог тільки якщо все добре

    setWindowTitle("Add Timer");

    ui->spinHours->setRange(0, 23);
    ui->spinMinutes->setRange(0, 59);
    ui->spinSeconds->setRange(0, 59);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TimerEditDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &TimerEditDialog::reject);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setShortcut(Qt::Key_Return);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setShortcut(Qt::Key_Escape);
}
);}

TimerEditDialog::~TimerEditDialog()
{
    delete ui;
}

QString TimerEditDialog::getName() const
{
    return ui->nameEdit->text();
}

int TimerEditDialog::getTotalSeconds() const
{
    int hours = ui->spinHours->value();
    int minutes = ui->spinMinutes->value();
    int seconds = ui->spinSeconds->value();
    return hours * 3600 + minutes * 60 + seconds;
}
