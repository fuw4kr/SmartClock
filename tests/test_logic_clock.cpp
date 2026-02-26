/**
 * @file test_logic_clock.cpp
 * @brief Definitions for test_logic_clock.
 * @details Implements logic declared in the corresponding header for test_logic_clock.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "../clock/clockmodel.h"
#include "../clock/jsonclockstorage.h"

namespace {

struct MemoryClockData {
    ClockSnapshot snapshot;
    bool has = false;
};

class MemoryClockStorage : public IClockStorage
{
public:
    explicit MemoryClockStorage(MemoryClockData *data) : data(data) {}

    bool load(ClockSnapshot &out) override
    {
        if (!data || !data->has)
            return false;
        out = data->snapshot;
        return true;
    }

    bool save(const ClockSnapshot &in) override
    {
        if (!data)
            return false;
        data->snapshot = in;
        data->has = true;
        return true;
    }

private:
    MemoryClockData *data = nullptr;
};

} // namespace

TEST(ClockModelTest, SaveLoadRoundTrip) {
    MemoryClockData data;
    ClockModel model(nullptr, std::make_unique<MemoryClockStorage>(&data));
    model.addClock("America/New_York");
    model.addClock("Asia/Tokyo");
    model.setFormat12h(true);

    EXPECT_TRUE(model.save());

    ClockModel reload(nullptr, std::make_unique<MemoryClockStorage>(&data));
    EXPECT_TRUE(reload.load());
    EXPECT_EQ(reload.clocks().size(), 2);
    EXPECT_TRUE(reload.format12h());
}

TEST(ClockModelTest, HandlesEmptyClockListInSave) {
    MemoryClockData data;
    ClockModel model(nullptr, std::make_unique<MemoryClockStorage>(&data));
    EXPECT_TRUE(model.save());
}

TEST(ClockModelTest, TimeTextForContainsZoneAndTime) {
    ClockModel model;
    ClockInfo ci{"UTC"};
    QString text = model.timeTextFor(ci);
    EXPECT_TRUE(text.startsWith("UTC"));
    EXPECT_TRUE(text.contains(":"));
}

TEST(ClockModelTest, ToggleFormatUpdatesState) {
    ClockModel model;
    model.setFormat12h(true);
    EXPECT_TRUE(model.format12h());
    model.setFormat12h(false);
    EXPECT_FALSE(model.format12h());
}

TEST(JsonClockStorageTest, SavesAndLoadsClockDataFromJson) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/clocks.json";

    JsonClockStorage storage(path);
    ClockSnapshot in;
    in.format12h = true;
    in.clocks.append(ClockInfo{"America/New_York"});
    in.clocks.append(ClockInfo{"Asia/Tokyo"});
    ASSERT_TRUE(storage.save(in));

    ClockSnapshot out;
    ASSERT_TRUE(storage.load(out));
    EXPECT_EQ(out.clocks.size(), 2);
    EXPECT_TRUE(out.format12h);
}

TEST(JsonClockStorageTest, SavedJsonContainsValidKeys) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/clocks.json";

    JsonClockStorage storage(path);
    ClockSnapshot in;
    in.clocks.append(ClockInfo{"Europe/Kyiv"});
    ASSERT_TRUE(storage.save(in));

    QFile f(path);
    ASSERT_TRUE(f.exists());
    ASSERT_TRUE(f.open(QIODevice::ReadOnly));
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    f.close();

    ASSERT_TRUE(doc.isObject());
    QJsonObject root = doc.object();
    EXPECT_TRUE(root.contains("clocks"));
    EXPECT_TRUE(root.contains("format12h"));
}

TEST(JsonClockStorageTest, LoadsCorruptedJsonGracefully) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/clocks.json";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("{not valid json");
    f.close();

    JsonClockStorage storage(path);
    ClockSnapshot out;
    EXPECT_TRUE(storage.load(out));
    EXPECT_EQ(out.clocks.size(), 0);
    EXPECT_FALSE(out.format12h);
}

TEST(JsonClockStorageTest, HandlesEmptyJsonFileGracefully) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/clocks.json";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.close();

    JsonClockStorage storage(path);
    ClockSnapshot out;
    EXPECT_TRUE(storage.load(out));
    EXPECT_EQ(out.clocks.size(), 0);
    EXPECT_FALSE(out.format12h);
}

TEST(JsonClockStorageTest, DeleteNonexistentFileDoesNotCrash) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/missing.json";

    JsonClockStorage storage(path);
    ClockSnapshot out;
    EXPECT_FALSE(storage.load(out));
}
