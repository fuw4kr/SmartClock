#ifndef CLOCKWINDOW_H
#define CLOCKWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui { class ClockWindow; }
QT_END_NAMESPACE

struct ClockInfo {
    QString zone;
};

class ClockWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ClockWindow(QWidget *parent = nullptr);
    ~ClockWindow();

private slots:
    void updateTime();
    void onAddClock();
    void onRemoveClock();
    void onToggleFormat(bool checked);

private:
    Ui::ClockWindow *ui;
    QTimer timer;
    QList<ClockInfo> clocks;
    bool format12h;

    void updateListTexts();
    QString timeTextFor(const ClockInfo &ci) const;

    void loadFromJson();
    void saveToJson() const;

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
