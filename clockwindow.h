#ifndef CLOCKWINDOW_H
#define CLOCKWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui { class ClockWindow; }
QT_END_NAMESPACE

struct ClockInfo {
    QString zone;
};

class ClockWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ClockWindow(QWidget *parent = nullptr);
    ~ClockWindow();

private slots:
    void updateTime();
    void onAddClock();
    void onRemoveClock();
    void onToggleFormat(bool checked);

private:
    Ui::ClockWindow *ui;
    QTimer timer;
    QList<ClockInfo> clocks;
    bool format12h;

    void updateListTexts();
    QString timeTextFor(const ClockInfo &ci) const;

    void loadFromJson();
    void saveToJson() const;
};

#endif // CLOCKWINDOW_H
