#include "CDCusb.hxx"
#include <cassert>
#include <qlogging.h>
#include <QDebug>

CDCusb::CDCusb()
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
}

void CDCusb::setNewLevel( uint16_t l )
{
    DWORD operatedBytes;
    uint8_t high   = ( l >> 8 ) & 0xFF;
    uint8_t low    = l & 0xFF;
    uint8_t d[ 2 ] = { high, low };
    if ( !WriteFile( hCom, &d, 2, &operatedBytes, 0 ) || !operatedBytes )
    {
        qDebug() << "FAILED TO WRITE CDC\n";
        return;
    }
}

CDCusb::~CDCusb()
{
    if ( hCom != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hCom );
        hCom = INVALID_HANDLE_VALUE;
    }
}
