#ifndef STOPWATCHWINDOW_H
#define STOPWATCHWINDOW_H

#include <QDialog>

namespace Ui {
class StopwatchWindow;
}

class StopwatchWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StopwatchWindow(QWidget *parent = nullptr);
    ~StopwatchWindow();

private:
    Ui::StopwatchWindow *ui;
};

#endif // STOPWATCHWINDOW_H
