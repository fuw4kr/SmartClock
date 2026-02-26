
/**
 * @file alarmfactory.h
 * @brief Declarations for alarmfactory.
 * @details Defines types and functions related to alarmfactory.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef ALARMFACTORY_H
#define ALARMFACTORY_H

#include <QGraphicsOpacityEffect>
#include "alarmitemwidget.h"

/**
 * @brief AlarmFactory Factory that creates configured objects.
 * @details Provides factory behavior for Alarm.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class AlarmFactory
{
public:
/**
 * @brief Create alarm widget.
 * @details Performs the operation and updates state as needed.
 * @param data data value.
 * @param parent Parent QObject.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static AlarmItemWidget* createAlarmWidget(const AlarmData &data, QWidget *parent = nullptr)
    {
        auto *widget = new AlarmItemWidget(data, parent);
        if (!data.enabled) {
            auto *effect = new QGraphicsOpacityEffect(widget);
            effect->setOpacity(0.5);
            widget->setGraphicsEffect(effect);
        }
        return widget; /**< Internal state value. */
    }
};

#endif // ALARMFACTORY_H
