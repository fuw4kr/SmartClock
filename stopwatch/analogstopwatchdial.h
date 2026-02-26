
/**
 * @file analogstopwatchdial.h
 * @brief Declarations for analogstopwatchdial.
 * @details Defines types and functions related to analogstopwatchdial.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ANALOGSTOPWATCHDIAL_H
#define ANALOGSTOPWATCHDIAL_H

#include <QWidget>
#include <QTime>

/**
 * @brief AnalogStopwatchDial stopwatch component.
 * @details Implements stopwatch-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AnalogStopwatchDial : public QWidget
{
    Q_OBJECT

public:
/**
 * @brief Create AnalogStopwatchDial instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit AnalogStopwatchDial(QWidget *parent = nullptr);
/**
 * @brief Get elapsed time.
 * @details Updates internal state and emits signals as needed.
 * @param time time value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setElapsed(const QTime &time);
/**
 * @brief Show operation.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void show();

protected:
/**
 * @brief Paint event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void paintEvent(QPaintEvent *event) override;

private:
    QTime elapsed; /**< Elapsed time in milliseconds. */
};

#endif // ANALOGSTOPWATCHDIAL_H
