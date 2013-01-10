#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(&a);
    w.show();
    w.startUpdate();

    return a.exec();
}
