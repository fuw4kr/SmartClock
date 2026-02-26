
/**
 * @file timereditdialog.h
 * @brief Declarations for timereditdialog.
 * @details Defines types and functions related to timereditdialog.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef TIMEREDITDIALOG_H
#define TIMEREDITDIALOG_H

#include <QDialog>

namespace Ui {
/**
 * @brief TimerEditDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for TimerEdit.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class TimerEditDialog; /**< Timer-related state. */
}

/**
 * @brief TimerEditDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for TimerEdit.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class TimerEditDialog : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create TimerEditDialog instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit TimerEditDialog(QWidget *parent = nullptr);
/**
 * @brief Destroy TimerEditDialog instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~TimerEditDialog();

/**
 * @brief Get name.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getName() const;
/**
 * @brief Get total seconds.
 * @details Returns the current value derived from internal state.
 * @return Integer value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    int getTotalSeconds() const;
/**
 * @brief Get type.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getType() const;
/**
 * @brief Get group.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getGroup() const;

private slots:
/**
 * @brief Update input mode.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateInputMode();

signals:
/**
 * @brief Timer added.
 * @details Performs the operation and updates state as needed.
 * @param name Name string.
 * @param duration duration value.
 * @param type type value.
 * @param group group value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void timerAdded(const QString &name, int duration, const QString &type, const QString &group);

private:
    Ui::TimerEditDialog *ui;
};

#endif // TIMEREDITDIALOG_H
