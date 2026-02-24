#ifndef STOPWATCHWINDOW_H
#define STOPWATCHWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QStackedWidget>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include "analogstopwatchdial.h"

namespace Ui {
class StopwatchWindow;
}

class StopwatchWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StopwatchWindow(QWidget *parent = nullptr);
    ~StopwatchWindow();
    QString getCurrentLapTimeString() const;
    QString getTotalTimeString() const;

private slots:
    void onStartStopClicked();
    void onLapClicked();
    void updateDisplay();
    void resetStopwatch();

private:
    Ui::StopwatchWindow *ui;
    QTimer *timer;
    QTime elapsed;
    bool running = false;
    QList<QTime> lapTimes;
    QStringList lapTexts;
    QList<int> lapDurations;

    AnalogStopwatchDial *analogDial;
    bool analogMode = false;

    QString filePath() const;
    void saveToFile() const;
    void loadFromFile();
    void updateLapColors();
    QStackedWidget *stackedView;

    friend class StopwatchWindowTest_LapAndResetWork_Test;
};

#endif // STOPWATCHWINDOW_H
