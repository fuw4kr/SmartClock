
/**
 * @file timerwindow.h
 * @brief Declarations for timerwindow.
 * @details Defines types and functions related to timerwindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef TIMERWINDOW_H
#define TIMERWINDOW_H

#include "timermanager.h"
#include "settingstimerdialog.h"
#include "../controllers/timercontroller.h"
#include <QWidget>

namespace Ui {
/**
 * @brief TimerWindow Top-level window UI class.
 * @details Provides window behavior for Timer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class TimerWindow; /**< Timer-related state. */
}

/**
 * @brief TimerWindow Top-level window UI class.
 * @details Provides window behavior for Timer.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class TimerWindow : public QWidget
{
    Q_OBJECT

public:
/**
 * @brief Create TimerWindow instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit TimerWindow(QWidget *parent = nullptr);
/**
 * @brief Destroy TimerWindow instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~TimerWindow();
/**
 * @brief Get manager.
 * @details Returns the current value derived from internal state.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    TimerManager* getManager() { return manager; }

protected:
/**
 * @brief Close event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void closeEvent(QCloseEvent *event) override;

signals:
/**
 * @brief Add timer requested.
 * @details Modifies the associated collection and notifies listeners.
 * @param name Name string.
 * @param seconds seconds value.
 * @param type type value.
 * @param group group value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addTimerRequested(const QString &name, int seconds, const QString &type, const QString &group);
/**
 * @brief Edit timer requested.
 * @details Performs the operation and updates state as needed.
 * @param index Zero-based index.
 * @param name Name string.
 * @param seconds seconds value.
 * @param type type value.
 * @param group group value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void editTimerRequested(int index, const QString &name, int seconds, const QString &type, const QString &group);
/**
 * @brief Delete timers requested.
 * @details Performs the operation and updates state as needed.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void deleteTimersRequested(const QList<int> &rows);
/**
 * @brief Start pause requested.
 * @details Transitions the state machine and notifies listeners.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void startPauseRequested(const QList<int> &rows);
/**
 * @brief Save requested.
 * @details Writes current state to persistent storage.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void saveRequested();

private slots:
/**
 * @brief On add timer.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAddTimer();
/**
 * @brief On settings.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onSettings();
/**
 * @brief On history.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onHistory();
/**
 * @brief On start pause timer.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onStartPauseTimer();
/**
 * @brief On delete timer.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onDeleteTimer();
/**
 * @brief On edit timer.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onEditTimer();
/**
 * @brief Update table.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateTable();
/**
 * @brief Update next up label.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateNextUpLabel();

private:
    Ui::TimerWindow *ui;
    TimerManager *manager;
    TimerController *controller;
    bool playSound = true; /**< Internal state value. */
    bool runAction = false; /**< Internal state value. */
    QString actionPath; /**< Filesystem path. */
/**
 * @brief Populate history table.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void populateHistoryTable();
/**
 * @brief Save history json.
 * @details Writes current state to persistent storage.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void saveHistoryJson() const;
/**
 * @brief Load history json.
 * @details Reads persisted state into the object.
 * @return True on success; false on failure.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void loadHistoryJson();
/**
 * @brief History file path.
 * @details Performs the operation and updates state as needed.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString historyFilePath() const;
    QList<TimerData> deletedTimers; /**< Timer-related state. */
    bool continueAfterExit = false; /**< Internal state value. */
};

#endif // TIMERWINDOW_H
