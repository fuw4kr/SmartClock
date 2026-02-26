/**
 * @file test_windowedit.cpp
 * @brief Definitions for test_windowedit.
 * @details Implements logic declared in the corresponding header for test_windowedit.
 * @note Keep implementation and header documentation consistent.
 * @warning Update documentation when behavior changes.
 * @sa SmartClock
 * @date 2026-02-26
 */

#include <gtest/gtest.h>
#include <QPixmap>
#include <QApplication>
#include <QTimer>
#include <QTest>
#include <QMouseEvent>
#include <QPushButton>
#include <QGraphicsEffect>
#include <QScreen>
#include <QEnterEvent>
#include <QListWidget>
#include <QPropertyAnimation>
#include "../windowEdit/framelessWindow.h"
#include "../windowEdit/snapPreviewWindow.h"

FramelessMessageBox* getOpenMessageBox() {
    for (QWidget* widget : QApplication::topLevelWidgets()) {
        if (auto msgBox = qobject_cast<FramelessMessageBox*>(widget)) {
            if (msgBox->isVisible()) {
                return msgBox;
            }
        }
    }
    return nullptr;
}

class FramelessWindowTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FramelessWindowTest, MessageBoxInformation) {
    QTimer::singleShot(250, []() {
        auto msgBox = getOpenMessageBox();
        ASSERT_TRUE(msgBox != nullptr);
        auto btns = msgBox->findChildren<QPushButton*>();
        for (auto btn : btns) {
            if (btn->text() == "OK") {
                btn->click();
                return;
            }
        }
        msgBox->reject(); // Фолбек, якщо кнопку не знайдено
    });
    FramelessMessageBox::information(nullptr, "Info Test", "This is info.");
    SUCCEED();
}

TEST_F(FramelessWindowTest, MessageBoxWarning) {
    QTimer::singleShot(250, []() {
        auto msgBox = getOpenMessageBox();
        ASSERT_TRUE(msgBox != nullptr);
        auto btns = msgBox->findChildren<QPushButton*>();
        for (auto btn : btns) if (btn->text() == "OK") btn->click();
    });

    FramelessMessageBox::warning(nullptr, "Warning Test", "This is warning.");
    SUCCEED();
}

TEST_F(FramelessWindowTest, MessageBoxQuestion_Accept) {
    QTimer::singleShot(250, []() {
        auto msgBox = getOpenMessageBox();
        ASSERT_TRUE(msgBox != nullptr);
        auto btns = msgBox->findChildren<QPushButton*>();
        for (auto btn : btns) if (btn->text() == "OK") btn->click();
    });

    bool result = FramelessMessageBox::question(nullptr, "Question Test", "Yes or No?");
    EXPECT_TRUE(result); // OK повертає true
}

TEST_F(FramelessWindowTest, MessageBoxQuestion_Reject) {
    QTimer::singleShot(250, []() {
        auto msgBox = getOpenMessageBox();
        ASSERT_TRUE(msgBox != nullptr);
        auto btns = msgBox->findChildren<QPushButton*>();
        for (auto btn : btns) if (btn->text() == "Cancel") btn->click();
    });

    bool result = FramelessMessageBox::question(nullptr, "Question Test", "Yes or No?");
    EXPECT_FALSE(result); // Cancel повертає false
}

TEST_F(FramelessWindowTest, MessageBoxCritical) {
    QTimer::singleShot(250, []() {
        auto msgBox = getOpenMessageBox();
        ASSERT_TRUE(msgBox != nullptr);
        auto btns = msgBox->findChildren<QPushButton*>();
        for (auto btn : btns) if (btn->text() == "Cancel") btn->click();
    });

    FramelessMessageBox::critical(nullptr, "Critical Test", "This is critical.");
    SUCCEED();
}

TEST_F(FramelessWindowTest, FullscreenToggleOnF4) {
    FramelessWindow w;
    w.resize(400, 400);
    w.show();
    QTest::qWait(100); // Чекаємо відображення вікна

    bool initiallyFullscreen = w.isFullScreen();
    QTest::keyClick(&w, Qt::Key_F4);
    QTest::qWait(300); 
    
    EXPECT_NE(initiallyFullscreen, w.isFullScreen());
    QTest::keyClick(&w, Qt::Key_F4);
    QTest::qWait(300);

    EXPECT_EQ(initiallyFullscreen, w.isFullScreen());
}

TEST_F(FramelessWindowTest, DragWindowByTitleBar) {
    FramelessWindow w;
    QWidget* titleBar = new QWidget(&w);
    titleBar->setObjectName("titleBar");
    titleBar->setGeometry(0, 0, 800, 36);
    w.resize(800, 600);
    w.show();
    QTest::qWait(100);

    QPoint startPos = w.pos();
    QPoint pressPos = titleBar->mapToGlobal(QPoint(10, 10));
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(10,10), pressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &pressEvent);
    QPoint movePos = pressPos + QPoint(50, 50);
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(60,60), movePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &moveEvent);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(60,60), movePos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &releaseEvent);
    EXPECT_NE(startPos, w.pos());
}

