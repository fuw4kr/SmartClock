
/**
 * @file clockwindow.h
 * @brief Declarations for clockwindow.
 * @details Defines types and functions related to clockwindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef CLOCKWINDOW_H
#define CLOCKWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include "clockmodel.h"
#include "../controllers/clockcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ClockWindow; }
QT_END_NAMESPACE

/**
 * @brief ClockWindow Top-level window UI class.
 * @details Provides window behavior for Clock.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ClockWindow : public QDialog
{
    Q_OBJECT

public:
/**
 * @brief Create ClockWindow instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit ClockWindow(QWidget *parent = nullptr);
/**
 * @brief Destroy ClockWindow instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~ClockWindow();

signals:
/**
 * @brief Add clock requested.
 * @details Modifies the associated collection and notifies listeners.
 * @param zone Time zone identifier.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void addClockRequested(const QString &zone);
/**
 * @brief Remove clocks requested.
 * @details Modifies the associated collection and notifies listeners.
 * @param rows rows value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void removeClocksRequested(const QList<int> &rows);
/**
 * @brief Get formatted string.
 * @details Performs the operation and updates state as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void formatToggled(bool enabled);

public slots:
/**
 * @brief Sync from model.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void syncFromModel();

private slots:
/**
 * @brief Update time.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateTime();
/**
 * @brief On add clock.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onAddClock();
/**
 * @brief On remove clock.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onRemoveClock();
/**
 * @brief Get formatted string.
 * @details Performs the operation and updates state as needed.
 * @param checked checked value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onToggleFormat(bool checked);

private:
    Ui::ClockWindow *ui;
    QTimer timer; /**< Timer-related state. */
    ClockModel *model;
    ClockController *controller;

/**
 * @brief Update list texts.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateListTexts();
/**
 * @brief Time text for.
 * @details Performs the operation and updates state as needed.
 * @param ci ci value.
 * @return Formatted string value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    QString timeTextFor(const ClockInfo &ci) const;

    friend class ClockWindowTest_UpdatesMainTimeLabel_Test;
    friend class ClockWindowTest_AddsAndRemovesClockSuccessfully_Test;
    friend class ClockWindowTest_TimeTextForReturnsFormattedString_Test;
    friend class ClockWindowEdgeTest_HandlesMultipleFormats_Test;
    friend class ClockWindowEdgeTest_HandlesEmptyClockListInSave_Test;
    friend class ClockWindowEdgeTest_TimeTextFormatContainsTimezoneName_Test;
    friend class ClockWindowTest_TogglesFormat12hAndSavesJson_Test;
    friend class ClockWindowTest_SavesAndLoadsClockDataFromJson_Test;
    friend class ClockWindowTest_UpdateListTextsExpandsWhenHasClocks_Test;
    friend class ClockWindowTest_UpdateListTextsHidesListWhenEmpty_Test;
    friend class ClockWindowTest_SavedJsonContainsValidKeys_Test;
    friend class ClockWindowTest_LoadsCorruptedJsonGracefully_Test;
    friend class ClockWindowTest_HandlesEmptyJsonFileGracefully_Test;
    friend class ClockWindowIntegrationTest_SaveLoadCycleKeepsFormatFlag_Test;
    friend class ClockWindowIntegrationTest_AnimationRunsWithoutCrash_Test;
    friend class ClockWindowIntegrationTest_MultipleUpdatesRefreshTimeLabel_Test;
    friend class ClockWindowIntegrationTest_DeleteNonexistentFileDoesNotCrash_Test;
};

#endif // CLOCKWINDOW_H
