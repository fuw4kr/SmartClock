#ifndef ALARMITEMWIDGET_H
#define ALARMITEMWIDGET_H

#include <QWidget>
#include "alarmmanager.h"

namespace Ui {
class AlarmItemWidget;
}

class AlarmItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmItemWidget(const AlarmData &data, QWidget *parent = nullptr);
    ~AlarmItemWidget();

    void setAlarmData(const AlarmData &data);
    AlarmData getAlarmData() const;

signals:
    void toggled(bool enabled);

private:
    Ui::AlarmItemWidget *ui;
    AlarmData alarm;
};

#endif // ALARMITEMWIDGET_H
