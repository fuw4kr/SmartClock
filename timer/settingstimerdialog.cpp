#include "settingstimerdialog.h"
#include "ui_settingstimerdialog.h"
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QMessageBox>

SettingsTimerDialog::SettingsTimerDialog(TimerManager *manager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsTimerDialog)
    , m_manager(manager)
{
    ui->setupUi(this);
    setWindowTitle("Timer Settings");

    ui->widgetFileSelect->setVisible(false);
    connect(ui->checkAction, &QCheckBox::toggled, this, [this](bool checked) {
        animateWidget(ui->widgetFileSelect, checked);
    });

    connect(ui->btnBrowse, &QPushButton::clicked, this, &SettingsTimerDialog::onBrowseClicked);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsTimerDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingsTimerDialog::reject);

    ui->widget_3->setMaximumHeight(0);
    ui->widget_3->setVisible(false);
    connect(ui->checkSound, &QCheckBox::toggled, this, [this](bool checked) {
        animateWidget(ui->widget_3, checked);
    });

    ui->widgetRecommendations->setMaximumHeight(0);
    ui->widgetRecommendations->setVisible(false);

    connect(ui->checkBox, &QCheckBox::toggled, this, [this](bool checked) {
        animateWidget(ui->widgetRecommendations, checked);
    });
    connect(ui->btnAdd, &QPushButton::clicked,
            this, &SettingsTimerDialog::onAddRecommendationClicked);
    connect(ui->btnRemove, &QPushButton::clicked,
            this, &SettingsTimerDialog::onDeleteRecommendationClicked);

    for (const TimerData &t : m_manager->getTimers()) {
        ui->comboFrom->addItem(t.name);
        ui->comboTo->addItem(t.name);
    }

    ui->tableRecommendations->setColumnCount(2);
    ui->tableRecommendations->setShowGrid(false);
    ui->tableRecommendations->setHorizontalHeaderLabels({"After Timer", "Recommend"});
    ui->tableRecommendations->horizontalHeader()->setStretchLastSection(true);
    ui->tableRecommendations->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableRecommendations->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableRecommendations->setMouseTracking(true);
    connect(ui->tableRecommendations, &QTableWidget::entered,
            this, [this](const QModelIndex &index) {
                ui->tableRecommendations->selectRow(index.row());
            });

    connect(ui->tableRecommendations, &QTableWidget::viewportEntered,
            this, [this]() {
                ui->tableRecommendations->clearSelection();
            });

    refreshRecommendationTable();
}

SettingsTimerDialog::~SettingsTimerDialog()
{
    delete ui;
}

