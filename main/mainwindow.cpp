#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow *MainWindow::mutualUi = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // first : mainwindow
    ui->setupUi(this);
    mutualUi = this;

    QWidget* p = takeCentralWidget();   //删除中央窗体，使用QDockWidget代替
    if (p)
        delete p;
    setDockNestingEnabled(true);        //允许嵌套dock
    //-------------------------------------------window
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
    QAction* pBleConnectWindow = new QAction(QIcon(QPixmap(":/src/5.png")), "ble连接窗口");
    pMenuWindow->addAction(pBleConnectWindow);
    QAction* pSoftwareInfiWindow = new QAction(QIcon(QPixmap(":/src/6.png")), "软件信息窗口");
    pMenuWindow->addAction(pSoftwareInfiWindow);
    QAction* pBleDebugWindow = new QAction(QIcon(QPixmap(":/src/7.png")), "ble调试窗口");
    pMenuWindow->addAction(pBleDebugWindow);
    QAction* pBleUartDebugWindow = new QAction(QIcon(QPixmap(":/src/8.png")), "蓝牙-串口调试窗口");
    pMenuWindow->addAction(pBleUartDebugWindow);
    pMenuBar->addMenu(pMenuWindow);

    QObject::connect(pActionFileSave, SIGNAL(triggered(bool)), this, SLOT(fileSave()));
    QObject::connect(pActionWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_resetWindow()));
    QObject::connect(pActionSaveWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_saveWindow()));
    QObject::connect(pActionRestoreWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_restoreWindow()));
    QObject::connect(pBleConnectWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_bleConnectWindow()));
    QObject::connect(pSoftwareInfiWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_SoftwareInfoWindow()));
    QObject::connect(pBleDebugWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_BleDebugWindow()));
    QObject::connect(pBleUartDebugWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_BleUartDebugWindow()));

    //----------------------------------------- 在工具栏添加图标按钮
    toolbar = new QToolBar("tool bar");
    toolbar->setObjectName("tool bar");

    QToolButton *toolBtn1 = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn1->setText("manual");
    //toolBtn1->setFixedSize(30,20);                              //调图标大小（不是setIconSize)
    toolbar->addWidget(toolBtn1);                               //向工具栏添加QToolButton按钮

    QToolButton *toolBtn2 = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn2->setText("tcpsocket");
    toolbar->addWidget(toolBtn2);                               //向工具栏添加QToolButton按钮

    QToolButton *toolBtn3 = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn3->setText("blelink");
    toolbar->addWidget(toolBtn3);                               //向工具栏添加QToolButton按钮

    QToolButton *toolBtn4 = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn4->setText("info");
    toolbar->addWidget(toolBtn4);                               //向工具栏添加QToolButton按钮

    QToolButton *toolBtn5 = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn5->setText("debug");
    toolbar->addWidget(toolBtn5);                               //向工具栏添加QToolButton按钮

    QToolButton *toolBtn6 = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn6->setText("cfgwifi");
    toolbar->addWidget(toolBtn6);                               //向工具栏添加QToolButton按钮

    addToolBar(Qt::TopToolBarArea, toolbar);

    connect(toolBtn1, &QToolButton::clicked, this, &MainWindow::menu_action_BleUartDebugWindow);
    connect(toolBtn2, &QToolButton::clicked, this, &MainWindow::menu_action_TcpSocketWindow);
    connect(toolBtn3, &QToolButton::clicked, this, &MainWindow::menu_action_bleConnectWindow);
    connect(toolBtn4, &QToolButton::clicked, this, &MainWindow::menu_action_SoftwareInfoWindow);
    connect(toolBtn5, &QToolButton::clicked, this, &MainWindow::menu_action_BleDebugWindow);
    connect(toolBtn6, &QToolButton::clicked, this, &MainWindow::menu_action_configWifi);
    DockwidgetInfo = new QDockWidget(this);
    text_info = new QTextEdit(this);
    text_info->setReadOnly(true);
    DockwidgetInfo->setWidget(text_info);
    DockwidgetInfo->setObjectName("info");
    DockwidgetInfo->setWindowTitle("info");
    addDockWidget(Qt::BottomDockWidgetArea, DockwidgetInfo);
    //---------------------------dockwidget window config

    // sencond : ble api init
    connectionHandler = new ConnectionHandler();
    deviceHandler = new DeviceHandler();
    deviceFinder = new DeviceFinder(deviceHandler);

    SetInfo("tip :\n"
            "   window              usage        \n"
            "1. info : some software info print\n"
            "2. manual : send uart cmd to mcu via ble \n"
            "3. tcpsocket : use tcp socket talk with dev\n"
            "4. blelink : connect to dev via ble\n"
            "5. debug : view dev debug message via ble\n"
            );

    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetInfo(QString str)
{
    if (!text_info){
        qDebug() << "text_info nullptr";
        return;
    }
    if (str == "clear"){
        text_info->clear();
    }
    text_info->append(str);
}

QByteArray MainWindow::calGetBleData(QByteArray array)
{
    uint8_t parsedValue;
    uchar senddata[20] = {0};
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
    deviceHandler->calculate(senddata);
    array = QByteArray((char*)senddata,20);
    return array;
}

