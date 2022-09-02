#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow *MainWindow::mutualUi = nullptr;

// 蓝牙数据回调
void MainWindow::ble_rx_data_func(const QByteArray &d)
{
    QString name = connectionHandler->name();

    if (name.contains("H1111", Qt::CaseInsensitive))
    {
        bleSensor->sensor_blemsg_handle(d);
    }
    else if (name.contains("H71", Qt::CaseInsensitive))
    {
        blesku->govee_sku_blemsg_handle(d);
    }
    else
    {
        blesku->govee_sku_blemsg_handle(d);
    }
}

MainWindow::MainWindow(QWidget *parent, QTextEdit *textEdit)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // first : mainwindow
    ui->setupUi(this);
    mutualUi = this;
    tetoutput = textEdit;

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
    DockwidgetInfo->setStyleSheet("border:none");
    addDockWidget(Qt::BottomDockWidgetArea, DockwidgetInfo);

    //----------------------------------------- 在工具栏添加图标按钮
    toolbar = new QToolBar(this);
    toolbar->setObjectName("toolbar");
    toolbar->setWindowTitle("toolbar");
    toolbar->setMovable(false);
    //toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //toolbar->setFixedWidth(80);
    //toolbar->setGeometry(0,0,0,0);
    //toolbar->setContentsMargins(0,0,0,0);

    toolBtn = new QToolButton(this);              //创建QToolButton
    toolBtn->setText(DockwidgetInfo->windowTitle());
    toolbar->addWidget(toolBtn);
    addToolBar(Qt::RightToolBarArea, toolbar);
    toolbar->connect(toolBtn, &QToolButton::clicked, this, [this]{
        closeAllWindow();
        if (DockwidgetInfo->isVisible()){
            DockwidgetInfo->setVisible(false);
        }else{
            DockwidgetInfo->setVisible(true);
        }
        toolBtn->setChecked(true);
    });

    //----------------------------------------- other window
    blelink      = new blelinkwindow(this);
    blesku      = new bleskumsghandle();
    SocketClient = new tcpSocketClient(this);
    bleuart      = new bleUartWindow(this);
    bleSensor     = new bleSensorWindow(this);

    //-----------------------------------------  sencond : ble api init
    connectionHandler = new ConnectionHandler();
    deviceHandler = new DeviceHandler(nullptr, tetoutput);
    deviceFinder = new DeviceFinder(nullptr, deviceHandler, tetoutput);

    connect(deviceFinder,  &DeviceFinder::scanDeviceResult,  blelink, &blelinkwindow::addBleDevToList);     // add ble dev name to list show
    connect(deviceHandler, &DeviceHandler::bleMessageChange, this,    &MainWindow::ble_rx_data_func);       // msg from ble dev
    connect(deviceHandler, &DeviceHandler::connectSuccess,   blelink, &blelinkwindow::bleConnectSuccess);   // discon statu occur
    connect(deviceHandler, &DeviceHandler::disconnectOccur,  blelink, &blelinkwindow::disconButton_clicked);// discon statu occur

    SetInfo("tip :\n"
            "   window              usage        \n"
            "1. info : some software info print\n"
            "2. manual : send uart cmd to mcu via ble \n"
            "3. tcpsocket : use tcp socket talk with dev\n"
            "4. blelink : connect to dev via ble\n"
            "5. debug : view dev debug message via ble\n"
            );

    setStatusBar(nullptr);
    setMenuBar(nullptr);

    QList<QDockWidget*> docks = this->findChildren<QDockWidget*>();
    for(int i = 0; i < docks.size(); i++)
    {
       docks.at(i)->setFeatures(QDockWidget::DockWidgetClosable |
                                QDockWidget::DockWidgetMovable  |
                                QDockWidget::DockWidgetFloatable);

       Q_FOREACH(QTabBar* tab, docks.at(i)->findChildren<QTabBar *>())
       {
           tab->setDrawBase(false);
       }
    }
    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ShowInfo(QString str)
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

void MainWindow::SetInfo(QString str)
{
    if (!tetoutput){
        qDebug() << "text_info nullptr";
        return;
    }
    if (str == "clear"){
        tetoutput->clear();
    }
    tetoutput->append(str);
}

// 计算校验和
void MainWindow::calGetBleData(uint8_t *data)
{
    uint8_t temp = 0;
    for(int i = 0; i < 19; i++)
    {
        temp = temp ^ data[i];
    }
    data[19] = temp;
}


void MainWindow::calGetBleData(QByteArray &array, uint8_t *msg)
{
    uint8_t parsedValue;
    for (int i = 0; i < array.size(); i++)
    {
        //qDebug() << array.length() << array.size() << array[i];
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
                msg[i/2] = parsedValue + (array[i] - '0');
            if (array[i] >= 'A' && array[i] <= 'F')
                msg[i/2] = parsedValue + (array[i] - ('A'-'9' - 1) - '0');
            if (array[i] >= 'a' && array[i] <= 'f')
                msg[i/2] = parsedValue + (array[i] - ('a'-'9' - 1) - '0');
            parsedValue = 0;
        }
        //qDebug() << msg[i/2];
    }
    calGetBleData(msg);
    array = QByteArray((char*)msg,20);
}

void MainWindow::ble_send(QByteArray array)
{
    if (deviceHandler->setChar.isValid()){
        uint8_t data[20] = {0};
        calGetBleData(array, data);
        SetInfo("send :" + array.toHex(' ')); // hex display split by ' '
        deviceHandler->characteristicWrite(deviceHandler->setChar,array);
    }
    else{
        SetInfo("warning : deviceHandler.setChar is null");
    }
}

void MainWindow::ble_char_send(uchar *array)
{
    calGetBleData(array);
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

void MainWindow::ble_pt_send(QByteArray array)
{
    if (deviceHandler->setChar.isValid())
    {
        SetInfo(array);
        deviceHandler->characteristicWrite(deviceHandler->setChar,array);
    }
    else
    {
        SetInfo("warning ble_pt_send setChar is null");
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

    QList<QToolButton*> btn = toolbar->findChildren<QToolButton*>();
    for(int i = 0; i < btn.size(); i++)
    {
       if (btn.at(i)->isChecked()){
           btn.at(i)->click();
       }
    }
    SetInfo("恢复窗口");
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
    SetInfo("保存窗口");

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
