
/**
 * @file snapPreviewWindow.h
 * @brief Snap preview window declarations.
 * @details Defines types and functions related to snapPreviewWindow.
 * @note Keep this file in sync with related declarations/definitions.
 * @warning Update documentation when API changes.
 * @sa SmartClock
 * @date 2026-02-26
 */
#ifndef SNAPPREVIEWWINDOW_H
#define SNAPPREVIEWWINDOW_H

#include <QWidget>
#include <QScreen>
#include <QTimer>
#include <QPainter>

/**
 * @brief SnapPreviewWindow Top-level window UI class.
 * @details Provides window behavior for SnapPreview.
 * @note Public API is documented per member.
 * @warning Respect ownership and lifetime rules.
 * @sa SmartClock
 */
class SnapPreviewWindow : public QWidget
{
    Q_OBJECT
public:
    enum class SnapType { None, Top, Left, Right };

/**
 * @brief Create SnapPreviewWindow instance.
 * @details Initializes instance state.
 * @param parent Parent QObject.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    explicit SnapPreviewWindow(QWidget *parent = nullptr);

/**
 * @brief Show preview.
 * @details Performs the operation and updates state as needed.
 * @param type type value.
 * @param screen screen value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void showPreview(SnapType type, QScreen *screen);
/**
 * @brief Hide preview.
 * @details Performs the operation and updates state as needed.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void hidePreview();
/**
 * @brief Current type.
 * @details Performs the operation and updates state as needed.
 * @return Return value of the operation.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    SnapType currentType() const { return currentSnap; }

protected:
/**
 * @brief Paint event.
 * @details Performs the operation and updates state as needed.
 * @param event event value.
 * @note Validate inputs where applicable.
 * @sa SmartClock
 */
    void paintEvent(QPaintEvent *event) override;

private:
    SnapType currentSnap = SnapType::None; /**< Internal state value. */
    QColor borderColor = QColor(59,130,246,200);
    QColor fillColor = QColor(59,130,246,40);
    QRect lastRect; /**< Internal state value. */
};

#endif // SNAPPREVIEWWINDOW_H
