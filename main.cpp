#include <QApplication>
#include "mainwindow.h"
#include "platform.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("SpicyHorse");
    a.setOrganizationDomain("spicyhorse.com");
    a.setApplicationName("Launcher");
    platformInitialize();

    MainWindow w(&a);
    w.show();

    return a.exec();
}
