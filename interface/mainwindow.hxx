#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include "CDCusb.hxx"
#include <QMainWindow>
#include <QTimer>

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
    CDCusb *_cdc;

  private:
    // bool r { 1 };
    // std::thread th;
    bool _cdcNewLevel { 0 };
    QTimer *timer;

  private slots:
    void timerCallback();
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_HXX
