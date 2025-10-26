#ifndef TIMEREDITDIALOG_H
#define TIMEREDITDIALOG_H

#include <QDialog>

namespace Ui {
class TimerEditDialog;
}

class TimerEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimerEditDialog(QWidget *parent = nullptr);
    ~TimerEditDialog();

    QString getName() const;
    int getTotalSeconds() const;

private:
    Ui::TimerEditDialog *ui;
};

#endif // TIMEREDITDIALOG_H
