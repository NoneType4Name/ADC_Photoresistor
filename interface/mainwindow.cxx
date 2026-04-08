#include <thread>
#include <QDebug>
#include <QStyle>
#include "mainwindow.hxx"
#include "./ui_mainwindow.h"
#include "CDCusb.hxx"

MainWindow::MainWindow( CDCusb *_cdc, QWidget *parent ) :
    QMainWindow( parent ), _cdc( _cdc ), ui( new Ui::MainWindow ), timer( new QTimer( this ) )
{
    ui->setupUi( this );
    connect( timer, &QTimer::timeout, this, &MainWindow::timerCallback );
    timer->start( 100 );
    // th = std::thread( [ & ]()
    //                   { timerCallback(); } );
}

void MainWindow::timerCallback()
{
    // bool prevPOn { pOn };
    // uint8_t prevB[ 2 ];
    // while ( r )
    {
        uint8_t data[ 2 ] { 0, 0 };
        if ( _cdcNewLevel )
        {
            _cdc->setNewLevel( ui->lineEdit->text().toUInt() );
            _cdcNewLevel = 0;
            return;
        }
        else if ( !_cdc->write( data, 2 ) )
        {
            qDebug() << "FAILED TO WRITE CDC (ZERO WRITTEN BYTES)\n";
            return;
            // r = 0;
            // return;
        }
        if ( !_cdc->read( data, 2 ) )
        {
            qDebug() << "ZERO READED BYTES FROM CDC\n";
            return;
            // r = 0;
            // return;
        }
        uint16_t res { static_cast<uint16_t>( ( data[ 0 ] << 8 ) | data[ 1 ] ) };
        bool pOn;
        if ( res > ui->status_2->text().toUInt() )
        {
            pOn = 1;
        }
        else if ( res < ui->status_2->text().toUInt() )
        {
            pOn = 0;
        }
        if ( res != ui->status_2->text().toUInt() )
        {
            QMetaObject::invokeMethod( ui->status, [ = ]()
                                       {
                ui->status->setProperty("colorState", pOn ? "green":"red");
                ui->status->style()->polish(ui->status); }, Qt::QueuedConnection );
            QMetaObject::invokeMethod( ui->status, "setText", Qt::QueuedConnection, Q_ARG( QString, res > ui->status_2->text().toInt() ? "ВКЛ" : "ВЫКЛ" ) );
        }
    }
}

MainWindow::~MainWindow()
{
    // r = 0;
    // while ( !th.joinable() )
    // {
    // }
    // th.join();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    _cdcNewLevel = 1;
}
