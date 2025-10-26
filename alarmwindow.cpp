#include "alarmwindow.h"
#include "ui_alarmwindow.h"

AlarmWindow::AlarmWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AlarmWindow)
{
    ui->setupUi(this);
}

AlarmWindow::~AlarmWindow()
{
    delete ui;
}
