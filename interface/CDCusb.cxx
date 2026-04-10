#include "CDCusb.hxx"
#include <cstdlib>
#include <qlogging.h>
#include <QDebug>

CDCusb::CDCusb( QString name )
{
    hCom = CreateFileA( name.toStdString().c_str(),
                        GENERIC_READ | GENERIC_WRITE,
                        0, NULL, OPEN_EXISTING, 0, NULL );

    if ( hCom == INVALID_HANDLE_VALUE )
    {
        qDebug() << "INVALID WIN API HANDLE\n";
        hCom = 0;
        return;
    }

    COMMTIMEOUTS newTimeouts;
    newTimeouts.ReadIntervalTimeout         = MAXDWORD;
    newTimeouts.ReadTotalTimeoutConstant    = 50;
    newTimeouts.ReadTotalTimeoutMultiplier  = 10;
    newTimeouts.WriteTotalTimeoutConstant   = 50;
    newTimeouts.WriteTotalTimeoutMultiplier = 10;
    if ( !SetCommTimeouts( hCom, &newTimeouts ) )
    {
        qDebug() << "FAILED TO SET TIMEOUTS\n";
        CloseHandle( hCom );
        hCom = 0;
        return;
    }

    DCB dcb       = { 0 };
    dcb.DCBlength = sizeof( dcb );
    if ( !GetCommState( hCom, &dcb ) )
    {
        qDebug() << "FAILED TO GET COMM STATE\n";
        CloseHandle( hCom );
        hCom = 0;
        return;
    }

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;
    SetCommState( hCom, &dcb );
    if ( !SetCommState( hCom, &dcb ) )
    {
        qDebug() << "FAILED TO SET COMM STATE\n";
        CloseHandle( hCom );
        hCom = 0;
        return;
    }
}

uint32_t CDCusb::write( uint8_t *buf, uint32_t len )
{
    DWORD operatedBytes;
    if ( !WriteFile( hCom, buf, len, &operatedBytes, 0 ) )
    {
        qDebug() << "FAILED TO WRITE CDC\n";
        return 0;
    }
    return operatedBytes;
}

uint32_t CDCusb::read( uint8_t *buf, uint32_t len )
{
    DWORD operatedBytes;
    if ( !ReadFile( hCom, buf, len, &operatedBytes, NULL ) )
    {
        qDebug() << "FAILED TO READ CDC\n";
        return 0;
    }
    return operatedBytes;
}

CDCusb::~CDCusb()
{
    if ( hCom && ( hCom != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( hCom );
        hCom = INVALID_HANDLE_VALUE;
    }
}
