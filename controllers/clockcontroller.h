#ifndef CLOCKCONTROLLER_H
#define CLOCKCONTROLLER_H

#include <QObject>
#include "../clock/clockmodel.h"

class ClockWindow;

class ClockController : public QObject
{
    Q_OBJECT
public:
    explicit ClockController(ClockModel *model, ClockWindow *view, QObject *parent = nullptr);

private slots:
    void onAddClockRequested(const QString &zone);
    void onRemoveClocksRequested(const QList<int> &rows);
    void onFormatToggled(bool enabled);

private:
    ClockModel *model;
    ClockWindow *view;
};

#endif // CLOCKCONTROLLER_H
