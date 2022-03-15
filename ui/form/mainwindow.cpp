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

    // ------------------------------------------在菜单栏添加选项

//    pMenuBar = ui->menuBar;
//    // 如上述，此菜单即添加了图标
//    QMenu* pMenuWindow = new QMenu("窗口");
//    QAction* pActionWindow = new QAction(QIcon(QPixmap(":/src/2.png")), "恢复窗口默认设置");
//    pMenuWindow->addAction(pActionWindow);
//    pMenuBar->addMenu(pMenuWindow);
//    QObject::connect(pActionFileSave, SIGNAL(triggered(bool)), this, SLOT(fileSave()));


    //---------------------------dockwidget window config

    DockwidgetInfo = new QDockWidget(this);
    text_info = new QTextEdit(this);
    text_info->setReadOnly(true);
    DockwidgetInfo->setWidget(text_info);
    DockwidgetInfo->setObjectName("软件输出信息");
    DockwidgetInfo->setWindowTitle("信息");
    DockwidgetInfo->setVisible(false);
    addDockWidget(Qt::BottomDockWidgetArea, DockwidgetInfo);

    //----------------------------------------- 在工具栏添加图标按钮
    toolbar = new QToolBar(this);
    toolbar->setObjectName("tool bar");
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //toolbar->setFixedWidth(80);
    //toolbar->setGeometry(0,0,0,0);
    //toolbar->setContentsMargins(0,0,0,0);

    toolBtn = new QToolButton(this);              //创建QToolButton
    toolBtn->setText(DockwidgetInfo->windowTitle());
    //toolBtn->setGeometry(0,0,0,0);
    //toolBtn->setContentsMargins(0,0,0,0);
    toolbar->addWidget(toolBtn);
    addToolBar(Qt::RightToolBarArea, toolbar);
    toolbar->connect(toolBtn, &QToolButton::clicked, this, [this]{
        closeAllWindow();
        if (DockwidgetInfo->isVisible()){
            DockwidgetInfo->setVisible(false);
        }else{
            DockwidgetInfo->setVisible(true);
        }
    });

    //----------------------------------------- other window
    blelink      = new blelinkwindow(this);
    SocketClient = new tcpSocketClient(this);
    bleuart      = new bleUartWindow(this);
    bledebug     = new bledebugwindow(this);

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
        if (tetoutput != nullptr)
        tetoutput->clear();
    }
    text_info->append(str);
    if (tetoutput != nullptr)
    tetoutput->append(str);
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
    if (deviceHandler->setChar.isValid()){
        deviceHandler->characteristicWrite(deviceHandler->setChar,array);
    }
    else{
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
void MainWindow::selectFunction(int i)
{
    switch (i){
    case 0:saveSettings();break;
    case 1:readSettings();break;
    case 2:resetWindow();break;
    case 3:fileSave();break;
    }
}


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
void MainWindow::resetWindow()
{
    addToolBar(Qt::RightToolBarArea, toolbar);

    //寻找所有的dockwidget
    int first = 0;
    QList<QDockWidget*> docks = this->findChildren<QDockWidget*>();
    for(int i = 0; i < docks.size(); i++)
    {
       //qDebug() << docks.at(i)->windowTitle();
       docks.at(i)->setVisible(false);
       docks.at(i)->setFloating(false);
       if (docks.at(i)->windowTitle() == "连接")
           docks.at(i)->setVisible(true);
       /*多个Dockwidget默认重叠*/
       //tabifyDockWidget(docks.at(i-1),docks.at(i));
    }

    QList<QToolButton*> btn = toolbar->findChildren<QToolButton*>();
    for(int i = 0; i < btn.size(); i++)
    {
       if (QString(btn.at(i)->text()).contains("连接", Qt::CaseInsensitive)){
           btn.at(i)->click();
       }
    }
    /*默认指针的Dockwidget显示*/
    //docks.at(first)->raise();

    SetInfo("恢复默认窗口信息成功");
    //showMsg("已恢复默认布局~");
    saveSettings();
}

void MainWindow::closeAllWindow()
{
    QList<QDockWidget*> docks = this->findChildren<QDockWidget*>();
    for(int i = 0; i < docks.size(); i++)
    {
       docks.at(i)->setVisible(false);
    }

    QList<QToolButton*> list = toolbar->findChildren<QToolButton*>();
    for (int i = 0; i < list.size(); i++)
    {
        QToolButton *btn = (QToolButton *)list.at(i);
        btn->setChecked(false);
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
    QString str = text_info->toPlainText();
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
