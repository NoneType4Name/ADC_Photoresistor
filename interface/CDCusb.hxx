#pragma once
#ifndef CDCUSB_HXX
#    define CDCUSB_HXX
#    include <windows.h>
#    include <cstdint>

class CDCusb
{
  public:
    CDCusb();
    void setNewLevel( uint16_t l );
    HANDLE hCom;
    ~CDCusb();
};
#endif