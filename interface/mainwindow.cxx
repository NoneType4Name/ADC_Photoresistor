#include <thread>
#include <QDebug>
#include <QStyle>
#include "mainwindow.hxx"
#include "./ui_mainwindow.h"
#include "CDCusb.hxx"

MainWindow::MainWindow( CDCusb *_cdc, QWidget *parent ) :
    QMainWindow( parent ), _cdc( _cdc ), ui( new Ui::MainWindow )
{
    ui->setupUi( this );
    th = std::thread( [ & ]()
                      { threadUpd(); } );
}

void MainWindow::threadUpd()
{
    bool pOn { 0 };
    bool prevPOn { pOn };
    uint8_t prevB[ 2 ];
    while ( r )
    {
        uint8_t data[ 2 ] { 0, 0 };
        if ( _cdcNewLevel )
        {
            _cdc->setNewLevel( ui->lineEdit->text().toUInt() );
            _cdcNewLevel = 0;
        }
        else if ( !_cdc->write( data, 2 ) )
        {
            qDebug() << "FAILED TO WRITE CDC (ZERO WRITTEN BYTES)\n";
            continue;
            // r = 0;
            // return;
        }
        if ( !_cdc->read( data, 2 ) )
        {
            qDebug() << "ZERO READED BYTES FROM CDC\n";
            continue;
            // r = 0;
            // return;
        }
        uint16_t res { static_cast<uint16_t>( ( data[ 0 ] << 8 ) | data[ 1 ] ) };
        prevPOn = pOn;
        if ( res >= ui->status_2->text().toUInt() && !pOn )
        {
            pOn = !pOn;
        }
        else if ( res < ui->status_2->text().toUInt() && pOn )
        {
            pOn = !pOn;
        }
        if ( prevPOn != pOn )
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
    r = 0;
    while ( !th.joinable() )
    {
    }
    th.join();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    _cdcNewLevel = 1;
}
