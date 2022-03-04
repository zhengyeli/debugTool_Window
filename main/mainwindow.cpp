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
    QAction* pcloseWindow = new QAction(QIcon(QPixmap(":/src/5.png")), "关闭所有窗口");
    pMenuWindow->addAction(pcloseWindow);
    pMenuBar->addMenu(pMenuWindow);

    QObject::connect(pActionFileSave, SIGNAL(triggered(bool)), this, SLOT(fileSave()));
    QObject::connect(pActionWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_resetWindow()));
    QObject::connect(pActionSaveWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_saveWindow()));
    QObject::connect(pActionRestoreWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_restoreWindow()));
    QObject::connect(pcloseWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_closeAllWindow()));

    //---------------------------dockwidget window config

    DockwidgetInfo = new QDockWidget(this);
    text_info = new QTextEdit(this);
    text_info->setReadOnly(true);
    DockwidgetInfo->setWidget(text_info);
    DockwidgetInfo->setObjectName("软件输出信息");
    DockwidgetInfo->setWindowTitle("信息");
    addDockWidget(Qt::BottomDockWidgetArea, DockwidgetInfo);

    //----------------------------------------- 在工具栏添加图标按钮
    toolbar = new QToolBar(this);
    toolbar->setObjectName("tool bar");

    QToolButton *toolBtn = new QToolButton(this);              //创建QToolButton
    toolBtn->setText(DockwidgetInfo->windowTitle());
    toolbar->addWidget(toolBtn);

    toolbar->connect(toolBtn, &QToolButton::clicked, this, [this]{
        closeAllWindow();
        if (DockwidgetInfo->isVisible()){
            DockwidgetInfo->setVisible(false);
        }else{
            DockwidgetInfo->setVisible(true);
        }
    });


    //-----------------------------------------  sencond : ble api init
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
    //QScreen *screen = QGuiApplication::primaryScreen();
    //QRect screenRect =  screen->availableVirtualGeometry();
    //resize(screenRect.width(),screenRect.height());

    //setWindowState(Qt::WindowMaximized);
    addToolBar(Qt::TopToolBarArea, toolbar);

    //寻找所有的dockwidget
    int first = 0;
    QList<QDockWidget*> docks = this->findChildren<QDockWidget*>();
    for(int i = 0; i < docks.size(); i++)
    {
       qDebug() << docks.at(i)->windowTitle();
       docks.at(i)->setVisible(true);
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

void MainWindow::menu_action_closeAllWindow()
{
    closeAllWindow();
}

void MainWindow::closeAllWindow()
{
    QList<QDockWidget*> docks = this->findChildren<QDockWidget*>();
    for(int i = 0; i < docks.size(); i++)
    {
       docks.at(i)->setVisible(false);
    }
}


//------------------------------------------------------


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
