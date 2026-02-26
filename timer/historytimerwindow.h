
/**
 * @file historytimerwindow.h
 * @brief Declarations for historytimerwindow.
 * @details Defines types and functions related to historytimerwindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef HISTORYTIMERWINDOW_H
#define HISTORYTIMERWINDOW_H

#include <QDialog>
#include "timermanager.h"

namespace Ui {
/**
 * @brief HistoryTimerWindow Top-level window UI class.
 * @details Provides window behavior for HistoryTimer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class HistoryTimerWindow; /**< Timer-related state. */
}

/**
 * @brief HistoryTimerWindow Top-level window UI class.
 * @details Provides window behavior for HistoryTimer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class HistoryTimerWindow : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create HistoryTimerWindow instance.
 * @details Initializes instance state.
 * @param deletedTimers deletedTimers value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit HistoryTimerWindow(QList<TimerData> *deletedTimers, QWidget *parent = nullptr);
/**
 * @brief Destroy HistoryTimerWindow instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~HistoryTimerWindow();

signals:
/**
 * @brief Restore timer.
 * @details Performs the operation and updates state as needed.
 * @param timer timer value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void restoreTimer(const TimerData &timer);
/**
 * @brief Emitted when history changes.
 * @details Signal emitted when the associated state changes.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void historyChanged();

private slots:
/**
 * @brief Update table.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateTable();
/**
 * @brief On close clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onCloseClicked();
/**
 * @brief On restore selected.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onRestoreSelected();
/**
 * @brief On delete selected.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onDeleteSelected();

private:
    Ui::HistoryTimerWindow *ui;
    QList<TimerData> *deletedTimers;
};

#endif // HISTORYTIMERWINDOW_H
