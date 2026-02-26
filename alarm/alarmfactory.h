#ifndef ALARMFACTORY_H
#define ALARMFACTORY_H

#include <QGraphicsOpacityEffect>
#include "alarmitemwidget.h"

class AlarmFactory
{
public:
    static AlarmItemWidget* createAlarmWidget(const AlarmData &data, QWidget *parent = nullptr)
    {
        auto *widget = new AlarmItemWidget(data, parent);
        if (!data.enabled) {
            auto *effect = new QGraphicsOpacityEffect(widget);
            effect->setOpacity(0.5);
            widget->setGraphicsEffect(effect);
        }
        return widget;
    }
};

#endif // ALARMFACTORY_H