void SettingsTimerDialog::animateWidget(QWidget *widget, bool expand)
{
    if (!widget)
        return;

    int startHeight = widget->maximumHeight();
    int endHeight = expand ? widget->sizeHint().height() : 0;

    if (expand)
        widget->setVisible(true);

    auto *animation = new QPropertyAnimation(widget, "maximumHeight", this);
    animation->setDuration(250);
    animation->setStartValue(startHeight);
    animation->setEndValue(endHeight);
    animation->setEasingCurve(QEasingCurve::InOutCubic);

    connect(animation, &QPropertyAnimation::finished, this, [=]() {
        if (!expand)
            widget->setVisible(false);
        else {
            widget->setMaximumHeight(QWIDGETSIZE_MAX);
            widget->updateGeometry();
            if (auto *lay = this->layout())
                lay->invalidate();
        }
    });

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void SettingsTimerDialog::onAddRecommendationClicked()
{
    QString from = ui->comboFrom->currentText();
    QString to = ui->comboTo->currentText();

    if (from == to || from.isEmpty() || to.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select two different timers.");
        return;
    }

    if (!m_manager->hasTimer(from) || !m_manager->hasTimer(to)) {
        QMessageBox::warning(this, "Warning", "One or both selected timers no longer exist.");
        return;
    }

    m_manager->setRecommendation(from, to);
    refreshRecommendationTable();

    QMessageBox::information(this, "Recommendation added",
                             QString("After '%1' → recommend '%2'").arg(from, to));
}

void SettingsTimerDialog::onDeleteRecommendationClicked()
{
    int row = ui->tableRecommendations->currentRow();
    if (row < 0) return;

    QString from = ui->tableRecommendations->item(row, 0)->text();
    m_manager->removeRecommendation(from);
    refreshRecommendationTable();
}

void SettingsTimerDialog::refreshRecommendationTable()
{
    auto recs = m_manager->getAllRecommendations();

    QMap<QString, QString> validRecs;
    for (auto it = recs.begin(); it != recs.end(); ++it) {
        if (m_manager->hasTimer(it.key()) && m_manager->hasTimer(it.value())) {
            validRecs.insert(it.key(), it.value());
        } else {
            m_manager->removeRecommendation(it.key());
        }
    }

    ui->tableRecommendations->setRowCount(validRecs.size());

    int row = 0;
    for (auto it = validRecs.begin(); it != validRecs.end(); ++it) {
        ui->tableRecommendations->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->tableRecommendations->setItem(row, 1, new QTableWidgetItem(it.value()));
        row++;
    }

    ui->tableRecommendations->resizeColumnsToContents();
    ui->tableRecommendations->resizeRowsToContents();
    ui->tableRecommendations->resizeColumnsToContents();
    ui->tableRecommendations->setVisible(true);
    ui->tableRecommendations->updateGeometry();
}

void SettingsTimerDialog::onBrowseClicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select file or program to run");
    if (!file.isEmpty())
        ui->lineEditPath->setText(file);
}

bool SettingsTimerDialog::isSoundEnabled() const { return ui->checkSound->isChecked(); }
bool SettingsTimerDialog::isActionEnabled() const { return ui->checkAction->isChecked(); }
QString SettingsTimerDialog::getActionPath() const { return ui->lineEditPath->text(); }

void SettingsTimerDialog::setSoundEnabled(bool enabled)
{
    ui->checkSound->blockSignals(true);
    ui->checkSound->setChecked(enabled);
    ui->widget_3->setVisible(enabled);
    ui->widget_3->setMaximumHeight(enabled ? ui->widget_3->sizeHint().height() : 0);
    ui->checkSound->blockSignals(false);
}

void SettingsTimerDialog::setActionEnabled(bool enabled)
{
    ui->checkAction->blockSignals(true);
    ui->checkAction->setChecked(enabled);
    ui->widgetFileSelect->setVisible(enabled);
    ui->widgetFileSelect->setMaximumHeight(enabled ? ui->widgetFileSelect->sizeHint().height() : 0);
    ui->checkAction->blockSignals(false);
}

void SettingsTimerDialog::setRecommendationsEnabled(bool enabled)
{
    ui->checkBox->blockSignals(true);
    ui->checkBox->setChecked(enabled);
    ui->widgetRecommendations->setVisible(enabled);
    ui->widgetRecommendations->setMaximumHeight(enabled ? ui->widgetRecommendations->sizeHint().height() : 0);
    ui->checkBox->blockSignals(false);
}

void SettingsTimerDialog::setActionPath(const QString &path)
{
    ui->lineEditPath->setText(path);
}

bool SettingsTimerDialog::continueAfterExit() const
{
    return ui->checkContinueOnExit->isChecked();
}

void SettingsTimerDialog::setContinueAfterExit(bool enabled)
{
    ui->checkContinueOnExit->setChecked(enabled);
}

QString SettingsTimerDialog::melodySoundPath() const { return ui->lineMelodyPath->text(); }
void SettingsTimerDialog::setMelodySoundPath(const QString &path)
{
    ui->lineMelodyPath->setText(path);
    m_melodySoundPath = path;
}

QString SettingsTimerDialog::reminderSoundPath() const { return ui->lineReminderPath->text(); }
void SettingsTimerDialog::setReminderSoundPath(const QString &path)
{
    ui->lineReminderPath->setText(path);
    m_reminderSoundPath = path;
}

bool SettingsTimerDialog::isRecommendationsEnabled() const
{
    return ui->checkBox->isChecked();
}

