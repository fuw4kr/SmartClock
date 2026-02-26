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