TEST_F(FramelessWindowTest, ResizeWindowFromTopLeft) {
    FramelessWindow w;
    w.resize(400, 400);
    w.show();
    QTest::qWait(100);

    int startWidth = w.width();
    QPoint pressPos = w.mapToGlobal(QPoint(2, 2)); // Координати TopLeft margin (зона ресайзу)
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(2,2), pressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &pressEvent);
    QPoint movePos = pressPos + QPoint(-50, -50);
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(-48,-48), movePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &moveEvent);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(-48,-48), movePos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &releaseEvent);
    EXPECT_GT(w.width(), startWidth);
}

TEST_F(FramelessWindowTest, DoubleClickTitleBarMaximizes) {
    FramelessWindow w;
    QWidget* titleBar = new QWidget(&w);
    titleBar->setObjectName("titleBar");
    titleBar->setGeometry(0, 0, 800, 36);
    w.resize(400, 400);
    w.show();
    QTest::qWait(100);

    QPoint clickPos = titleBar->mapToGlobal(QPoint(20, 15));
    QMouseEvent dblClickEvent(QEvent::MouseButtonDblClick, QPoint(20, 15), clickPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &dblClickEvent);
    
    QTest::qWait(100);
    

    EXPECT_GT(w.width(), 400); 
}

TEST_F(FramelessWindowTest, DialogHasShadowWhenEnabled) {
    FramelessDialog d(nullptr, true); // Shadow enabled
    EXPECT_TRUE(d.graphicsEffect() != nullptr);
}

TEST_F(FramelessWindowTest, DialogResizeFromBottomRight) {
    FramelessDialog d(nullptr, false);
    d.setResizeEnabled(true);
    d.resize(300, 300);
    d.show();
    QTest::qWait(100);

    int startHeight = d.height();
    QPoint pressPos = d.mapToGlobal(QPoint(298, 298)); // BottomRight corner

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(298,298), pressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &pressEvent);

    QPoint movePos = pressPos + QPoint(50, 50);
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(348,348), movePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &moveEvent);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(348,348), movePos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &releaseEvent);

    EXPECT_GT(d.height(), startHeight);
}



TEST_F(FramelessWindowTest, SnapToScreenEdges) {
    FramelessWindow w;
    QWidget* titleBar = new QWidget(&w);
    titleBar->setObjectName("titleBar");
    titleBar->setGeometry(0, 0, 800, 36);
    w.resize(400, 400);
    w.show();
    QTest::qWait(100);

    QScreen *screen = QGuiApplication::primaryScreen();
    ASSERT_TRUE(screen != nullptr);
    QRect avail = screen->availableGeometry();
    QPoint pressPos = titleBar->mapToGlobal(QPoint(20, 10));
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(20, 10), pressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &pressEvent);
    QPoint topEdgePos(avail.x() + 100, avail.y()); // Координата на самому верху
    QMouseEvent moveTop(QEvent::MouseMove, w.mapFromGlobal(topEdgePos), topEdgePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &moveTop);
    QMouseEvent releaseTop(QEvent::MouseButtonRelease, w.mapFromGlobal(topEdgePos), topEdgePos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &releaseTop);

    QTest::qWait(50);
    EXPECT_EQ(w.geometry(), avail); // Вікно мало стати на весь доступний екран (isMaximized)
    pressPos = titleBar->mapToGlobal(QPoint(20, 10));
    QMouseEvent pressEvent2(QEvent::MouseButtonPress, QPoint(20, 10), pressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &pressEvent2);

    QPoint leftEdgePos(avail.x(), avail.y() + 100);
    QMouseEvent moveLeft(QEvent::MouseMove, w.mapFromGlobal(leftEdgePos), leftEdgePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &moveLeft);

    QMouseEvent releaseLeft(QEvent::MouseButtonRelease, w.mapFromGlobal(leftEdgePos), leftEdgePos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &releaseLeft);

    QTest::qWait(50);
    EXPECT_EQ(w.width(), avail.width() / 2); // Має зайняти рівно ліву половину
}
TEST_F(FramelessWindowTest, DialogCursorUpdatesAndEnterLeave) {
    FramelessDialog d(nullptr, false);
    d.setResizeEnabled(true);
    d.resize(300, 300);
    d.show();
    QTest::qWait(100);
    QPoint rightEdge(298, 150);
    QMouseEvent moveEvent(QEvent::MouseMove, rightEdge, d.mapToGlobal(rightEdge), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &moveEvent);
    EXPECT_EQ(d.cursor().shape(), Qt::SizeHorCursor); // Right краю відповідає горизонтальний курсор

    QPoint bottomEdge(150, 298);
    QMouseEvent moveEvent2(QEvent::MouseMove, bottomEdge, d.mapToGlobal(bottomEdge), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &moveEvent2);
    EXPECT_EQ(d.cursor().shape(), Qt::SizeVerCursor); // Bottom краю відповідає вертикальний курсор
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEnterEvent enterEvent(QPointF(150, 150), QPointF(150, 150), d.mapToGlobal(QPoint(150, 150)));
#else
    QEvent enterEvent(QEvent::Enter);
#endif
    QApplication::sendEvent(&d, &enterEvent);
    EXPECT_EQ(d.cursor().shape(), Qt::ArrowCursor);

    QEvent leaveEvent(QEvent::Leave);
    QApplication::sendEvent(&d, &leaveEvent);
    EXPECT_EQ(d.cursor().shape(), Qt::ArrowCursor);
}

