#include <QApplication>
#include <QMessageBox>

#include "QtSingleApplication"

#include "mainwindow.h"
#include "platform.h"


int main(int argc, char *argv[])
{
    platformInitialize();
    QtSingleApplication a(argc, argv);

    if (a.isRunning()) {
        QMessageBox::critical(0, "Launcher is already running", "Launcher is already running.");
        return 1;
    }

    MainWindow w(&a);
    w.show();

    return a.exec();
}
