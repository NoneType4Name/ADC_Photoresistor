#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include "CDCusb.hxx"
#include <QMainWindow>
#include <QTimer>
#include <array>
#include <cstdint>
#include <qlist.h>
#include "deps/qcustomplot-source/qcustomplot.h"
#include "CircularBuffer.hxx"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow( QWidget *parent = nullptr );
    ~MainWindow();
    Ui::MainWindow *ui;
    CDCusb *_cdc { 0 };

  private:
    bool _cdcNewLevel { 0 };
    uint16_t level { 4000 };
    QTimer *timer;
    QCustomPlot *plot;
    CircularBuffer<double> plotData { 100 };
    QCPItemStraightLine *plotLevelLine { 0 };

  private slots:
    void timerCallback();
    void on_pushButton_clicked();
    void on_comboBox_currentTextChanged( const QString &arg1 );
    void on_lineEdit_editingFinished();
};
#endif // MAINWINDOW_HXX
