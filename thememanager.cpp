/**
 * @file thememanager.cpp
 * @brief Definitions for thememanager.
 * @details Implements logic declared in the corresponding header for thememanager.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include "thememanager.h"
#include <QFile>
#include <QDebug>

ThemeManager& ThemeManager::instance() {
    static ThemeManager manager;
    return manager;
}

void ThemeManager::applyTheme(Theme theme)
{
    m_theme = theme;
    QString path = (theme == Theme::Light)
                       ? ":/style/lightTheme.qss"
                       : ":/style/darkTheme.qss";

    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString style = file.readAll();
        emit themeChanged(style);
        qDebug() << "themeChanged: " << path;
        return;
    }
    qDebug() << "theme load failed: " << path;
}
