#ifndef HISTORYTIMERWINDOW_H
#define HISTORYTIMERWINDOW_H

#include <QDialog>
#include "timermanager.h"

namespace Ui {
class HistoryTimerWindow;
}

class HistoryTimerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HistoryTimerWindow(QList<TimerData> *deletedTimers, QWidget *parent = nullptr);
    ~HistoryTimerWindow();

signals:
    void restoreTimer(const TimerData &timer);

private slots:
    void updateTable();
    void onCloseClicked();

private:
    Ui::HistoryTimerWindow *ui;
    QList<TimerData> *deletedTimers;
};

#endif // HISTORYTIMERWINDOW_H
