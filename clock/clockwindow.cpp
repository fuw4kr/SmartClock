#include "clockwindow.h"
#include "ui_clockwindow.h"
#include "clocksettingsdialog.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimeZone>
#include <QShortcut>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QEvent>
#include <QStyle>

ClockWindow::ClockWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ClockWindow)
    , format12h(false)
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

    loadFromJson();

    ui->checkFormat12->setChecked(format12h);
    updateTime();
}

ClockWindow::~ClockWindow()
{
    saveToJson();
    delete ui;
}

void ClockWindow::updateTime()
{
    QLocale en(QLocale::English);
    QString fmt = format12h ? "hh:mm:ss AP" : "HH:mm:ss";

    QDateTime now = QDateTime::currentDateTime();
    ui->labelMainTime->setText(en.toString(now.time(), fmt));
    ui->labelZone->setText("Local Time (" + QString::fromUtf8(QTimeZone::systemTimeZoneId()) + ")");

    updateListTexts();
}

void ClockWindow::onAddClock()
{
    ClockSettingsDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        ClockInfo ci;
        ci.zone = dlg.getSelectedZone();
        clocks.append(ci);
        ui->listClocks->addItem(timeTextFor(ci));
        ui->listClocks->setVisible(true);
        updateListTexts();
        saveToJson();
    }
}

void ClockWindow::onRemoveClock()
{
    auto selected = ui->listClocks->selectedItems();

    if (selected.isEmpty()) {
        QMessageBox::information(this, "No selection", "Please select at least one clock to delete.");
        return;
    }

    if (QMessageBox::question(this, "Confirm Deletion",
                              QString("Delete %1 selected clock(s)?").arg(selected.size()),
                              QMessageBox::Yes | QMessageBox::No)
        == QMessageBox::No)
        return;

    for (auto *it : selected) {
        int row = ui->listClocks->row(it);
        clocks.removeAt(row);
        delete ui->listClocks->takeItem(row);
    }

    ui->listClocks->setVisible(!clocks.isEmpty());
    updateListTexts();
    saveToJson();
}

void ClockWindow::onToggleFormat(bool checked)
{
    format12h = checked;
    updateTime();
    saveToJson();
}

QString ClockWindow::timeTextFor(const ClockInfo &ci) const
{
    QTimeZone tz(ci.zone.toUtf8());
    QDateTime nowTz = QDateTime::currentDateTimeUtc().toTimeZone(tz);
    QLocale en(QLocale::English);
    QString fmt = format12h ? "hh:mm:ss AP" : "HH:mm:ss";
    QString timeText = en.toString(nowTz.time(), fmt);
    return QString("%1 — %2").arg(QString::fromUtf8(tz.id()), timeText);
}

void ClockWindow::updateListTexts()
{
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

void ClockWindow::saveToJson() const
{
    QJsonObject root;
    root["format12h"] = format12h;

    QJsonArray arr;
    for (const ClockInfo &ci : clocks) {
        QJsonObject obj;
        obj["zone"] = ci.zone;
        arr.append(obj);
    }
    root["clocks"] = arr;

    QJsonDocument doc(root);
    QFile file("clocks.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void ClockWindow::loadFromJson()
{
    ui->listClocks->clearSelection();

    QFile file("clocks.json");
    if (!file.exists()) {
        ui->listClocks->setVisible(false);
        return;
    }
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    format12h = root.value("format12h").toBool(false);

    clocks.clear();
    ui->listClocks->clear();

    QJsonArray arr = root.value("clocks").toArray();
    for (const QJsonValue &v : arr) {
        if (!v.isObject()) continue;
        QJsonObject o = v.toObject();
        ClockInfo ci;
        ci.zone = o.value("zone").toString();
        if (!ci.zone.isEmpty()) {
            clocks.append(ci);
            ui->listClocks->addItem(timeTextFor(ci));
        }
    }

    ui->listClocks->setVisible(!clocks.isEmpty());
}

