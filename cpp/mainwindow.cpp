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

MainWindow *MainWindow::mutualUi = nullptr;

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
    mutualUi = this;

    QWidget* p = takeCentralWidget();   //删除中央窗体，使用QDockWidget代替
    if (p)
        delete p;

    readSettings();
    // ------------------------------------------在菜单栏添加选项

    pMenuBar = ui->menuBar;
    QMenu* pMenuFile = new QMenu("文件");
    // 新建一个Action，然后加入到菜单A中
    // QAction* pActionA = new QAction("保存调试信息");
    QAction* pActionFileSave = new QAction(QIcon(QPixmap(":/src/1.png")), "调试信息数据 另存为");
    pMenuFile->addAction(pActionFileSave);
    pMenuBar->addMenu(pMenuFile);

    // 如上述，此菜单即添加了图标
    QMenu* pMenuWindow = new QMenu("窗口");
    //pMenuWindow->setIcon(QIcon(QPixmap(":/src/2.png")));
    QAction* pActionWindow = new QAction(QIcon(QPixmap(":/src/2.png")), "恢复窗口默认设置");
    pActionWindow->setStatusTip(tr("窗口信息"));
    pMenuWindow->addAction(pActionWindow);
    QAction* pActionSaveWindow = new QAction(QIcon(QPixmap(":/src/3.png")), "保存窗口设置");
    pMenuWindow->addAction(pActionSaveWindow);
    QAction* pActionRestoreWindow = new QAction(QIcon(QPixmap(":/src/4.png")), "恢复窗口设置");
    pMenuWindow->addAction(pActionRestoreWindow);
    pMenuBar->addMenu(pMenuWindow);

    QObject::connect(pActionFileSave, SIGNAL(triggered(bool)), this, SLOT(fileSave()));
    QObject::connect(pActionWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_resetWindow()));
    QObject::connect(pActionSaveWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_saveWindow()));
    QObject::connect(pActionRestoreWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_restoreWindow()));

    //----------------------------------------- 在工具栏添加图标按钮
    toolbar = new QToolBar("工具栏");
    QToolButton *toolBtn1 = new QToolButton(this);              //创建QToolButton
    toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn1->setFixedSize(30,20);                              //调图标大小（不是setIconSize)
    toolbar->addWidget(toolBtn1);                               //向工具栏添加QToolButton按钮
    addToolBar(Qt::TopToolBarArea, toolbar);

    //----------------------------------------- 在界面添加窗口
    dock = new QDockWidget(this);
    setDockNestingEnabled(true);        //允许嵌套dock
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setWindowTitle("连接蓝牙窗口");

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
    connect(button_scan_sku,     SIGNAL(clicked(bool)),                 this,SLOT(scanButton_clicked()));
    connect(sku_list,            SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(bleDevlist_itemClicked(QListWidgetItem*)));
    connect(button_ble_send,     SIGNAL(clicked(bool)),                 this,SLOT(sendButton_clicked()));
    connect(button_blelog_send,  SIGNAL(clicked(bool)),                 this,SLOT(sendcmdButton_clicked()));
    connect(button_clear,        SIGNAL(clicked(bool)),                 this,SLOT(clearButton_clicked()));
    connect(button_stop,         SIGNAL(clicked(bool)),                 this,SLOT(pauseButton_clicked()));
    connect(button_continue,     SIGNAL(clicked(bool)),                 this,SLOT(continueButton_clicked()));
    connect(button_discon,       SIGNAL(clicked(bool)),                 this,SLOT(disconButton_clicked()));

    dock->setWidget(dockWidgetContents);
    CreatNewView();

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetTextEdit(int index, QString str)
{
    if (!MyDockQTE_bleinfo){
        return;
    }

    MyDockQTE_bleinfo->append(str);
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

void MainWindow::scanButton_clicked()
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
void MainWindow::bleDevlist_itemClicked(QListWidgetItem* item)
{
    qDebug() << item->data(0);
    qDebug() << item->data(0).toString();
    deviceFinder.connectToService(item->data(0).toString());
    //deviceFinder.connectToService("ihoment_H7160_07F7");
}


void MainWindow::sendButton_clicked()
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


void MainWindow::disconButton_clicked()
{
    deviceHandler.disconnectDevice();
}

void MainWindow::sendcmdButton_clicked()
{
    QByteArray array;
    QString data = text_blelog_send->toPlainText();
    array = data.toUtf8(); //"aa11"
    qDebug() << array;
    deviceHandler.characteristicWrite(deviceHandler.bledebugsetChar,array);
}

void MainWindow::pauseButton_clicked()
{
    deviceHandler.disconnectService();
}


void MainWindow::continueButton_clicked()
{
    deviceHandler.continueConnectService();
}


void MainWindow::clearButton_clicked()
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
    MyDockQTE->setReadOnly(true);
    dock1->setWidget(MyDockQTE);
    dock1->setWindowTitle("无线蓝牙打印窗口");
    //dock1->setAttribute(Qt::WA_DeleteOnClose);
    //connect(dock1,SIGNAL(visibilityChanged(bool)),this,SLOT(dock1CloseEvent(bool)));
    //dock2->setFloating(1);
    dock1->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    dock2 = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,dock2);
    //给浮动窗口添加文本编辑区
    MyDockQTE_bleinfo = new QTextEdit(this);
    MyDockQTE_bleinfo->setReadOnly(true);
    dock2->setWidget(MyDockQTE_bleinfo);
    dock2->setWindowTitle("蓝牙调试窗口");
    dock2->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
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
    SetTextEdit(1, "保存窗口信息成功");
    qDebug() << "保存窗口信息成功";
}

