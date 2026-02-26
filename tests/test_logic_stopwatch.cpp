#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QFile>
#include "../stopwatch/stopwatchmodel.h"
#include "../stopwatch/jsonstopwatchstorage.h"

namespace {

struct MemoryStopwatchData {
    StopwatchSnapshot snapshot;
    bool has = false;
};

class MemoryStopwatchStorage : public IStopwatchStorage
{
public:
    explicit MemoryStopwatchStorage(MemoryStopwatchData *data) : data(data) {}

    bool load(StopwatchSnapshot &out) override
    {
        if (!data || !data->has)
            return false;
        out = data->snapshot;
        return true;
    }

    bool save(const StopwatchSnapshot &in) override
    {
        if (!data)
            return false;
        data->snapshot = in;
        data->has = true;
        return true;
    }

private:
    MemoryStopwatchData *data = nullptr;
};

} // namespace

TEST(StopwatchModelTest, StartStopAndTickUpdatesElapsed) {
    StopwatchModel model;
    EXPECT_FALSE(model.isRunning());
    model.start();
    EXPECT_TRUE(model.isRunning());
    model.tick(150);
    model.stop();
    EXPECT_FALSE(model.isRunning());
    EXPECT_GE(model.elapsedMs(), 150);
}

TEST(StopwatchModelTest, AddLapWhileRunningAddsEntry) {
    StopwatchModel model;
    model.start();
    model.tick(100);
    model.addLap();
    EXPECT_EQ(model.lapDurations().size(), 1);
}

TEST(StopwatchModelTest, ResetClearsState) {
    StopwatchModel model;
    model.start();
    model.tick(200);
    model.addLap();
    model.reset();
    EXPECT_FALSE(model.isRunning());
    EXPECT_EQ(model.elapsedMs(), 0);
    EXPECT_TRUE(model.lapDurations().isEmpty());
}

TEST(StopwatchModelTest, SaveLoadRoundTrip) {
    MemoryStopwatchData data;
    StopwatchModel model(nullptr, std::make_unique<MemoryStopwatchStorage>(&data));
    model.start();
    model.tick(120);
    model.addLap();
    model.stop();

    EXPECT_TRUE(model.save());

    StopwatchModel reload(nullptr, std::make_unique<MemoryStopwatchStorage>(&data));
    EXPECT_TRUE(reload.load());
    EXPECT_EQ(reload.lapDurations().size(), 1);
    EXPECT_EQ(reload.elapsedMs(), model.elapsedMs());
}

TEST(JsonStopwatchStorageTest, SavesAndLoadsSnapshot) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/stopwatch.json";

    JsonStopwatchStorage storage(path);
    StopwatchSnapshot in;
    in.elapsedMs = 1234;
    in.running = true;
    in.lapDurations.append(400);
    in.lapDurations.append(834);
    ASSERT_TRUE(storage.save(in));

    StopwatchSnapshot out;
    ASSERT_TRUE(storage.load(out));
    EXPECT_EQ(out.elapsedMs, 1234);
    EXPECT_TRUE(out.running);
    EXPECT_EQ(out.lapDurations.size(), 2);
}

TEST(JsonStopwatchStorageTest, SaveDefaultRemovesFile) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/stopwatch.json";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Truncate));
    f.write("{\"dummy\":true}");
    f.close();
    ASSERT_TRUE(QFile::exists(path));

    JsonStopwatchStorage storage(path);
    StopwatchSnapshot in;
    EXPECT_TRUE(storage.save(in));
    EXPECT_FALSE(QFile::exists(path));
}

TEST(JsonStopwatchStorageTest, LoadsCorruptedJsonGracefully) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = dir.path() + "/stopwatch.json";

    QFile f(path);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("corrupted_json");
    f.close();

    JsonStopwatchStorage storage(path);
    StopwatchSnapshot out;
    EXPECT_TRUE(storage.load(out));
    EXPECT_EQ(out.elapsedMs, 0);
    EXPECT_FALSE(out.running);
    EXPECT_EQ(out.lapDurations.size(), 0);
}
