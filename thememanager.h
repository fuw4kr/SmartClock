#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>

enum class Theme { Light, Dark };

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();
    void applyTheme(Theme theme);
    Theme currentTheme() const { return m_theme; }

    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
    ThemeManager(ThemeManager&&) = delete;
    ThemeManager& operator=(ThemeManager&&) = delete;

signals:
    void themeChanged(const QString &styleSheet);
private:
    ThemeManager() = default;
    Theme m_theme = Theme::Light;
};

#endif // THEMEMANAGER_H
