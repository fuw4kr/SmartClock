#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include "snapPreviewWindow.h"

class FramelessWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *parent = nullptr);
    ~FramelessWindow() override;

    signals:
        void windowMaximizedChanged(bool maximized);

protected:
    enum ResizeRegion {
        None, Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    ResizeRegion detectResizeRegion(const QPoint &pos);
    void updateCursorShape(const QPoint &pos);
    void toggleMaximizeRestore();
    void toggleFullscreenMode();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *e) override;
    void showEvent(QShowEvent *event) override;

    bool isFullscreenMode = false;
    bool m_uiInitialized = false;
    bool isMaximized = false;
    bool isDragging = false;
    bool isResizing = false;
    QPoint dragOffset;
    QRect savedGeometryBeforeMaximize;
    ResizeRegion currentResizeRegion = None;
    SnapPreviewWindow *snapPreview = nullptr;
private:
    void setResizeCursor(ResizeRegion region);
};

class FramelessDialog : public QDialog {
    Q_OBJECT
public:
    explicit FramelessDialog(QWidget *parent, bool enableShadow = true);
    ~FramelessDialog() override = default;

    void setResizeEnabled(bool enabled) { resizeEnabled = enabled; }
    void setShadowEnabled(bool enabled) { shadowEnabled = enabled; }

private:
    enum ResizeRegion {
        None, Left, Right, Top, Bottom,
        TopLeft, TopRight, BottomLeft, BottomRight
    };
    bool shadowEnabled = true;
    bool resizeEnabled = true;
    bool m_uiInitialized = false;
    bool isDragging = false;
    bool isResizing = false;
    QPoint dragOffset;
    ResizeRegion currentResizeRegion = None;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void applyShadowEffect();

    ResizeRegion detectResizeRegion(const QPoint &pos);
    void updateCursorShape(const QPoint &pos);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void showEventFade(QShowEvent *event);
};

class FramelessMessageBox : public FramelessDialog
{
    Q_OBJECT
public:
    explicit FramelessMessageBox(const QString &title,
                                 const QString &message,
                                 QMessageBox::Icon icon = QMessageBox::Information,
                                 QWidget *parent = nullptr);

    int exec() override;

    static void information(QWidget *parent,
                            const QString &title,
                            const QString &message);

    static void warning(QWidget *parent,
                        const QString &title,
                        const QString &message);

    static bool question(QWidget *parent,
                         const QString &title,
                         const QString &message);

    static void critical(QWidget *parent,
                         const QString &title,
                         const QString &message);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    private slots:
        void onOkClicked();
    void onCancelClicked();

private:
    QLabel *titleLabel = nullptr;
    QLabel *iconLabel = nullptr;
    QLabel *messageLabel = nullptr;
    QPushButton *okButton = nullptr;
    QPushButton *cancelButton = nullptr;
    QGraphicsDropShadowEffect *shadowEffect = nullptr;

    bool isDragging = false;
    QPoint dragOffset;
    int resultCode = QMessageBox::Rejected;
};



#endif // FRAMELESSDIALOG_H