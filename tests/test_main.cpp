/**
 * @file test_main.cpp
 * @brief Definitions for test_main.
 * @details Implements logic declared in the corresponding header for test_main.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

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
