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
    uint8_t prevB[ 3 ];
    while ( r )
    {
        uint8_t b[ 3 ] { 0, 0, 0 };
        DWORD operatedBytes;
        uint8_t com[ 2 ] { 0, 0 };
        if ( !WriteFile( _cdc->hCom, &com, 2, &operatedBytes, 0 ) )
        {
            qDebug() << "FAILED TO WRITE CDC\n";
            r = 0;
            return;
        }
        operatedBytes = 0;
        if ( !ReadFile( _cdc->hCom, &b, 2, &operatedBytes, NULL ) )
        {
            qDebug() << "FAILED TO READ CDC\n";
            r = 0;
            return;
        }
        if ( !operatedBytes )
        {
            qDebug() << "BAD PACKET\n";
            continue;
        }
        uint16_t res { static_cast<uint16_t>( ( b[ 0 ] << 8 ) | b[ 1 ] ) };
        // QMetaObject::invokeMethod( ui->lineEdit, "setText", Qt::QueuedConnection, Q_ARG( QString, QString::number( res ) ) );
        QMetaObject::invokeMethod( ui->status, "setText", Qt::QueuedConnection, Q_ARG( QString, res > ui->status_2->text().toInt() ? "ВКЛ" : "ВЫКЛ" ) );
        auto d  = ui->status_2->text().toInt();
        prevPOn = pOn;
        if ( res >= ui->status_2->text().toInt() && !pOn )
        {
            pOn = !pOn;
        }
        else if ( res < ui->status_2->text().toInt() && pOn )
        {
            pOn = !pOn;
        }
        if ( prevPOn != pOn )
            QMetaObject::invokeMethod( ui->status, [ = ]()
                                       {
        ui->status->setProperty("colorState", pOn ? "green":"red");
        ui->status->style()->polish(ui->status); }, Qt::QueuedConnection );
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
    _cdc->setNewLevel( ui->lineEdit->text().toInt() );
}