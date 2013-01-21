#include <QApplication>
#include <QDir>
#include "mainwindow.h"
#include "platform.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("SpyciHorse");
    a.setOrganizationDomain("spicyhorse.com");
    a.setApplicationName("Launcher");

    platformInitialize();

    MainWindow w(&a);
    w.show();

    return a.exec();
}
