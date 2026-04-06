#include "mainwindow.h"
#include "CDC.hxx"
#include <QApplication>

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    MainWindow w;
    CDC usb { w };
    w.show();
    return a.exec();
}
