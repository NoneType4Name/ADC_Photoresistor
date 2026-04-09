#pragma once
#ifndef CDCUSB_HXX
#    define CDCUSB_HXX
#    include <windows.h>
#    include <cstdint>
#    include <QString>

class CDCusb
{
  public:
    CDCusb( QString name );
    uint32_t write( uint8_t *buf, uint32_t len );
    uint32_t read( uint8_t *buf, uint32_t len );
    HANDLE hCom;
    ~CDCusb();
};
#endif