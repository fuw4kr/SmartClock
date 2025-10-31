#include "historytimerwindow.h"
#include "ui_historytimerwindow.h"
#include <QMessageBox>
#include <QPushButton>
#include <QKeyEvent>
#include <QPropertyAnimation>

HistoryTimerWindow::HistoryTimerWindow(QList<TimerData> *deletedTimers, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryTimerWindow)
    , deletedTimers(deletedTimers)
{
    ui->setupUi(this);
    setWindowTitle("Timer History");

    ui->tableHistory->setColumnCount(4);
    ui->tableHistory->setShowGrid(false);
    ui->tableHistory->setHorizontalHeaderLabels({"Name", "Duration", "Restore", "Delete"});
    ui->tableHistory->horizontalHeader()->setStretchLastSection(true);
    ui->tableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableHistory->setMouseTracking(true);
    connect(ui->tableHistory, &QTableWidget::entered,
            this, [this](const QModelIndex &index) {
                ui->tableHistory->selectRow(index.row());
            });

    connect(ui->tableHistory, &QTableWidget::viewportEntered,
            this, [this]() {
                ui->tableHistory->clearSelection();
            });

    connect(ui->btnClose, &QPushButton::clicked, this, &HistoryTimerWindow::onCloseClicked);
    ui->btnClose->setShortcut(Qt::Key_Escape);

    ui->widgetActions->setVisible(false);
    ui->widgetActions->setMaximumHeight(0);

    connect(ui->btnRestoreSelected, &QPushButton::clicked, this, &HistoryTimerWindow::onRestoreSelected);
    connect(ui->btnDeleteSelected, &QPushButton::clicked, this, &HistoryTimerWindow::onDeleteSelected);

    connect(ui->tableHistory->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this]() {
        int selectedCount = ui->tableHistory->selectionModel()->selectedRows().count();
        bool shouldShow = selectedCount >= 2;

    if (ui->widgetActions->isVisible() != shouldShow) {
        QPropertyAnimation *anim = new QPropertyAnimation(ui->widgetActions, "maximumHeight");
        anim->setDuration(200);
        anim->setStartValue(shouldShow ? 0 : ui->widgetActions->height());
        anim->setEndValue(shouldShow ? 40 : 0);
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        ui->widgetActions->setVisible(shouldShow);
     }
    });

    ui->tableHistory->setSelectionMode(QAbstractItemView::ExtendedSelection);

    updateTable();
}

HistoryTimerWindow::~HistoryTimerWindow()
{
    delete ui;
}

void HistoryTimerWindow::onRestoreSelected()
{
    QModelIndexList selectedRows = ui->tableHistory->selectionModel()->selectedRows();
    if (selectedRows.size() < 2) return;

    QList<int> indexes;
    for (const QModelIndex &idx : selectedRows)
        indexes.append(idx.row());
    std::sort(indexes.begin(), indexes.end(), std::greater<int>());

    for (int i : indexes) {
        if (i >= 0 && i < deletedTimers->size()) {
            TimerData timer = deletedTimers->takeAt(i);
            emit restoreTimer(timer);
        }
    }

    updateTable();
    ui->widgetActions->setVisible(false);
}

void HistoryTimerWindow::onDeleteSelected()
{
    QModelIndexList selectedRows = ui->tableHistory->selectionModel()->selectedRows();
    if (selectedRows.size() < 2) return;

    if (QMessageBox::question(this, "Delete permanently",
                              "Delete selected timers permanently?",
                              QMessageBox::Yes | QMessageBox::No)
        != QMessageBox::Yes)
        return;

    QList<int> indexes;
    for (const QModelIndex &idx : selectedRows)
        indexes.append(idx.row());
    std::sort(indexes.begin(), indexes.end(), std::greater<int>());

    for (int i : indexes) {
        if (i >= 0 && i < deletedTimers->size())
            deletedTimers->removeAt(i);
    }

    updateTable();
    ui->widgetActions->setVisible(false);
    emit historyChanged();
}

void HistoryTimerWindow::updateTable()
{
    ui->tableHistory->clearContents();
    ui->tableHistory->setRowCount(deletedTimers->size());

    for (int i = 0; i < deletedTimers->size(); ++i) {
        const TimerData &t = deletedTimers->at(i);

        ui->tableHistory->setItem(i, 0, new QTableWidgetItem(t.name));

        int hours = t.duration / 3600;
        int minutes = (t.duration % 3600) / 60;
        int seconds = t.duration % 60;
        QString duration = QString("%1:%2:%3")
                               .arg(hours, 2, 10, QChar('0'))
                               .arg(minutes, 2, 10, QChar('0'))
                               .arg(seconds, 2, 10, QChar('0'));
        ui->tableHistory->setItem(i, 1, new QTableWidgetItem(duration));

        QPushButton *btnRestore = new QPushButton("Restore", ui->tableHistory);
        btnRestore->setProperty("index", i);
        ui->tableHistory->setCellWidget(i, 2, btnRestore);

        QPushButton *btnDelete = new QPushButton("Delete", ui->tableHistory);
        btnDelete->setProperty("index", i);
        ui->tableHistory->setCellWidget(i, 3, btnDelete);

        connect(btnRestore, &QPushButton::clicked, this, [this, btnRestore]() {
            bool ok;
            int index = btnRestore->property("index").toInt(&ok);
            if (!ok || index < 0 || index >= deletedTimers->size()) return;

            TimerData timer = deletedTimers->takeAt(index);
            emit restoreTimer(timer);
            updateTable();
        });

        connect(btnDelete, &QPushButton::clicked, this, [this, btnDelete]() {
            bool ok;
            int index = btnDelete->property("index").toInt(&ok);
            if (!ok || index < 0 || index >= deletedTimers->size()) return;

            if (QMessageBox::question(this, "Delete permanently",
                                      "Delete this timer permanently?",
                                      QMessageBox::Yes | QMessageBox::No)
                == QMessageBox::Yes)
            {
                deletedTimers->removeAt(index);
                updateTable();
                emit historyChanged();
            }
        });
    }
}

void HistoryTimerWindow::onCloseClicked()
{
    accept();
}
