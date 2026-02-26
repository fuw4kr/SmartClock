
/**
 * @file framelessWindow.h
 * @brief Custom frameless window declarations.
 * @details Defines types and functions related to framelessWindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include "snapPreviewWindow.h"

/**
 * @brief FramelessWindow Top-level window UI class.
 * @details Provides window behavior for Frameless.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class FramelessWindow : public QMainWindow {
    Q_OBJECT
public:
/**
 * @brief Create FramelessWindow instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit FramelessWindow(QWidget *parent = nullptr);
/**
 * @brief Destroy FramelessWindow instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~FramelessWindow() override;

    signals:
/**
 * @brief Emitted when window maximized changes.
 * @details Signal emitted when the associated state changes.
 * @param maximized maximized value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
        void windowMaximizedChanged(bool maximized);

protected:
/**
 * @brief ResizeRegion type.
 * @details Holds data and behavior specific to ResizeRegion.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
    enum ResizeRegion {
        None, Left, Right, Top, Bottom, ///< Enum value: none.
        TopLeft, TopRight, BottomLeft, BottomRight
    };
/**
 * @brief Mouse press event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mousePressEvent(QMouseEvent *event) override;
/**
 * @brief Mouse move event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mouseMoveEvent(QMouseEvent *event) override;
/**
 * @brief Mouse release event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mouseReleaseEvent(QMouseEvent *event) override;
/**
 * @brief Mouse double click event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mouseDoubleClickEvent(QMouseEvent *event) override;
/**
 * @brief Resize event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void resizeEvent(QResizeEvent *event) override;
/**
 * @brief Key press event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void keyPressEvent(QKeyEvent *event) override;
/**
 * @brief Detect resize region.
 * @details Performs the operation and updates state as needed.
 * @param pos pos value.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ResizeRegion detectResizeRegion(const QPoint &pos);
/**
 * @brief Update cursor shape.
 * @details Performs the operation and updates state as needed.
 * @param pos pos value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateCursorShape(const QPoint &pos);
/**
 * @brief Toggle maximize restore.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void toggleMaximizeRestore();
/**
 * @brief Toggle fullscreen mode.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void toggleFullscreenMode();
/**
 * @brief Event filter.
 * @details Performs the operation and updates state as needed.
 * @param obj obj value.
 * @param event event value.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool eventFilter(QObject *obj, QEvent *event) override;
/**
 * @brief Enter event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void enterEvent(QEnterEvent *event) override;
/**
 * @brief Leave event.
 * @details Performs the operation and updates state as needed.
 * @param e e value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void leaveEvent(QEvent *e) override;
/**
 * @brief Show event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void showEvent(QShowEvent *event) override;

    bool isFullscreenMode = false; /**< Internal state value. */
    bool m_uiInitialized = false; /**< Internal state value. */
    bool isMaximized = false; /**< Internal state value. */
    bool isDragging = false; /**< Internal state value. */
    bool isResizing = false; /**< Internal state value. */
    QPoint dragOffset; /**< Internal state value. */
    QRect savedGeometryBeforeMaximize; /**< Internal state value. */
    ResizeRegion currentResizeRegion = None; /**< Internal state value. */
    SnapPreviewWindow *snapPreview = nullptr;
private:
/**
 * @brief Set resize cursor.
 * @details Updates internal state and emits signals as needed.
 * @param region region value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setResizeCursor(ResizeRegion region);
};

/**
 * @brief FramelessDialog Modal dialog for configuring settings.
 * @details Provides dialog behavior for Frameless.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class FramelessDialog : public QDialog {
    Q_OBJECT
public:
/**
 * @brief Create FramelessDialog instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @param enableShadow enableShadow value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit FramelessDialog(QWidget *parent, bool enableShadow = true);
/**
 * @brief Destroy FramelessDialog instance.
 * @details Releases owned resources.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ~FramelessDialog() override = default;

/**
 * @brief Set resize enabled.
 * @details Updates internal state and emits signals as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setResizeEnabled(bool enabled) { resizeEnabled = enabled; }
/**
 * @brief Set shadow enabled.
 * @details Updates internal state and emits signals as needed.
 * @param enabled True to enable, false to disable.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void setShadowEnabled(bool enabled) { shadowEnabled = enabled; }

private:
/**
 * @brief ResizeRegion type.
 * @details Holds data and behavior specific to ResizeRegion.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
    enum ResizeRegion {
        None, Left, Right, Top, Bottom, ///< Enum value: none.
        TopLeft, TopRight, BottomLeft, BottomRight
    };
    bool shadowEnabled = true; /**< Internal state value. */
    bool resizeEnabled = true; /**< Internal state value. */
    bool m_uiInitialized = false; /**< Internal state value. */
    bool isDragging = false; /**< Internal state value. */
    bool isResizing = false; /**< Internal state value. */
    QPoint dragOffset; /**< Internal state value. */
    ResizeRegion currentResizeRegion = None; /**< Internal state value. */

