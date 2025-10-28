#ifndef SETTINGSTIMERDIALOG_H
#define SETTINGSTIMERDIALOG_H

#include <QDialog>
#include "timermanager.h"

namespace Ui {
class SettingsTimerDialog;
}

class SettingsTimerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsTimerDialog(TimerManager *manager, QWidget *parent = nullptr);
    ~SettingsTimerDialog();

    bool isSoundEnabled() const;
    bool isActionEnabled() const;
    QString getActionPath() const;

    void setSoundEnabled(bool enabled);
    void setActionEnabled(bool enabled);
    void setActionPath(const QString &path);

    bool continueAfterExit() const;
    void setContinueAfterExit(bool enabled);

    QString melodySoundPath() const;
    QString reminderSoundPath() const;
    void setMelodySoundPath(const QString &path);
    void setReminderSoundPath(const QString &path);

    bool isRecommendationsEnabled() const;
    void setRecommendationsEnabled(bool enabled);

private slots:
    void onBrowseClicked();

    void onAddRecommendationClicked();
    void onDeleteRecommendationClicked();

private:
    Ui::SettingsTimerDialog *ui;
    TimerManager *m_manager;
    QString m_melodySoundPath;
    QString m_reminderSoundPath;

    void animateWidget(QWidget *widget, bool expand);
    void refreshRecommendationTable();
};

#endif // SETTINGSTIMERDIALOG_H
