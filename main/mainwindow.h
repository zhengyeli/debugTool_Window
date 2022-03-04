#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "Window/bleuartwindow.h"
#include "Window/blelinkwindow.h"
#include "Window/bledebugwindow.h"
#include "Window/bleconfigwifi.h"

#include "devicefinder.h"
#include "devicehandler.h"
#include "connectionhandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DeviceInfo;
class DeviceHandler;
class DeviceFinder;

class MainWindow : public QMainWindow
{
    Q_OBJECT
friend class DeviceFinder;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMenuBar* pMenuBar;
    QToolBar* toolbar;

    QDockWidget *DockWidgetsocket, *DockWidgetBleUart, *DockWidgetblelink, *DockWigetbleDebug, *DockwidgetInfo, *DockwidgetWifiConfig;
    ConnectionHandler *connectionHandler;
    DeviceHandler *deviceHandler;
    DeviceFinder *deviceFinder;
    QTextEdit *text_info, *text_debug;

private slots:
    void fileSave();

    void menu_action_resetWindow();
    void menu_action_restoreWindow();
    void menu_action_saveWindow();
    void menu_action_closeAllWindow();

public:
    Ui::MainWindow *ui;
    static MainWindow *mutualUi;
    void SetInfo(QString str);
    void SetListView(QString str);
    void SetBleLogPrint(QString str);
    void readSettings();
    void saveSettings();
    void showMsg(QString str);
    QByteArray calGetBleData(QByteArray);
    void ble_send(QByteArray array); // BB AA
    void ble_char_send(uchar *array);//
    void ble_debug_send(QByteArray array); //bb aa

    void creatNewDockWindow(QDockWidget *w, Qt::DockWidgetArea, bool mix);
    void closeAllWindow();
};
#endif // MAINWINDOW_H
