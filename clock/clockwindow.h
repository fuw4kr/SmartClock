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

class ClockWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ClockWindow(QWidget *parent = nullptr);
    ~ClockWindow();

signals:
    void addClockRequested(const QString &zone);
    void removeClocksRequested(const QList<int> &rows);
    void formatToggled(bool enabled);

public slots:
    void syncFromModel();

private slots:
    void updateTime();
    void onAddClock();
    void onRemoveClock();
    void onToggleFormat(bool checked);

private:
    Ui::ClockWindow *ui;
    QTimer timer;
    ClockModel *model;
    ClockController *controller;

    void updateListTexts();
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
