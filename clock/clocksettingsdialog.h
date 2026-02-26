
/**
 * @file clocksettingsdialog.h
 * @brief Declarations for clocksettingsdialog.
 * @details Defines types and functions related to clocksettingsdialog.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef CLOCKSETTINGSDIALOG_H
#define CLOCKSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
/**
 * @brief ClockSettingsDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for ClockSettings.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ClockSettingsDialog; /**< Internal state value. */
}

/**
 * @brief ClockSettingsDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for ClockSettings.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ClockSettingsDialog : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create ClockSettingsDialog instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit ClockSettingsDialog(QWidget *parent = nullptr);
/**
 * @brief Destroy ClockSettingsDialog instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~ClockSettingsDialog();

/**
 * @brief Get selected zone.
 * @details Returns the current value derived from internal state.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString getSelectedZone() const;

private:
    Ui::ClockSettingsDialog *ui;
};

#endif // CLOCKSETTINGSDIALOG_H
