
/**
 * @file alarmitemwidget.h
 * @brief Declarations for alarmitemwidget.
 * @details Defines types and functions related to alarmitemwidget.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ALARMITEMWIDGET_H
#define ALARMITEMWIDGET_H

#include <QWidget>
#include "alarmmanager.h"

namespace Ui {
/**
 * @brief AlarmItemWidget alarm component.
 * @details Implements alarm-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmItemWidget; /**< Alarm-related state. */
}

/**
 * @brief AlarmItemWidget alarm component.
 * @details Implements alarm-related behavior.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmItemWidget : public QWidget
{
    Q_OBJECT

public:
/**
 * @brief Create AlarmItemWidget instance.
 * @details Initializes instance state.
 * @param data data value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit AlarmItemWidget(const AlarmData &data, QWidget *parent = nullptr);
/**
 * @brief Destroy AlarmItemWidget instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~AlarmItemWidget();

/**
 * @brief Set alarm data.
 * @details Updates internal state and emits signals as needed.
 * @param data data value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setAlarmData(const AlarmData &data);
/**
 * @brief Get alarm data.
 * @details Returns the current value derived from internal state.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    AlarmData getAlarmData() const;

signals:
/**
 * @brief Toggled.
 * @details Performs the operation and updates state as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void toggled(bool enabled);
protected:
/**
 * @brief Event.
 * @details Performs the operation and updates state as needed.
 * @param e e value.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool event(QEvent *e) override;
/**
 * @brief Event filter.
 * @details Performs the operation and updates state as needed.
 * @param obj obj value.
 * @param e e value.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
/**
 * @brief Set hovered.
 * @details Updates internal state and emits signals as needed.
 * @param hovered hovered value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setHovered(bool hovered);

    Ui::AlarmItemWidget *ui;
    AlarmData alarm; /**< Alarm-related state. */
};

#endif // ALARMITEMWIDGET_H