// 恢复默认的界面布局信息
void MainWindow::menu_action_resetWindow()
{
    addToolBar(Qt::TopToolBarArea, toolbar);
    if (dock != nullptr)
    {
        dock->setFloating(0);
        addDockWidget(Qt::LeftDockWidgetArea,dock);
        dock->setVisible(true);
    }
    if (dock1 == nullptr && dock2 == nullptr && dock == nullptr)
    {
        MainWindow();
    }
    if (dock1 != nullptr)
    {
        dock1->setFloating(0);
        addDockWidget(Qt::RightDockWidgetArea,dock1);
        dock1->setVisible(true);
    }
    if (dock2 != nullptr)
    {
        dock2->setFloating(0);
        addDockWidget(Qt::RightDockWidgetArea,dock2);
        dock2->setVisible(true);
    }
    SetTextEdit(1, "恢复默认窗口信息成功");
    qDebug() << "恢复默认窗口信息成功";
    showMsg("已恢复默认布局~");
}

// 恢复界面信息
void MainWindow::menu_action_restoreWindow()
{
    readSettings();
}

void MainWindow::menu_action_saveWindow()
{
    saveSettings();
}

void MainWindow::showMsg(const QString str)
{
    QMessageBox msgBox;
    msgBox.setText(str);
    msgBox.exec();
}

void MainWindow::fileSave()
{
    QString str = text_blelog_send->toPlainText();
    QByteArray buf = str.toUtf8();
    QString fileName = nullptr;

#define savefile

#ifdef savefile

    QWidget *qwidget = new QWidget();
    QString dir = QFileDialog::getSaveFileName(qwidget,"save file","",nullptr);
    QFile file(dir);

#else
    bool ok;
    // 打开路径
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     "/",
                                                  QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    QString text = QInputDialog::getText(this, tr("我只是打酱油的~"),tr("请输入文件名字~"), QLineEdit::Password,0, &ok);
    if (ok && !text.isEmpty())
    {
        fileName = text; //获取输入保存的文件名
    }
    else
    {
        fileName = tr("logMsgSave.txt"); //默认保存的文件名
    }
    QFile file(dir+"/"+fileName);
#endif

    file.open(QFile::WriteOnly);
    file.write(buf);
    file.close();
}
