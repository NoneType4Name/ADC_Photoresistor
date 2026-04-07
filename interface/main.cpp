#include "mainwindow.hxx"
#include "CDCusb.hxx"
#include <QApplication>

bool r { 1 };

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    CDCusb cdc;
    MainWindow w( &cdc );
    w.show();
    return a.exec();
}
