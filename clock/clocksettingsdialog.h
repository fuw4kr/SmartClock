#ifndef CLOCKSETTINGSDIALOG_H
#define CLOCKSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class ClockSettingsDialog;
}

class ClockSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClockSettingsDialog(QWidget *parent = nullptr);
    ~ClockSettingsDialog();

    QString getSelectedZone() const;

private:
    Ui::ClockSettingsDialog *ui;
};

#endif // CLOCKSETTINGSDIALOG_H