void MainWindow::ble_send(QByteArray array)
{
    array = calGetBleData(array);
    if (deviceHandler->setChar.isValid())
    {
        deviceHandler->characteristicWrite(deviceHandler->setChar,array);
    }
    else
    {
        //qDebug() << array;
        SetInfo("warning : deviceHandler.setChar is null");
    }
}

void MainWindow::ble_debug_send(QByteArray array)
{
    array = calGetBleData(array);
    if (deviceHandler->bledebugsetChar.isValid())
    {
        deviceHandler->characteristicWrite(deviceHandler->bledebugsetChar,array);
    }
    else
    {
        SetInfo("warning : deviceHandler.setChar is null");
    }
}

void MainWindow::ble_char_send(uchar *array)
{
    deviceHandler->calculate(array);
    QByteArray Array = QByteArray((char*)array,20);
    if (deviceHandler->setChar.isValid())
    {
        deviceHandler->characteristicWrite(deviceHandler->setChar,Array);
    }
    else
    {
        qDebug() << Array.toHex();
        SetInfo("warning : deviceHandler.setChar is null");
    }
}

//------------------------------------------------------

//读取上次界面信息
void MainWindow::readSettings()
{
    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("mainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    resize(settings.value("size").toSize());
    settings.endGroup();
    qDebug() << "恢复窗口";
}

// 存储当前界面布局信息
void MainWindow::saveSettings()
{
    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("mainWindow");
    settings.setValue("geometry",saveGeometry());
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.endGroup();
    qDebug() << "保存窗口";
/*
    //寻找所有的dockwidget
    QList<QDockWidget*> docks = this->findChildren<QDockWidget*>();
    for(int i = 0; i < docks.size(); i++)
    {
       qDebug() << docks.at(i)->windowTitle();
    }
*/
}

// 恢复默认的界面布局信息
void MainWindow::menu_action_resetWindow()
{
    //qDebug() << this->size();QSize(462, 563)
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenRect =  screen->availableVirtualGeometry();
    resize(screenRect.width(),screenRect.height());

    //setWindowState(Qt::WindowMaximized);
    addToolBar(Qt::TopToolBarArea, toolbar);

    //寻找所有的dockwidget
    int first = 0;
    QList<QDockWidget*> docks = this->findChildren<QDockWidget*>();
    for(int i = 0; i < docks.size(); i++)
    {
       qDebug() << docks.at(i)->windowTitle();
       if (docks.at(i)->windowTitle() == "连接蓝牙窗口"){
           first = i;
       }
       if (i == 0){
           continue;
       }
       /*多个Dockwidget默认重叠*/
       tabifyDockWidget(docks.at(i-1),docks.at(i));
    }

    /*默认指针的Dockwidget显示*/
    docks.at(first)->raise();

    SetInfo("恢复默认窗口信息成功");
    showMsg("已恢复默认布局~");
    saveSettings();
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

void MainWindow::menu_action_bleConnectWindow()
{
    if (DockWidgetblelink->isVisible()){
        DockWidgetblelink->setVisible(false);
    }else{
        DockWidgetblelink->setVisible(true);
    }
}

void MainWindow::menu_action_SoftwareInfoWindow()
{
    if (DockwidgetInfo->isVisible()){
        DockwidgetInfo->setVisible(false);
    }else{
        DockwidgetInfo->setVisible(true);
    }
}

void MainWindow::menu_action_BleDebugWindow()
{
    if (DockWigetbleDebug->isVisible()){
        DockWigetbleDebug->setVisible(false);
    }else{
        DockWigetbleDebug->setVisible(true);
    }
}

void MainWindow::menu_action_BleUartDebugWindow()
{
   if (DockWidgetBleUart->isVisible()){
        DockWidgetBleUart->setVisible(false);
   }else{
        DockWidgetBleUart->setVisible(true);
   }
}

void MainWindow::menu_action_TcpSocketWindow()
{
    if (DockWidgetsocket->isVisible()){
        DockWidgetsocket->setVisible(false);
    }else{
        DockWidgetsocket->setVisible(true);
    }
}

void MainWindow::menu_action_configWifi()
{
    if (DockwidgetWifiConfig->isVisible()){
        DockwidgetWifiConfig->setVisible(false);
    }else{
        DockwidgetWifiConfig->setVisible(true);
    }

}

void MainWindow::showMsg(QString str)
{
    QMessageBox msgBox;
    msgBox.setText(str);
    msgBox.exec();
}

void MainWindow::fileSave()
{
    QString str = text_debug->toPlainText();
    QByteArray buf = str.toUtf8();

#define savefile

#ifdef savefile

    QWidget *qwidget = new QWidget();
    QString dir = QFileDialog::getSaveFileName(qwidget,"save file","",nullptr);
    QFile file(dir);

#else
    bool ok;
    QString fileName = nullptr;
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

void MainWindow::creatNewDockWindow(QDockWidget *w, Qt::DockWidgetArea a, bool mix)
{
    // 进行布局
    addDockWidget(a,w);
    if (mix){
        //tabifyDockWidget(dock,dock1);
    }
}
