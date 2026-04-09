#include "mainwindow.hxx"
#include <QApplication>

bool r { 1 };

int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    MainWindow w;
    w.show();
    return a.exec();
}
