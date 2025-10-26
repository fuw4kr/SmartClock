#ifndef TIMERWINDOW_H
#define TIMERWINDOW_H

#include <QWidget>
#include "timermanager.h"
#include "settingstimerdialog.h"

namespace Ui {
class TimerWindow;
}

class TimerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TimerWindow(QWidget *parent = nullptr);
    ~TimerWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onAddTimer();
    void onSettings();
    void onHistory();
    void onStartPauseTimer();
    void onDeleteTimer();
    void onEditTimer();
    void updateTable();

private:
    Ui::TimerWindow *ui;
    TimerManager manager;
    bool playSound = true;
    bool runAction = false;
    QString actionPath;
    void populateHistoryTable();
    void saveHistoryJson() const;
    void loadHistoryJson();
    QString historyFilePath() const;
    QString timersFilePath() const;
    QList<TimerData> deletedTimers;
    bool continueAfterExit = false;



};

#endif // TIMERWINDOW_H
