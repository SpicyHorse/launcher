#include <QApplication>
#include <QDir>
#include "mainwindow.h"
#include "platform.h"

int main(int argc, char *argv[])
{
    platformInitialize();

    QApplication a(argc, argv);
#ifndef DEBUG_BUILD
    #if MAC_OS_X_VERSION_10_5
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("plugins");
    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
    #elif WINNT
    QDir dir(QApplication::applicationDirPath());
    dir.cd("plugins");
    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
    #endif
#endif

    MainWindow w(&a);
    w.show();
    w.startUpdate();

    return a.exec();
}
