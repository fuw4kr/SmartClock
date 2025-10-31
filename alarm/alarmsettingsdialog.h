#ifndef ALARMSETTINGSDIALOG_H
#define ALARMSETTINGSDIALOG_H

#include <QDialog>
#include <QTime>

namespace Ui {
class AlarmSettingsDialog;
}

class AlarmSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmSettingsDialog(QWidget *parent = nullptr);
    ~AlarmSettingsDialog();

    QTime getTime() const;
    QString getLabel() const;
    QString getRepeatMode() const;
    QStringList getDays() const;
    QString getSoundPath() const;
    bool getSnooze() const;

private slots:
    void onBrowseSound();
    void onRepeatChanged(const QString &text);

private:
    Ui::AlarmSettingsDialog *ui;
};

#endif // ALARMSETTINGSDIALOG_H
