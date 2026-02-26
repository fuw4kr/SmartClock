
/**
 * @file settingstimerdialog.h
 * @brief Declarations for settingstimerdialog.
 * @details Defines types and functions related to settingstimerdialog.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef SETTINGSTIMERDIALOG_H
#define SETTINGSTIMERDIALOG_H

#include <QDialog>
#include "timermanager.h"

namespace Ui {
/**
 * @brief SettingsTimerDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for SettingsTimer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class SettingsTimerDialog; /**< Timer-related state. */
}

/**
 * @brief SettingsTimerDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for SettingsTimer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class SettingsTimerDialog : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create SettingsTimerDialog instance.
 * @details Initializes instance state.
 * @param manager manager value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit SettingsTimerDialog(TimerManager *manager, QWidget *parent = nullptr);
/**
 * @brief Destroy SettingsTimerDialog instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~SettingsTimerDialog();

/**
 * @brief Check whether sound enabled.
 * @details Returns the current value derived from internal state.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool isSoundEnabled() const;
/**
 * @brief Check whether action enabled.
 * @details Returns the current value derived from internal state.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool isActionEnabled() const;
/**
 * @brief Get action path.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getActionPath() const;

/**
 * @brief Set sound enabled.
 * @details Updates internal state and emits signals as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setSoundEnabled(bool enabled);
/**
 * @brief Set action enabled.
 * @details Updates internal state and emits signals as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setActionEnabled(bool enabled);
/**
 * @brief Set action path.
 * @details Updates internal state and emits signals as needed.
 * @param path Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setActionPath(const QString &path);

/**
 * @brief Continue after exit.
 * @details Performs the operation and updates state as needed.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool continueAfterExit() const;
/**
 * @brief Set continue after exit.
 * @details Updates internal state and emits signals as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setContinueAfterExit(bool enabled);

/**
 * @brief Melody sound path.
 * @details Performs the operation and updates state as needed.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString melodySoundPath() const;
/**
 * @brief Reminder sound path.
 * @details Performs the operation and updates state as needed.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString reminderSoundPath() const;
/**
 * @brief Set melody sound path.
 * @details Updates internal state and emits signals as needed.
 * @param path Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setMelodySoundPath(const QString &path);
/**
 * @brief Set reminder sound path.
 * @details Updates internal state and emits signals as needed.
 * @param path Filesystem path.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setReminderSoundPath(const QString &path);

/**
 * @brief Check whether recommendations enabled.
 * @details Returns the current value derived from internal state.
 * @return True if the condition holds; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool isRecommendationsEnabled() const;
/**
 * @brief Set recommendations enabled.
 * @details Updates internal state and emits signals as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setRecommendationsEnabled(bool enabled);

private slots:
/**
 * @brief On browse clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onBrowseClicked();

/**
 * @brief On add recommendation clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAddRecommendationClicked();
/**
 * @brief On delete recommendation clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onDeleteRecommendationClicked();

private:
    Ui::SettingsTimerDialog *ui;
    TimerManager *m_manager;
    QString m_melodySoundPath; /**< Filesystem path. */
    QString m_reminderSoundPath; /**< Filesystem path. */

/**
 * @brief Animate widget.
 * @details Performs the operation and updates state as needed.
 * @param widget widget value.
 * @param expand expand value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void animateWidget(QWidget *widget, bool expand);
/**
 * @brief Refresh recommendation table.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void refreshRecommendationTable();
};

#endif // SETTINGSTIMERDIALOG_H
