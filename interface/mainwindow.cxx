#include <thread>
#include <QDebug>
#include <QStyle>
#include "mainwindow.hxx"
#include "./ui_mainwindow.h"
#include "CDCusb.hxx"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::MainWindow ), timer( new QTimer( this ) )
{
    ui->setupUi( this );
    ULONG portNumbers[ 256 ];
    ULONG foundCount = 0;
    if ( GetCommPorts( portNumbers, 256, &foundCount ) == ERROR_SUCCESS )
    {
    }
    _cdc = new CDCusb( portNumbers[ 0 ] );
    connect( timer, &QTimer::timeout, this, &MainWindow::timerCallback );
    timer->start( 100 );
}

void MainWindow::timerCallback()
{
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
        }
        if ( !_cdc->read( data, 2 ) )
        {
            qDebug() << "ZERO READED BYTES FROM CDC\n";
            return;
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
            ui->status->setProperty( "colorState", pOn ? "green" : "red" );
            ui->status->style()->polish( ui->status );
            ui->status->setText( res > ui->status_2->text().toInt() ? "ВКЛ" : "ВЫКЛ" );
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    _cdcNewLevel = 1;
}
