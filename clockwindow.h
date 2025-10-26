#ifndef ClockWindow_H
#define ClockWindow_H

#include <QWidget>

namespace Ui {
class ClockWindow;
}

class ClockWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ClockWindow(QWidget *parent = nullptr);
    ~ClockWindow();

private:
    Ui::ClockWindow *ui;
};

#endif // ClockWindow_H
