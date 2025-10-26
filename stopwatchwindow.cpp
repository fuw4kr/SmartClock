#include "stopwatchwindow.h"
#include "ui_stopwatchwindow.h"

StopwatchWindow::StopwatchWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StopwatchWindow)
{
    ui->setupUi(this);
}

StopwatchWindow::~StopwatchWindow()
{
    delete ui;
}
