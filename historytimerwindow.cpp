#include "historytimerwindow.h"
#include "ui_historytimerwindow.h"
#include <QMessageBox>
#include <QPushButton>
#include <QKeyEvent>

HistoryTimerWindow::HistoryTimerWindow(QList<TimerData> *deletedTimers, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HistoryTimerWindow)
    , deletedTimers(deletedTimers)
{
    ui->setupUi(this);
    setWindowTitle("Timer History");

    ui->tableHistory->setColumnCount(4);
    ui->tableHistory->setHorizontalHeaderLabels({"Name", "Duration", "Restore", "Delete"});
    ui->tableHistory->horizontalHeader()->setStretchLastSection(true);
    ui->tableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->btnClose, &QPushButton::clicked, this, &HistoryTimerWindow::onCloseClicked);
    ui->btnClose->setShortcut(Qt::Key_Escape); // ← Esc закриває вікно

    updateTable();
}

HistoryTimerWindow::~HistoryTimerWindow()
{
    delete ui;
}

void HistoryTimerWindow::updateTable()
{
    ui->tableHistory->clearContents();
    ui->tableHistory->setRowCount(deletedTimers->size());

    for (int i = 0; i < deletedTimers->size(); ++i) {
        const TimerData &t = deletedTimers->at(i);

        // Назва
        ui->tableHistory->setItem(i, 0, new QTableWidgetItem(t.name));

        // Тривалість
        int hours = t.duration / 3600;
        int minutes = (t.duration % 3600) / 60;
        int seconds = t.duration % 60;
        QString duration = QString("%1:%2:%3")
                               .arg(hours, 2, 10, QChar('0'))
                               .arg(minutes, 2, 10, QChar('0'))
                               .arg(seconds, 2, 10, QChar('0'));
        ui->tableHistory->setItem(i, 1, new QTableWidgetItem(duration));

        // кнопка Restore
        QPushButton *btnRestore = new QPushButton("Restore", ui->tableHistory);
        btnRestore->setProperty("index", i);
        ui->tableHistory->setCellWidget(i, 2, btnRestore);

        // кнопка Delete
        QPushButton *btnDelete = new QPushButton("Delete", ui->tableHistory);
        btnDelete->setProperty("index", i);
        ui->tableHistory->setCellWidget(i, 3, btnDelete);

        // Restore натискання
        connect(btnRestore, &QPushButton::clicked, this, [this, btnRestore]() {
            bool ok;
            int index = btnRestore->property("index").toInt(&ok);
            if (!ok || index < 0 || index >= deletedTimers->size()) return;

            TimerData timer = deletedTimers->takeAt(index);
            emit restoreTimer(timer);  // сигнал у TimerWindow
            updateTable();             // оновити таблицю
        });

        // Delete натискання
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
            }
        });
    }
}

void HistoryTimerWindow::onCloseClicked()
{
    accept(); // закриває вікно
}