protected:
/**
 * @brief Show event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void showEvent(QShowEvent *event) override;
/**
 * @brief Close event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void closeEvent(QCloseEvent *event) override;
/**
 * @brief Mouse press event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mousePressEvent(QMouseEvent *event) override;
/**
 * @brief Mouse move event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mouseMoveEvent(QMouseEvent *event) override;
/**
 * @brief Mouse release event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mouseReleaseEvent(QMouseEvent *event) override;
/**
 * @brief Resize event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void resizeEvent(QResizeEvent *event) override;
/**
 * @brief Apply shadow effect.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void applyShadowEffect();

/**
 * @brief Detect resize region.
 * @details Performs the operation and updates state as needed.
 * @param pos pos value.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    ResizeRegion detectResizeRegion(const QPoint &pos);
/**
 * @brief Update cursor shape.
 * @details Performs the operation and updates state as needed.
 * @param pos pos value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void updateCursorShape(const QPoint &pos);
/**
 * @brief Event filter.
 * @details Performs the operation and updates state as needed.
 * @param obj obj value.
 * @param event event value.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    bool eventFilter(QObject *obj, QEvent *event) override;
/**
 * @brief Enter event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void enterEvent(QEnterEvent *event) override;
/**
 * @brief Leave event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void leaveEvent(QEvent *event) override;
/**
 * @brief Show event fade.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void showEventFade(QShowEvent *event);
};

/**
 * @brief FramelessMessageBox type.
 * @details Holds data and behavior specific to FramelessMessageBox.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class FramelessMessageBox : public FramelessDialog
{
    Q_OBJECT
public:
/**
 * @brief Create FramelessMessageBox instance.
 * @details Initializes instance state.
 * @param title Title string.
 * @param message message value.
 * @param icon icon value.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit FramelessMessageBox(const QString &title,
                                 const QString &message,
                                 QMessageBox::Icon icon = QMessageBox::Information,
                                 QWidget *parent = nullptr);

/**
 * @brief Exec.
 * @details Performs the operation and updates state as needed.
 * @return Integer value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    int exec() override;

/**
 * @brief Get formatted string.
 * @details Performs the operation and updates state as needed.
 * @param parent Parent QObject.
 * @param title Title string.
 * @param message message value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static void information(QWidget *parent,
                            const QString &title,
                            const QString &message);

/**
 * @brief Warning.
 * @details Performs the operation and updates state as needed.
 * @param parent Parent QObject.
 * @param title Title string.
 * @param message message value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static void warning(QWidget *parent,
                        const QString &title,
                        const QString &message);

/**
 * @brief Question.
 * @details Performs the operation and updates state as needed.
 * @param parent Parent QObject.
 * @param title Title string.
 * @param message message value.
 * @return True on success; false otherwise.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static bool question(QWidget *parent,
                         const QString &title,
                         const QString &message);

/**
 * @brief Critical.
 * @details Performs the operation and updates state as needed.
 * @param parent Parent QObject.
 * @param title Title string.
 * @param message message value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    static void critical(QWidget *parent,
                         const QString &title,
                         const QString &message);

protected:
/**
 * @brief Mouse press event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mousePressEvent(QMouseEvent *event) override;
/**
 * @brief Mouse move event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mouseMoveEvent(QMouseEvent *event) override;
/**
 * @brief Mouse release event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void mouseReleaseEvent(QMouseEvent *event) override;

    private slots:
/**
 * @brief On ok clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
        void onOkClicked();
/**
 * @brief On cancel clicked.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void onCancelClicked();

private:
    QLabel *titleLabel = nullptr;
    QLabel *iconLabel = nullptr;
    QLabel *messageLabel = nullptr;
    QPushButton *okButton = nullptr;
    QPushButton *cancelButton = nullptr;
    QGraphicsDropShadowEffect *shadowEffect = nullptr;

    bool isDragging = false; /**< Internal state value. */
    QPoint dragOffset; /**< Internal state value. */
    int resultCode = QMessageBox::Rejected; /**< Internal state value. */
};



#endif // FRAMELESSDIALOG_H