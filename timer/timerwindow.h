#ifndef TIMERWINDOW_H
#define TIMERWINDOW_H

#include "timermanager.h"
#include "settingstimerdialog.h"
#include "../controllers/timercontroller.h"
#include <QWidget>

namespace Ui {
class TimerWindow;
}

class TimerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TimerWindow(QWidget *parent = nullptr);
    ~TimerWindow();
    TimerManager* getManager() { return manager; }

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void addTimerRequested(const QString &name, int seconds, const QString &type, const QString &group);
    void editTimerRequested(int index, const QString &name, int seconds, const QString &type, const QString &group);
    void deleteTimersRequested(const QList<int> &rows);
    void startPauseRequested(const QList<int> &rows);
    void saveRequested();

private slots:
    void onAddTimer();
    void onSettings();
    void onHistory();
    void onStartPauseTimer();
    void onDeleteTimer();
    void onEditTimer();
    void updateTable();
    void updateNextUpLabel();

private:
    Ui::TimerWindow *ui;
    TimerManager *manager;
    TimerController *controller;
    bool playSound = true;
    bool runAction = false;
    QString actionPath;
    void populateHistoryTable();
    void saveHistoryJson() const;
    void loadHistoryJson();
    QString historyFilePath() const;
    QList<TimerData> deletedTimers;
    bool continueAfterExit = false;
};

#endif // TIMERWINDOW_H
