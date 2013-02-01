#include <QApplication>
#include "mainwindow.h"
#include "platform.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    platformInitialize();
    MainWindow w(&a);
    w.show();

    return a.exec();
}
