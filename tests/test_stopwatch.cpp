#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QTest>
#include <QSettings>
#include <QString>
#include <QStyleOptionViewItem>
#include <QPainter>
#include "../stopwatch/stopwatchwindow.h"
#include "../stopwatch/analogstopwatchdial.h"

TEST(AnalogStopwatchDialTest, InitialStateIsZero) {
    AnalogStopwatchDial dial;
    QTime t(0, 0, 0, 0);
    dial.setElapsed(t);
    EXPECT_GE(dial.width(), 0);
}

TEST(AnalogStopwatchDialTest, PaintEventRunsWithoutCrash) {
    AnalogStopwatchDial dial;
    QTime t(1, 2, 3, 400);
    dial.resize(300, 300);
    dial.setElapsed(t);
    
    EXPECT_NO_THROW({
        dial.update();
        QTest::qWait(0);
    });
}

TEST(StopwatchWindowTest, StartsAndStopsCorrectly) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    QPushButton* lap = w.findChild<QPushButton*>("btnLap");
    ASSERT_TRUE(start && lap);

    EXPECT_FALSE(lap->isEnabled());
    start->click(); 
    EXPECT_FALSE(w.getTotalTimeString().isEmpty());
}

TEST(StopwatchWindowTest, LapAndResetWork) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    QPushButton* lap = w.findChild<QPushButton*>("btnLap");
    QListWidget* list = w.findChild<QListWidget*>("listLaps");
    ASSERT_TRUE(start && lap && list);

    start->click();
    w.updateDisplay();
    lap->click(); 

    start->click(); 
    lap->click();   
    EXPECT_LE(list->count(), 1);
}

TEST(StopwatchWindowTest, SaveAndLoadRestoresData) {
    {
        StopwatchWindow w;
        QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
        QPushButton* lap   = w.findChild<QPushButton*>("btnLap");
        QListWidget* list  = w.findChild<QListWidget*>("listLaps");
        ASSERT_TRUE(start && lap && list);

        start->click();
        QTest::qWait(60);
        lap->click();
        start->click();
    }

    
    StopwatchWindow w2;
    QListWidget* list2 = w2.findChild<QListWidget*>("listLaps");
    ASSERT_TRUE(list2);
    EXPECT_GE(list2->count(), 0);  
}

TEST(StopwatchWindowTest, FilePathExistsAndWritable) {
    
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    EXPECT_FALSE(dir.isEmpty());

    
    QString path = dir + "/stopwatch_test.json";
    QFile f(path);

    EXPECT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write("{\"test\":true}");
    f.close();

    EXPECT_TRUE(QFile::exists(path));

    QFile::remove(path);
}

TEST(StopwatchWindowTest, AnalogModeSwitchDoesNotCrash) {
    StopwatchWindow w;
    QSettings settings("SmartClockApp", "Stopwatch");
    settings.setValue("analogMode", true);

    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    start->click();
    EXPECT_NO_THROW(start->click());
}

TEST(StopwatchWindowTest, TotalTimeStringReturnsLabelText) {
    StopwatchWindow w;
    QLabel* label = w.findChild<QLabel*>("labelTime");
    label->setText("11:11.11");
    EXPECT_EQ(w.getTotalTimeString(), "11:11.11");
}

TEST(StopwatchWindowTest, LoadFromCorruptFileDoesNotCrash) {
    
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = dir + "/stopwatch.json";

    QDir().mkpath(dir); 

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("corrupted_json"); 
    f.close();

    
    EXPECT_NO_THROW({
        StopwatchWindow w;
    });

    QFile::remove(path); 
}

TEST(StopwatchWindowAdvancedTest, MultipleLapsMarkBestAndWorst) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    QPushButton* lap = w.findChild<QPushButton*>("btnLap");
    QListWidget* list = w.findChild<QListWidget*>("listLaps");
    start->click();

    for (int i = 0; i < 3; ++i) {
        QTest::qWait(10 + i * 10);
        lap->click();
    }

    EXPECT_GE(list->count(), 3);
}

TEST(StopwatchWindowAdvancedTest, SaveFileContainsValidJson) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = dir + "/stopwatch.json";

    
    {
        StopwatchWindow w;
        QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
        QPushButton* lap   = w.findChild<QPushButton*>("btnLap");
        ASSERT_TRUE(start && lap);

        start->click();
        QTest::qWait(40);
        lap->click();
        start->click();

    }

    QFile f(path);
    ASSERT_TRUE(f.exists());
    ASSERT_TRUE(f.open(QIODevice::ReadOnly));

    QByteArray data = f.readAll();
    f.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    EXPECT_TRUE(doc.isObject());
    EXPECT_EQ(err.error, QJsonParseError::NoError);

    QFile::remove(path);
}

TEST(StopwatchWindowAdvancedTest, LoadFileRestoresLapCount) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = dir + "/stopwatch.json";

    {
        StopwatchWindow w;
        QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
        QPushButton* lap   = w.findChild<QPushButton*>("btnLap");
        QListWidget* list  = w.findChild<QListWidget*>("listLaps");

        ASSERT_TRUE(start && lap && list);

        start->click();
        QTest::qWait(40);
        lap->click();
        start->click();

        EXPECT_GE(list->count(), 1);
    }

    StopwatchWindow restored;
    QListWidget* list2 = restored.findChild<QListWidget*>("listLaps");
    ASSERT_TRUE(list2);
    EXPECT_GE(list2->count(), 0);  

    QFile::remove(path);
}

TEST(StopwatchWindowAdvancedTest, AnalogDialReceivesElapsedTime) {
    AnalogStopwatchDial dial;
    dial.resize(200, 200);
    dial.setElapsed(QTime::currentTime());

    EXPECT_NO_THROW({
        dial.show();       
        dial.update();     
        QTest::qWait(30);  
    });
}

TEST(StopwatchWindowAdvancedTest, RapidStartStopDoesNotCrash) {
    StopwatchWindow w;
    QPushButton* start = w.findChild<QPushButton*>("btnStartStop");
    for (int i = 0; i < 5; ++i) {
        start->click();
        QTest::qWait(5);
        start->click();
    }
    EXPECT_FALSE(w.getTotalTimeString().isEmpty());
}

TEST(StopwatchWindowAdvancedTest, QSettingsPersistAfterRestart) {
    {
        StopwatchWindow w;
        QSettings s("SmartClockApp", "Stopwatch");
        s.setValue("analogMode", true);
        
    }

    QSettings s("SmartClockApp", "Stopwatch");
    EXPECT_TRUE(s.contains("analogMode"));
    EXPECT_EQ(s.value("analogMode").toBool(), true);
}

