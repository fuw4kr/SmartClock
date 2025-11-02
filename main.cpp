#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"

extern int qInitResources_resources();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInitResources_resources();
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Error", "System tray not available!");
        return -1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();
    return a.exec();
}

