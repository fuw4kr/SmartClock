#ifndef TIMERWINDOW_H
#define TIMERWINDOW_H

#include "timermanager.h"
#include "settingstimerdialog.h"
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>

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
    void updateNextUpLabel();

    void updateTrayTooltip();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);


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

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QTimer *trayUpdateTimer;

};

#endif // TIMERWINDOW_H