TEST_F(FramelessWindowTest, DialogDragRespectsScreenBounds) {
    FramelessDialog d(nullptr, false);
    QWidget* titleBar = new QWidget(&d);
    titleBar->setObjectName("titleBar");
    titleBar->setGeometry(0, 0, 300, 36);
    d.resize(300, 300);
    d.show();
    QTest::qWait(100);

    QScreen *screen = QGuiApplication::primaryScreen();
    ASSERT_TRUE(screen != nullptr);
    QRect avail = screen->availableGeometry();
    QPoint pressPos = titleBar->mapToGlobal(QPoint(10, 10));
    QMouseEvent press(QEvent::MouseButtonPress, QPoint(10,10), pressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &press);
    QPoint outOfBoundsTop = QPoint(pressPos.x(), avail.y() - 100);
    QMouseEvent moveUp(QEvent::MouseMove, d.mapFromGlobal(outOfBoundsTop), outOfBoundsTop, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &moveUp);
    EXPECT_GE(d.y(), avail.y());
    QMouseEvent release(QEvent::MouseButtonRelease, d.mapFromGlobal(outOfBoundsTop), outOfBoundsTop, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&d, &release);
}

TEST_F(FramelessWindowTest, MessageBoxDragging) {
    FramelessMessageBox msgBox("Title", "Message", QMessageBox::Information, nullptr);
    msgBox.show();
    QTest::qWait(100);

    QPoint startPos = msgBox.pos();
    QPoint clickLocal(10, 10);
    QPoint clickGlobal = msgBox.mapToGlobal(clickLocal);
    QMouseEvent press(QEvent::MouseButtonPress, clickLocal, clickGlobal, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&msgBox, &press);
    QPoint dragGlobal = clickGlobal + QPoint(100, 100);
    QPoint dragLocal = msgBox.mapFromGlobal(dragGlobal);
    QMouseEvent move(QEvent::MouseMove, dragLocal, dragGlobal, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&msgBox, &move);
    QMouseEvent release(QEvent::MouseButtonRelease, dragLocal, dragGlobal, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&msgBox, &release);
    EXPECT_NE(msgBox.pos(), startPos);

    msgBox.close(); // Очищуємо після себе
}

class SnapPreviewTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SnapPreviewTest, PaintEventCoverage) {
    SnapPreviewWindow preview;
    QScreen *screen = QGuiApplication::primaryScreen();
    preview.showPreview(SnapPreviewWindow::SnapType::Left, screen);
    QPixmap pixmap(preview.size());
    pixmap.fill(Qt::transparent);
    preview.render(&pixmap);
    EXPECT_FALSE(pixmap.isNull());
    preview.hidePreview();
    EXPECT_EQ(preview.currentType(), SnapPreviewWindow::SnapType::None);
    EXPECT_FALSE(preview.isVisible());
}

TEST_F(FramelessWindowTest, ResizeClampsToMinimumSize) {
    FramelessWindow w;
    w.setMinimumSize(200, 200);
    w.resize(400, 400);
    w.show();
    QTest::qWait(100);

    // Імітуємо натискання на лівий край (Left)
    QPoint pressPos = w.mapToGlobal(QPoint(2, 200));
    QMouseEvent press(QEvent::MouseButtonPress, QPoint(2,200), pressPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &press);

    // Тягнемо лівий край сильно вправо (щоб ширина стала меншою за 200)
    QPoint movePos = pressPos + QPoint(300, 0);
    QMouseEvent move(QEvent::MouseMove, w.mapFromGlobal(movePos), movePos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &move);

    QMouseEvent release(QEvent::MouseButtonRelease, w.mapFromGlobal(movePos), movePos, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&w, &release);

    // Перевіряємо, що ширина не стала меншою за встановлений мінімум
    EXPECT_EQ(w.width(), 200);
}

