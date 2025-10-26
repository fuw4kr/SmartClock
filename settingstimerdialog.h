#ifndef SETTINGSTIMERDIALOG_H
#define SETTINGSTIMERDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsTimerDialog;
}

class SettingsTimerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsTimerDialog(QWidget *parent = nullptr);
    ~SettingsTimerDialog();

    bool isSoundEnabled() const;
    bool isActionEnabled() const;
    QString getActionPath() const;

    void setSoundEnabled(bool enabled);
    void setActionEnabled(bool enabled);
    void setActionPath(const QString &path);

    bool continueAfterExit() const;
    void setContinueAfterExit(bool enabled);

private slots:
    void onBrowseClicked();

private:
    Ui::SettingsTimerDialog *ui;
};

#endif // SETTINGSTIMERDIALOG_H
