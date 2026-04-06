#pragma once
#ifndef CDC_HXX
#    define CDC_HXX

#    include <thread>
#    include <QWidget>
#    include "mainwindow.h"
#    include <windows.h>

class CDC
{
  public:
    CDC( MainWindow &w );
    void setNewLevel( uint16_t l );
    ~CDC();

  private:
    MainWindow &wnd;
    std::thread th;
    HANDLE hCom;
    bool r { 1 };
    friend void threadUpd( CDC * );
};
#endif