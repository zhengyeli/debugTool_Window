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
#include <QApplication>

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

// CUSTOM


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
friend class DeviceFinder;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QPushButton * button_scan_sku,*button_ble_send ,*button_blelog_send ,*button_clear ,*button_stop,*button_continue ,*button_discon ;
    QTextEdit *text_sku,*text_ble_send,*text_blelog_send ;
    QListWidget * sku_list;
    QMenuBar* pMenuBar;
    QToolBar* toolbar;

private slots:
    void scanButton_clicked();
    void sendButton_clicked();
    void bleDevlist_itemClicked(QListWidgetItem* i);
    void disconButton_clicked();
    void sendcmdButton_clicked();
    void pauseButton_clicked();
    void continueButton_clicked();
    void clearButton_clicked();

    void menu_action_resetWindow();
    void menu_action_restoreWindow();
    void menu_action_saveWindow();
    void fileSave();
    void showMsg(const QString str);

public:
    Ui::MainWindow *ui;
    static MainWindow *mutualUi;
    void SetTextEdit(int text_index, QString str);
    void SetListView(int list_index, QString str);
    void SetBleLogPrint(QString str);
    void CreatNewView();
    void readSettings();
    void saveSettings();
};
#endif // MAINWINDOW_H
