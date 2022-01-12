#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QBluetoothLocalDevice>
#include <QBluetoothUuid>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyService>
#include <QLowEnergyController>

#include <bluedevice.h>

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>

#include "devicefinder.h"
#include "devicehandler.h"
#include "deviceinfo.h"
#include "connectionhandler.h"

#include <QWidget>
#include <QVBoxLayout>                      //垂直布局
#include <QHBoxLayout>                      //水平布局
#include <QGridLayout>                      //栅格布局

#include <QCloseEvent>

#include <newqdockwidget.h>

MainWindow *MainWindow::mutualUi = nullptr;//！！！！初始化，非常重要

ConnectionHandler connectionHandler;
DeviceHandler deviceHandler;
DeviceFinder deviceFinder(&deviceHandler);
QTextEdit *MyDockQTE = nullptr;
QPushButton *QClearButton = nullptr;
QTextEdit *MyDockQTE_bleinfo = nullptr;

QDockWidget *dock = nullptr;
QDockWidget *dock1 = nullptr;
QDockWidget *dock2 = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mutualUi = this;//！！！赋值，非常重要
    readSettings();
    QWidget* p = takeCentralWidget();   //删除中央窗体，使用QDockWidget代替
    if (p)
        delete p;
    dock = new QDockWidget(this);
    setDockNestingEnabled(true);        //允许嵌套dock
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable); // 设置可移动
    dock->setWindowTitle("连接蓝牙窗口");
    //dock->setFeatures(QDockWidget::DockWidgetFloatable); // 设置可浮动
    //dock->setFeatures(QDockWidget::DockWidgetClosable); // 设置可关闭
    //dock->setFeatures(QDockWidget::NoDockWidgetFeatures); // 设置可关闭
    //dock->setFeatures(QDockWidget::AllDockWidgetFeatures); // 以上三种都包含
    //dock->setGeometry(0,0,250,200);

    // 进行布局
    addDockWidget(Qt::LeftDockWidgetArea,dock);

    QWidget *dockWidgetContents;
    dockWidgetContents = new QWidget(dock);
    //dockWidgetContents->setGeometry(QRect(10, 10, 100, 400)); // 从屏幕上（10，10）位置开始（即为最左上角的点），显示一个30*35的界面（宽30，高35）

    /* 所有控件 */
    text_sku = new QTextEdit(dockWidgetContents);
        text_sku->setText("7160");
        //text_sku->setMinimumWidth(60); //宽度
        //text_sku->setMaximumWidth(70);
        text_sku->setMinimumHeight(30);
        text_sku->setMaximumHeight(30);
    text_ble_send = new QTextEdit(dockWidgetContents);
        text_ble_send->setText("aa01");
        text_ble_send->setMinimumHeight(30);
        text_ble_send->setMaximumHeight(30);
    text_blelog_send = new QTextEdit(dockWidgetContents);
        text_blelog_send->setText("govee");
        text_blelog_send->setMinimumHeight(30);
        text_blelog_send->setMaximumHeight(30);
    button_scan_sku = new QPushButton(dockWidgetContents);
        button_scan_sku->setText("scan");
    button_ble_send = new QPushButton(dockWidgetContents);
        button_ble_send->setText("send");
    button_blelog_send = new QPushButton(dockWidgetContents);
        button_blelog_send->setText("log send");
    button_clear = new QPushButton(dockWidgetContents);
        button_clear->setText("clear");
    button_stop = new QPushButton(dockWidgetContents);
        button_stop->setText("stop");
    button_continue = new QPushButton(dockWidgetContents);
        button_continue->setText("continue");
    sku_list= new QListWidget(dockWidgetContents);
    button_discon = new QPushButton(dockWidgetContents);
        button_discon->setText("disconnect");

    //new QPushButton();     在空板上增加按键
    //new QPushButton(this); 在当前板上增加按键
    //new QPushButton(dock); 在dock板上增加按键

    /* 左布局 */
    QVBoxLayout *left_verticalLayout;
    left_verticalLayout = new QVBoxLayout();
    left_verticalLayout->setSpacing(20);  //间距
    left_verticalLayout->setContentsMargins(11, 11, 11, 11);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    //left_verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    left_verticalLayout->addWidget(text_sku);
    left_verticalLayout->addWidget(text_ble_send);
    left_verticalLayout->addWidget(text_blelog_send);

    /* 右布局 */
    QVBoxLayout *right_verticalLayout;
    right_verticalLayout = new QVBoxLayout();
    right_verticalLayout->setSpacing(20);  //间距
    right_verticalLayout->setContentsMargins(11, 11, 11, 11);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    right_verticalLayout->addWidget(button_scan_sku);
    right_verticalLayout->addWidget(button_ble_send);
    right_verticalLayout->addWidget(button_blelog_send);

    /* 将两个垂直布局放入水平布局 */
    QHBoxLayout *horizontalLayout;
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(10);  //间距
    horizontalLayout->setContentsMargins(11, 11, 11, 11);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    horizontalLayout->addItem(left_verticalLayout);
    horizontalLayout->addItem(right_verticalLayout);

    /* 将剩余按钮放入水平布局 */
    QHBoxLayout *horizontalLayout1;
    horizontalLayout1 = new QHBoxLayout();
    horizontalLayout1->setSpacing(10);  //间距
    horizontalLayout1->setContentsMargins(11, 11, 11, 11);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    horizontalLayout1->addWidget(button_clear);
    horizontalLayout1->addWidget(button_discon);
    horizontalLayout1->addWidget(button_stop);
    horizontalLayout1->addWidget(button_continue);

    /* 将两个水平布局放入新的垂直布局 */
    QVBoxLayout *updown_verticalLayout;
    updown_verticalLayout = new QVBoxLayout(dockWidgetContents);
    updown_verticalLayout->addItem(horizontalLayout);
    updown_verticalLayout->addItem(horizontalLayout1);
    updown_verticalLayout->addWidget(sku_list);

    //绑定控件的回调函数;
    connect(button_scan_sku,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_scan_clicked()));
    connect(sku_list,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(on_listWidget_bleDev_itemClicked(QListWidgetItem *)));
    connect(button_ble_send,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_send_clicked()));
    connect(button_blelog_send,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_sendcmd_clicked()));
    connect(button_clear,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_clear_clicked()));
    connect(button_stop,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_pause_clicked()));
    connect(button_continue,SIGNAL(clicked(bool)),this,SLOT(on_Button_contiunue_clicked()));
    connect(button_discon,SIGNAL(clicked(bool)),this,SLOT(on_pushButton_discon_clicked()));

    dock->setWidget(dockWidgetContents);


    CreatNewView();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::SetTextEdit(int index, QString str)
{
    if (!MyDockQTE_bleinfo){
        return;
    }

    MyDockQTE_bleinfo->append(str);
    //ui->textEdit_log->append(str);
}

