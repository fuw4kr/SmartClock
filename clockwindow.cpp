#include "clockwindow.h"
#include "ui_clockwindow.h"

ClockWindow::ClockWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClockWindow)
{
    ui->setupUi(this);
}

ClockWindow::~ClockWindow()
{
    delete ui;
}
