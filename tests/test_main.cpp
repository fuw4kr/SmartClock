#include <gtest/gtest.h>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

extern int qInitResources_resources();

int main(int argc, char **argv) {
    qputenv("TEST_MODE", "1");

    QApplication app(argc, argv);
    qInitResources_resources();

    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(base);
    if (dir.exists()) {
        dir.removeRecursively();
    }
    dir.mkpath(base);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
