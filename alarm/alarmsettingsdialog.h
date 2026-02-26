
/**
 * @file alarmsettingsdialog.h
 * @brief Declarations for alarmsettingsdialog.
 * @details Defines types and functions related to alarmsettingsdialog.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ALARMSETTINGSDIALOG_H
#define ALARMSETTINGSDIALOG_H

#include <QDialog>
#include <QTime>
#include "alarmrepeatmode.h"

namespace Ui {
/**
 * @brief AlarmSettingsDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for AlarmSettings.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmSettingsDialog; /**< Alarm-related state. */
}

/**
 * @brief AlarmSettingsDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for AlarmSettings.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmSettingsDialog : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create AlarmSettingsDialog instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit AlarmSettingsDialog(QWidget *parent = nullptr);
/**
 * @brief Destroy AlarmSettingsDialog instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~AlarmSettingsDialog();

/**
 * @brief Get time.
 * @details Returns the current value derived from internal state.
 * @return Elapsed time as QTime.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QTime getTime() const;
/**
 * @brief Get label.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getLabel() const;
/**
 * @brief Get repeat mode.
 * @details Returns the current value derived from internal state.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    RepeatMode getRepeatMode() const;
/**
 * @brief Get days.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QStringList getDays() const;
/**
 * @brief Get sound path.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getSoundPath() const;
/**
 * @brief Get snooze.
 * @details Returns the current value derived from internal state.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool getSnooze() const;

private slots:
/**
 * @brief On browse sound.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onBrowseSound();
/**
 * @brief Emitted when on repeat changes.
 * @details Signal emitted when the associated state changes.
 * @param text Text value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onRepeatChanged(const QString &text);

private:
    Ui::AlarmSettingsDialog *ui;
};

#endif // ALARMSETTINGSDIALOG_H
