#include "clockwindow.h"
#include "ui_clockwindow.h"
#include "clocksettingsdialog.h"
#include <QMessageBox>
#include <QTimeZone>
#include <QShortcut>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QSignalBlocker>
#include <QLocale>
#include <QDebug>

ClockWindow::ClockWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ClockWindow)
    , model(new ClockModel(this))
{
    ui->setupUi(this);
    setWindowTitle("World Clock");

    connect(ui->btnAddClock, &QPushButton::clicked, this, &ClockWindow::onAddClock);
    connect(ui->btnRemoveClock, &QPushButton::clicked, this, &ClockWindow::onRemoveClock);
    connect(ui->checkFormat12, &QCheckBox::toggled, this, &ClockWindow::onToggleFormat);

    QShortcut *deleteShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(deleteShortcut, &QShortcut::activated, this, &ClockWindow::onRemoveClock);

    connect(&timer, &QTimer::timeout, this, &ClockWindow::updateTime);
    timer.start(1000);

    controller = new ClockController(model, this, this);
    ui->checkFormat12->setChecked(model->format12h());
    updateTime();
}

ClockWindow::~ClockWindow()
{
    delete ui;
}

void ClockWindow::syncFromModel()
{
    ui->listClocks->clearSelection();
    ui->listClocks->clear();

    const auto &clocks = model->clocks();
    for (const auto &ci : clocks)
        ui->listClocks->addItem(timeTextFor(ci));

    QSignalBlocker blocker(ui->checkFormat12);
    ui->checkFormat12->setChecked(model->format12h());

    updateListTexts();
}

void ClockWindow::updateTime()
{
    QLocale en(QLocale::English);
    QString fmt = model->format12h() ? "hh:mm:ss AP" : "HH:mm:ss";

    QDateTime now = QDateTime::currentDateTime();
    ui->labelMainTime->setText(en.toString(now.time(), fmt));
    ui->labelZone->setText("Local Time (" + QString::fromUtf8(QTimeZone::systemTimeZoneId()) + ")");

    updateListTexts();
}

void ClockWindow::onAddClock()
{
    ClockSettingsDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        emit addClockRequested(dlg.getSelectedZone());
    }
}

void ClockWindow::onRemoveClock()
{
    auto selected = ui->listClocks->selectedItems();

    if (selected.isEmpty()) {
        if (qEnvironmentVariableIsSet("TEST_MODE")) {
            qDebug() << "[TEST_MODE] Suppressed Settings dialog";
            return;
        }
        QMessageBox::information(this, "No selection", "Please select at least one clock to delete.");
        return;
    }

    if (QMessageBox::question(this, "Confirm Deletion",
                              QString("Delete %1 selected clock(s)?").arg(selected.size()),
                              QMessageBox::Yes | QMessageBox::No)
        == QMessageBox::No)
        return;

    QList<int> rows;
    for (auto *it : selected)
        rows << ui->listClocks->row(it);
    emit removeClocksRequested(rows);
}

void ClockWindow::onToggleFormat(bool checked)
{
    emit formatToggled(checked);
    updateTime();
}

QString ClockWindow::timeTextFor(const ClockInfo &ci) const
{
    return model->timeTextFor(ci);
}

void ClockWindow::updateListTexts()
{
    const auto &clocks = model->clocks();
    if (clocks.isEmpty()) {
        QPropertyAnimation *anim = new QPropertyAnimation(ui->listClocks, "maximumHeight");
        anim->setDuration(200);
        anim->setStartValue(ui->listClocks->height());
        anim->setEndValue(0);
        anim->setEasingCurve(QEasingCurve::InOutQuad);
        connect(anim, &QPropertyAnimation::finished, [this]() {
            ui->listClocks->setVisible(false);
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        return;
    }

    ui->listClocks->setVisible(true);

    for (int i = 0; i < clocks.size(); ++i)
        if (auto item = ui->listClocks->item(i))
            item->setText(timeTextFor(clocks[i]));

    int rowHeight = 28;
    int maxVisible = 6;
    int newHeight = qMin(clocks.size(), maxVisible) * rowHeight + 10;

    QPropertyAnimation *anim = new QPropertyAnimation(ui->listClocks, "maximumHeight");
    anim->setDuration(250);
    anim->setStartValue(ui->listClocks->height());
    anim->setEndValue(newHeight);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
