
/**
 * @file thememanager.h
 * @brief Theme management declarations.
 * @details Defines types and functions related to thememanager.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>

enum class Theme { Light, Dark };

/**
 * @brief ThemeManager Manager that owns and coordinates related objects.
 * @details Provides manager behavior for Theme.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class ThemeManager : public QObject {
    Q_OBJECT ///< Enum value: q object.
public:
/**
 * @brief Instance.
 * @details Performs the operation and updates state as needed.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static ThemeManager& instance();
/**
 * @brief Apply theme.
 * @details Performs the operation and updates state as needed.
 * @param theme theme value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void applyTheme(Theme theme);
/**
 * @brief Current theme.
 * @details Performs the operation and updates state as needed.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    Theme currentTheme() const { return m_theme; }

/**
 * @brief Create ThemeManager instance.
 * @details Initializes instance state.
 * @param ThemeManager ThemeManager value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
/**
 * @brief Create ThemeManager instance.
 * @details Initializes instance state.
 * @param ThemeManager ThemeManager value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ThemeManager(ThemeManager&&) = delete;
    ThemeManager& operator=(ThemeManager&&) = delete;

signals:
/**
 * @brief Emitted when theme changes.
 * @details Signal emitted when the associated state changes.
 * @param styleSheet styleSheet value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void themeChanged(const QString &styleSheet);
private:
/**
 * @brief Create ThemeManager instance.
 * @details Initializes instance state.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ThemeManager() = default;
    Theme m_theme = Theme::Light; /**< Internal state value. */
};

#endif // THEMEMANAGER_H
