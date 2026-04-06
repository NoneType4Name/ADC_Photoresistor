#include "CDC.hxx"
#include "ui_mainwindow.h"
#include <cassert>
#include <cstdint>
#include <qdebug.h>
#include <qlogging.h>
#include <qobject.h>
#include <thread>
#include <QDebug>
#include <QStyle>
void threadUpd( CDC *_cdc )
{
    bool pOn { 0 };
    bool prevPOn { pOn };
    uint8_t prevB[ 3 ];
    while ( _cdc->r )
    {
        uint8_t b[ 3 ] { 0, 0, 0 };
        DWORD operatedBytes;
        uint8_t com[ 2 ] { 0, 0 };
        if ( !WriteFile( _cdc->hCom, &com, 2, &operatedBytes, 0 ) )
        {
            qDebug() << "FAILED TO WRITE CDC\n";
            _cdc->r = 0;
            return;
        }
        operatedBytes = 0;
        while ( !operatedBytes )
            if ( !ReadFile( _cdc->hCom, &b, 3, &operatedBytes, NULL ) )
            {
                qDebug() << "FAILED TO READ CDC\n";
                _cdc->r = 0;
                return;
            }
        if ( b[ 2 ] != '\n' )
        {
            qDebug() << "BAD PACKET\n";
            continue;
        }
        uint16_t res { static_cast<uint16_t>( ( b[ 0 ] << 8 ) | b[ 1 ] ) };
        // QMetaObject::invokeMethod( _cdc->wnd.ui->lineEdit, "setText", Qt::QueuedConnection, Q_ARG( QString, QString::number( res ) ) );
        QMetaObject::invokeMethod( _cdc->wnd.ui->status, "setText", Qt::QueuedConnection, Q_ARG( QString, res > _cdc->wnd.ui->status_2->text().toInt() ? "ВКЛ" : "ВЫКЛ" ) );
        auto d  = _cdc->wnd.ui->status_2->text().toInt();
        prevPOn = pOn;
        if ( res >= _cdc->wnd.ui->status_2->text().toInt() && !pOn )
        {
            pOn = !pOn;
        }
        else if ( res < _cdc->wnd.ui->status_2->text().toInt() && pOn )
        {
            pOn = !pOn;
        }
        if ( prevPOn != pOn )
            QMetaObject::invokeMethod( _cdc->wnd.ui->status, [ = ]()
                                       {
        _cdc->wnd.ui->status->setProperty("colorState", pOn ? "green":"red");
        _cdc->wnd.ui->status->style()->polish(_cdc->wnd.ui->status); }, Qt::QueuedConnection );
    }
}

CDC::CDC( MainWindow &w ) :
    wnd( w )
{
    hCom = CreateFileA( "\\\\.\\COM6",
                        GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING, 0, NULL );

    if ( hCom == INVALID_HANDLE_VALUE )
        qDebug() << "INVALID WIN API HANDLE\n";
    COMMTIMEOUTS newTimeouts;
    newTimeouts.ReadIntervalTimeout         = MAXDWORD;
    newTimeouts.ReadTotalTimeoutConstant    = 50;
    newTimeouts.ReadTotalTimeoutMultiplier  = 10;
    newTimeouts.WriteTotalTimeoutConstant   = 50;
    newTimeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts( hCom, &newTimeouts );
    DCB dcb       = { 0 };
    dcb.DCBlength = sizeof( dcb );
    GetCommState( hCom, &dcb );

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;
    SetCommState( hCom, &dcb );

    assert( SetCommState( hCom, &dcb ) );
    th = std::thread( [ & ]()
                      { threadUpd( this ); } );
}
CDC::~CDC()
{
    if ( hCom != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hCom );
        hCom = INVALID_HANDLE_VALUE;
    }
    r = 0;
    while ( !th.joinable() )
    {
    }
    th.join();
}

void CDC::setNewLevel( uint16_t l ) {} // todo
