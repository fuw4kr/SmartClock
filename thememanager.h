#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>

enum class Theme { Light, Dark };

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();
    void applyTheme(Theme theme);
    Theme currentTheme() const { return m_theme; }
private:
    ThemeManager() = default;
    Theme m_theme = Theme::Light;
};

#endif // THEMEMANAGER_H
