#include <array>
#include <cstdint>
#include <string>
#include <thread>
#include <QDebug>
#include <QStyle>
#include "mainwindow.hxx"
#include "./ui_mainwindow.h"
#include "CDCusb.hxx"
#include "deps/qcustomplot-source/qcustomplot.h"
#include <QSerialPortInfo>

static std::array<uint16_t, 100> timeLine;

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::MainWindow ), timer( new QTimer( this ) )
{
    ui->setupUi( this );
    auto l { QSerialPortInfo::availablePorts() };
    for ( size_t i { 0 }; i < l.size(); ++i )
    {
        ui->comboBox->addItem( l[ i ].portName() );
        ui->comboBox->setItemData( i, l[ i ].description(), Qt::ToolTipRole );
    }
    for ( size_t i { 0 }; i < 100; ++i )
        timeLine[ i ] = i * 50;
    plot = new QCustomPlot( ui->centralwidget );
    plot->addGraph();
    plot->graph( 0 )->setPen( QPen( Qt::red, 2 ) );
    plot->graph( 0 )->setBrush( QBrush( QColor( 255, 0, 0, 30 ) ) );

    plot->xAxis->setLabel( "time (s)" );
    plot->yAxis->setLabel( "ADC value" );
    plot->xAxis->setRange( 0, 5000 ); // msec
    plot->yAxis->setRange( 0, 5000 ); // adc amplitude

    plot->xAxis->grid()->setPen( QPen( QColor( 200, 200, 200 ), 0, Qt::DotLine ) );
    plot->yAxis->grid()->setPen( QPen( QColor( 200, 200, 200 ), 0, Qt::DotLine ) );

    connect( timer, &QTimer::timeout, this, &MainWindow::timerCallback );
    timer->start( 50 );
}

void MainWindow::timerCallback()
{
    {
        uint8_t data[ 2 ] { 0, 0 };

        if ( !_cdc )
        {
            plot->graph( 0 )->setData( ( timeLine.data() ), plotData.data() );
            return;
        }
        if ( _cdcNewLevel )
        {
            level          = static_cast<uint16_t>( ui->lineEdit->text().toUInt() );
            uint8_t high   = ( level >> 8 ) & 0xFF;
            uint8_t low    = level & 0xFF;
            uint8_t d[ 2 ] = { high, low };
            if ( !_cdc->write( d, 2 ) )
            {
                qDebug() << "FAILED TO WRITE CDC (ZERO WRITTEN BYTES)\n";
                return;
            }
            if ( !_cdc->read( d, 2 ) )
            {
                qDebug() << "FAILED TO READ CDC\n";
                return;
            }
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
        ui->label->setText( QString::number( res ) );
        bool pOn;
        if ( res >= level )
        {
            pOn = 1;
        }
        else if ( res < level )
        {
            pOn = 0;
        }

        ui->status->setProperty( "colorState", pOn ? "green" : "red" );
        ui->status->style()->polish( ui->status );
        ui->status->setText( pOn ? "Enable" : "Disable" );
    }
}

MainWindow::~MainWindow()
{
    delete _cdc;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    // dummy <33
}

void MainWindow::on_comboBox_currentTextChanged( const QString &arg1 )
{
    if ( _cdc )
    {
        delete _cdc;
    }
    _cdc = new CDCusb( arg1 );
}

void MainWindow::on_lineEdit_editingFinished()
{
    _cdcNewLevel = 1;
}
