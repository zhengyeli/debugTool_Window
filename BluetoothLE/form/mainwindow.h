#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtCore>
#include <QtGui>

#include <QMainWindow>
#include <QApplication>

// BLUETOOTH
#include <bluedevice.h>
#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyService>
#include <QLowEnergyController>

// WIDGET
#include <QWidget>
#include <QRadioButton>
#include <QMenuBar>                   //菜单栏
#include <QMenu>                      //菜单
#include <QAction>                    //菜单项
#include <QDebug>                     //输出
#include <QToolBar>                   // 工具栏
#include <QPushButton>                //按钮
#include <QStatusBar>                 //状态栏
#include <QLabel>                     //标签
#include <QTextEdit>                  //文本编辑区
#include <QDockWidget>                //浮动窗口
#include <QDialog>                    //对话框
#include <QMessageBox>                //标准对话框
#include <QVBoxLayout>                //垂直布局
#include <QHBoxLayout>                //水平布局
#include <QGridLayout>                //栅格布局
#include <QToolButton>                //工具栏按钮
#include <QFileDialog>                //文件对话窗口
#include <QInputDialog>               //
#include <QListWidget>
#include <QListWidgetItem>

// CONFIG
#include <QSettings>
#include <QScreen>

// CUSTOM
#include "tcpsocketclient.h"
#include "bleuartwindow.h"
#include "blelinkwindow.h"
#include "bledebugwindow.h"
#include "bleadvwindow.h"
#include "blesensorwindow.h"
#include "bleskumsghandle.h"

#include "devicefinder.h"
#include "devicehandler.h"
#include "connectionhandler.h"
#include "frmmain.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DeviceInfo;
class DeviceHandler;
class DeviceFinder;


class blelinkwindow ;
class tcpSocketClient ;
class bleUartWindow ;
class bledebugwindow ;

class MainWindow : public QMainWindow
{
    Q_OBJECT
friend class DeviceFinder;

public:
    MainWindow(QWidget *parent = nullptr, QTextEdit *text_info = nullptr);
    ~MainWindow();
    QMenuBar* pMenuBar;
    QToolBar* toolbar;
    //QWidget *toolbar;
    QToolButton *toolBtn = nullptr;

    QDockWidget *DockWidgetsocket, *DockWidgetBleUart, *DockWidgetblelink, *DockWigetbleDebug, *DockwidgetInfo, *DockwidgetWifiConfig;
    QDockWidget *DockWidgetSensor;
    ConnectionHandler *connectionHandler;
    DeviceHandler *deviceHandler;
    DeviceFinder *deviceFinder;

    blelinkwindow *blelink;
    bleskumsghandle *blesku;
    tcpSocketClient *SocketClient;
    bleUartWindow *bleuart;
    bledebugwindow *bledebug;
    bleSensorWindow *bleSensor;
    int ble_data_length = 20;
    QTextEdit *text_info, *text_debug, *tetoutput = nullptr;

public slots:
    void fileSave();
    void resetWindow();
    void ble_rx_data_func(const QByteArray &d);

public:
    Ui::MainWindow *ui;
    static MainWindow *mutualUi;
    void ShowInfo(QString str);
    void SetInfo(QString str);
    void SetListView(QString str);
    void SetBleLogPrint(QString str);
    void readSettings();
    void saveSettings();
    void showMsg(QString str);
    void calGetBleData(QByteArray &, uint8_t *msg);
    void calGetBleData(uint8_t *data);
    void ble_send(QByteArray array); // BB AA
    void ble_char_send(uchar *array);//
    void ble_pt_send(QByteArray array);//

    void creatNewDockWindow(QDockWidget *w, Qt::DockWidgetArea, bool mix);
    void closeAllWindow();
    void selectFunction(int i);
};
#endif // MAINWINDOW_H
