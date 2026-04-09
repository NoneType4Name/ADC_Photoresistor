#include "CDCusb.hxx"
#include <cassert>
#include <cstdlib>
#include <qlogging.h>
#include <QDebug>

CDCusb::CDCusb( QString name )
{
    hCom = CreateFileA( name.toStdString().c_str(),
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
}

uint32_t CDCusb::write( uint8_t *buf, uint32_t len )
{
    DWORD operatedBytes;
    assert( WriteFile( hCom, buf, len, &operatedBytes, 0 ) );
    return operatedBytes;
}

uint32_t CDCusb::read( uint8_t *buf, uint32_t len )
{
    DWORD operatedBytes;
    assert( ReadFile( hCom, buf, len, &operatedBytes, NULL ) );
    return operatedBytes;
}

CDCusb::~CDCusb()
{
    if ( hCom != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hCom );
        hCom = INVALID_HANDLE_VALUE;
    }
}
