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
    QString getType() const;
    QString getGroup() const;

private slots:
    void updateInputMode();

signals:
    void timerAdded(const QString &name, int duration, const QString &type, const QString &group);

private:
    Ui::TimerEditDialog *ui;
};

#endif // TIMEREDITDIALOG_H
