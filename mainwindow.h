#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>

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

#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QPushButton * button_scan_sku,*button_ble_send ,*button_blelog_send ,*button_clear ,*button_stop,*button_continue ,*button_discon ;
    QTextEdit *text_sku,*text_ble_send,*text_blelog_send ;
    QListWidget * sku_list;
private slots:
    void on_pushButton_scan_clicked();
    void on_pushButton_send_clicked();
    void on_listWidget_bleDev_itemClicked(QListWidgetItem *item);
    void on_pushButton_discon_clicked();
    void on_pushButton_sendcmd_clicked();
    void on_pushButton_pause_clicked();
    void on_Button_contiunue_clicked();
    void on_pushButton_clear_clicked();

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