void MainWindow::SetListView(int index, QString str)
{
    if (str == "clear"){
        sku_list->clear();
        return;
    }
    sku_list->addItem(str);
    qDebug() << str;
}

void MainWindow::on_pushButton_scan_clicked()
{
    deviceFinder.sku = text_sku->toPlainText();
    sku_list->clear(); //清除sku list
    deviceFinder.startSearch();
}

void MainWindow::SetBleLogPrint(QString str)
{
    if (str == ""){
        return;
    }
    MyDockQTE->append(str);
}

//选中sku时触发
void MainWindow::on_listWidget_bleDev_itemClicked(QListWidgetItem *item)
{
    qDebug() << item->data(0);
    qDebug() << item->data(0).toString();
    deviceFinder.connectToService(item->data(0).toString());
    //deviceFinder.connectToService("ihoment_H7160_07F7");
}


void MainWindow::on_pushButton_send_clicked()
{
    QByteArray array;
    uint8_t parsedValue;
    uint8_t senddata[20] = {0};
    QString data = text_ble_send->toPlainText();
    //qDebug() << data.toUtf8(); //"aa11"
    array = data.toUtf8(); //"aa11"

    for (int i = 0; i < array.length(); i++)
    {
        if ((i % 2) == 0) //字节的高四位
        {
            if (array[i] > '0' && array[i] <= '9')
               parsedValue = (array[i] - '0') * 16;
            if (array[i] >= 'A' && array[i] <= 'F')
               parsedValue = (array[i] - ('A'-'9' - 1) - '0') * 16;
            if (array[i] >= 'a' && array[i] <= 'f')
               parsedValue = (array[i] - ('a'-'9' - 1) - '0') * 16;
        }else{ //字节的低四位
            if (array[i] >= '0' && array[i] <= '9')
                senddata[i/2] = parsedValue + (array[i] - '0');
            if (array[i] >= 'A' && array[i] <= 'F')
                senddata[i/2] = parsedValue + (array[i] - ('A'-'9' - 1) - '0');
            if (array[i] >= 'a' && array[i] <= 'f')
                senddata[i/2] = parsedValue + (array[i] - ('a'-'9' - 1) - '0');
            parsedValue = 0;
        }
    }
    deviceHandler.calculate(senddata);
    array = QByteArray((char*)senddata,20);
    deviceHandler.characteristicWrite(deviceHandler.setChar,array);
}


void MainWindow::on_pushButton_discon_clicked()
{
    deviceHandler.disconnectDevice();
}

void MainWindow::on_pushButton_sendcmd_clicked()
{
    QByteArray array;
    QString data = text_blelog_send->toPlainText();
    array = data.toUtf8(); //"aa11"
    qDebug() << array;
    deviceHandler.characteristicWrite(deviceHandler.bledebugsetChar,array);
}

void MainWindow::on_pushButton_pause_clicked()
{
    deviceHandler.disconnectService();
}


void MainWindow::on_Button_contiunue_clicked()
{
    deviceHandler.continueConnectService();
}


void MainWindow::on_pushButton_clear_clicked()
{
    if (MyDockQTE_bleinfo)
    MyDockQTE_bleinfo->clear();
    if (MyDockQTE)
    MyDockQTE->clear();
}

// 创建两个dockwidget窗口
void MainWindow::CreatNewView()
{
    dock1 = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,dock1);
    //给浮动窗口添加文本编辑区
    MyDockQTE = new QTextEdit(this);
    dock1->setWidget(MyDockQTE);
    dock1->setWindowTitle("无线蓝牙打印窗口");
    //dock1->setAttribute(Qt::WA_DeleteOnClose);
    //connect(dock1,SIGNAL(visibilityChanged(bool)),this,SLOT(dock1CloseEvent(bool)));
    //dock2->setFloating(1);
    dock1->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    dock2 = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,dock2);
    //给浮动窗口添加文本编辑区
    MyDockQTE_bleinfo = new QTextEdit(this);
    dock2->setWidget(MyDockQTE_bleinfo);
    dock2->setWindowTitle("蓝牙调试窗口");
    dock2->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    //connect(dock2,SIGNAL(visibilityChanged(bool)),this,SLOT(dock2CloseEvent(bool)));
    //dock2->setFloating(1);
}

//读取上次界面信息
void MainWindow::readSettings()
{
    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("mainWindow");
    resize(settings.value("size").toSize());
    restoreState(settings.value("state").toByteArray());
    settings.endGroup();
}

// 存储当前界面布局信息
void MainWindow::saveSettings()
{
    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("mainWindow");
    //settings.setValue("geometry", saveGeometry());
    //settings.setValue("pos", this->pos());
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.endGroup();
}