TEST_F(FramelessWindowTest, ShowEventEnablesMouseTrackingForChildren) {
    FramelessWindow w;
    QListWidget* list = new QListWidget(&w);
    w.setCentralWidget(list);

    EXPECT_FALSE(list->hasMouseTracking()); // Спочатку false

    w.show();
    QTest::qWait(50);

    // Після showEvent() FramelessWindow має увімкнути tracking для QListWidget
    EXPECT_TRUE(list->hasMouseTracking());
}

TEST_F(FramelessWindowTest, DialogCloseEventTriggersFadeOutAnimation) {
    FramelessDialog d(nullptr, false);
    d.show();
    QTest::qWait(100);

    EXPECT_EQ(d.windowOpacity(), 1.0);

    // Викликаємо закриття, що має запустити анімацію згасання
    d.close();

    // Чекаємо половину часу анімації (приблизно 90ms з 180ms)
    QTest::qWait(90);

    // Прозорість має бути десь посередині
    EXPECT_LT(d.windowOpacity(), 1.0);
    EXPECT_GT(d.windowOpacity(), 0.0);

    // Чекаємо завершення анімації (із запасом)
    QTest::qWait(250);
    EXPECT_FALSE(d.isVisible());
}

TEST_F(FramelessWindowTest, MessageBoxDirectExecutionReturnsCorrectCodes) {
    // Тестуємо повернення Accepted при кліку на OK
    FramelessMessageBox boxOk("Title", "Msg", QMessageBox::Information, nullptr);
    QTimer::singleShot(100, [&boxOk]() {
        for (auto btn : boxOk.findChildren<QPushButton*>()) {
            if (btn->text() == "OK") btn->click();
        }
    });
    EXPECT_EQ(boxOk.exec(), QMessageBox::Accepted);

    // Тестуємо повернення Rejected при кліку на Cancel
    FramelessMessageBox boxCancel("Title", "Msg", QMessageBox::Question, nullptr);
    QTimer::singleShot(100, [&boxCancel]() {
        for (auto btn : boxCancel.findChildren<QPushButton*>()) {
            if (btn->text() == "Cancel") btn->click();
        }
    });
    EXPECT_EQ(boxCancel.exec(), QMessageBox::Rejected);
}

TEST_F(FramelessWindowTest, MessageBoxClickingButtonDoesNotStartDrag) {
    FramelessMessageBox msgBox("Title", "Msg", QMessageBox::Question, nullptr);
    msgBox.show();
    QTest::qWait(100);

    // Знаходимо кнопку OK
    QPushButton* btn = nullptr;
    for (auto b : msgBox.findChildren<QPushButton*>()) {
        if (b->text() == "OK") btn = b;
    }
    ASSERT_TRUE(btn != nullptr);

    QPoint startPos = msgBox.pos();
    QPoint clickLocal = btn->geometry().center();
    QPoint clickGlobal = msgBox.mapToGlobal(clickLocal);

    // Клікаємо саме по кнопці
    QMouseEvent press(QEvent::MouseButtonPress, clickLocal, clickGlobal, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&msgBox, &press);

    QPoint moveGlobal = clickGlobal + QPoint(50, 50);
    QMouseEvent move(QEvent::MouseMove, msgBox.mapFromGlobal(moveGlobal), moveGlobal, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(&msgBox, &move);

    QMouseEvent release(QEvent::MouseButtonRelease, msgBox.mapFromGlobal(moveGlobal), moveGlobal, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(&msgBox, &release);

    // Вікно не повинно було зміститися, оскільки клік був по кнопці
    EXPECT_EQ(msgBox.pos(), startPos);

    msgBox.close();
}

TEST_F(SnapPreviewTest, RightEdgeGeometryCalculatedCorrectly) {
    SnapPreviewWindow preview;
    QScreen *screen = QGuiApplication::primaryScreen();
    ASSERT_TRUE(screen != nullptr);

    QRect screenRect = screen->availableGeometry();
    preview.showPreview(SnapPreviewWindow::SnapType::Right, screen);

    // Перевіряємо, що вікно перемкнулося в правильний режим
    EXPECT_EQ(preview.currentType(), SnapPreviewWindow::SnapType::Right);

    // Перевіряємо логіку позиціонування з допуском у 2 пікселі (через можливі округлення DPI)
    int expectedX = screenRect.x() + screenRect.width() / 2;
    EXPECT_GE(preview.geometry().x(), expectedX - 2);
    EXPECT_LE(preview.geometry().x(), expectedX + 2);

    // Перевіряємо, що ширина вікна відповідає приблизно половині екрана
    int expectedWidth = screenRect.width() / 2;
    EXPECT_GE(preview.width(), expectedWidth - 2);
    EXPECT_LE(preview.width(), expectedWidth + 2);
}