#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "devicefinder.h"
#include "devicehandler.h"
#include "deviceinfo.h"
#include "connectionhandler.h"

MainWindow *MainWindow::mutualUi = nullptr;

ConnectionHandler connectionHandler;
DeviceHandler deviceHandler;
DeviceFinder deviceFinder(&deviceHandler);

QDockWidget *dock = nullptr;
QDockWidget *dock1 = nullptr;
QDockWidget *dock2 = nullptr;
QDockWidget *dockBleUart = nullptr;
QTextEdit *MyDockQTE = nullptr;
QTextEdit *MyDockQTE_bleinfo = nullptr;

// ble uart
QVBoxLayout *verticalLayout = nullptr;
QWidget *WidgetContents = nullptr;
QHBoxLayout *horizontalLayout = nullptr;
QHBoxLayout *horizontalLayout1 = nullptr;
QGridLayout *gridlayout = nullptr;
myQPushButton *firstbutton = nullptr;


static int line = 1;
static int row  = 0;
int maxrow = 4;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mutualUi = this;

    QWidget* p = takeCentralWidget();   //删除中央窗体，使用QDockWidget代替
    if (p)
        delete p;

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
    QObject::connect(pSoftwareInfiWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_SoftwareInfiWindow()));
    QObject::connect(pBleDebugWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_BleDebugWindow()));
    QObject::connect(pBleUartDebugWindow, SIGNAL(triggered(bool)), this, SLOT(menu_action_BleUartDebugWindow()));

    //----------------------------------------- 在工具栏添加图标按钮
    toolbar = new QToolBar("tool bar");
    toolBtn1 = new QToolButton(this);              //创建QToolButton
    toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn1->setFixedSize(30,20);                              //调图标大小（不是setIconSize)
    toolbar->addWidget(toolBtn1);                               //向工具栏添加QToolButton按钮
    toolbar->setObjectName("tool bar");
    addToolBar(Qt::TopToolBarArea, toolbar);

    connect(toolBtn1, SIGNAL(clicked(bool)), this, SLOT(toolBarBleUartButtonClick()));

    //----------------------------------------- 在界面添加窗口
    dock = new QDockWidget(this);
    setDockNestingEnabled(true);        //允许嵌套dock
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setWindowTitle("连接蓝牙窗口");
    dock->setObjectName("连接蓝牙窗口");

    // 进行布局
    addDockWidget(Qt::TopDockWidgetArea,dock);

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
    toolBarBleUartButtonClick(); //load widget
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
    deviceFinder.connectToService("ihoment_H7160_07F7");
}

QByteArray MainWindow::calGetBleData(QByteArray array)
{
    uint8_t parsedValue;
    uint8_t senddata[20] = {0};
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
    return array;
}

void MainWindow::sendButton_clicked()
{
    QString data = text_ble_send->toPlainText();
    //qDebug() << data.toUtf8(); //"aa11"
    QByteArray array = data.toUtf8(); //"aa11"
    array = calGetBleData(array);
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
    dock1->setObjectName("无线蓝牙打印窗口");
    dock1->setWindowTitle("无线蓝牙打印窗口");
    //dock1->setAttribute(Qt::WA_DeleteOnClose);
    //connect(dock1,SIGNAL(visibilityChanged(bool)),this,SLOT(dock1CloseEvent(bool)));
    //dock2->setFloating(1);
    dock1->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock1->setVisible(false);

    dock2 = new QDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,dock2);
    //给浮动窗口添加文本编辑区
    MyDockQTE_bleinfo = new QTextEdit(this);
    MyDockQTE_bleinfo->setReadOnly(true);
    dock2->setWidget(MyDockQTE_bleinfo);
    dock2->setObjectName("蓝牙调试窗口");
    dock2->setWindowTitle("蓝牙调试窗口");
    dock2->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock2->setVisible(false);
    //connect(dock2,SIGNAL(visibilityChanged(bool)),this,SLOT(dock2CloseEvent(bool)));
    //dock2->setFloating(1);
}

//读取上次界面信息
void MainWindow::readSettings()
{
    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("mainWindow");
    resize(settings.value("size").toSize());
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
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
    resize(QSize(462, 563));
    addToolBar(Qt::TopToolBarArea, toolbar);
    if (dock1 == nullptr || dock2 == nullptr || dock == nullptr)
    {
        MainWindow();
    }
    if (dock != nullptr)
    {
        dock->setFloating(0);
        addDockWidget(Qt::LeftDockWidgetArea,dock);
        dock->setVisible(true);
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
    if (dockBleUart != nullptr)
    {
        dockBleUart->setFloating(0);
        addDockWidget(Qt::BottomDockWidgetArea,dockBleUart);
        dockBleUart->setVisible(true);
    }
    /*多个Dockwidget默认重叠*/
    tabifyDockWidget(dock,dock1);
    tabifyDockWidget(dock1,dock2);
    /*默认指针的Dockwidget显示*/
    dock->raise();
    SetTextEdit(1, "恢复默认窗口信息成功");
    qDebug() << "恢复默认窗口信息成功";
    showMsg("已恢复默认布局~");
    saveSettings();
}

//
void MainWindow::toolBarBleUartButtonClick()
{
    if (dockBleUart != nullptr){
       qDebug() << "dockBleUart really have";
       if (dockBleUart->isVisible())
       {
           qDebug() << "dockBleUart is valid";
           return;
       }
       else
       {
           qDebug() << "dockBleUart is no valid, creating now";
       }
    }
    qDebug() << "i am in";
    dockBleUart = new QDockWidget(this);
    dockBleUart->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockBleUart->setWindowTitle("蓝牙-串口透传窗口");
    dockBleUart->setObjectName("蓝牙-串口透传窗口");
    //dockBleUart->setFloating(true);

    WidgetContents = new QWidget(dockBleUart);
    gridlayout= new QGridLayout(WidgetContents);

    QPushButton *button_add_bleUart;
    button_add_bleUart = new QPushButton(WidgetContents);
    button_add_bleUart->setMaximumHeight(20); //limit size
    button_add_bleUart->setMaximumWidth(40);
    button_add_bleUart->setText("add");

    QPushButton *button_load_bleUart;
    button_load_bleUart = new QPushButton(WidgetContents);
    button_load_bleUart->setMaximumHeight(20); //limit size
    button_load_bleUart->setMaximumWidth(40);
    button_load_bleUart->setText("load");


    QPushButton *button_save_bleUart;
    button_save_bleUart = new QPushButton(WidgetContents);
    button_save_bleUart->setMaximumHeight(20); //limit size
    button_save_bleUart->setMaximumWidth(40);
    button_save_bleUart->setText("save");

    gridlayout->addWidget(button_add_bleUart,0,0);
    gridlayout->addWidget(button_load_bleUart,0,1);
    gridlayout->addWidget(button_save_bleUart,0,2);

    connect(button_add_bleUart, SIGNAL(clicked(bool)), this ,SLOT(toolbarButtonAdd_clicked()));
    connect(button_load_bleUart, SIGNAL(clicked(bool)), this ,SLOT(bleCmdLoadFile()));
    connect(button_save_bleUart, SIGNAL(clicked(bool)), this ,SLOT(bleCmdSaveFile()));

    addDockWidget(Qt::BottomDockWidgetArea,dockBleUart);
    dockBleUart->setWidget(WidgetContents);
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
    if (dock->isVisible()){
        dock->setVisible(false);
    }else{
        dock->setVisible(true);
    }
}

void MainWindow::menu_action_SoftwareInfiWindow()
{
    if (dock2->isVisible()){
        dock2->setVisible(false);
    }else{
        dock2->setVisible(true);
    }
}

void MainWindow::menu_action_BleDebugWindow()
{
    if (dock1->isVisible()){
        dock1->setVisible(false);
    }else{
        dock1->setVisible(true);
    }
}

void MainWindow::menu_action_BleUartDebugWindow()
{
   if (dockBleUart->isVisible()){
        dockBleUart->setVisible(false);
   }else{
        dockBleUart->setVisible(true);
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
    QString str = MyDockQTE->toPlainText();
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

//  dynamic add button
void MainWindow::toolbarButtonAdd_clicked()
{
    myQPushButton *temp = nullptr, *ptemp = nullptr;
    bool ok;
    QString cmd = "";
    QString buttonName = "ButtonN";

    QString text = QInputDialog::getText(this, tr("我只是打酱油的~"),tr("input button name"), QLineEdit::Normal,0, &ok);
    if (ok && !text.isEmpty())
    {
        buttonName = text; //获取输入
    }
    else
    {
        return;
    }

    text = QInputDialog::getText(this, tr("我只是打酱油的~"),tr("input cmd + data (BB0101)"), QLineEdit::Normal,0, &ok);
    if (ok && !text.isEmpty())
    {
        cmd = text; //获取输入
    }
    else
    {
        return;
    }

    if (firstbutton == nullptr){
        qDebug() << "firstbutton is null";
        firstbutton = new myQPushButton(WidgetContents);
        firstbutton->setMaximumHeight(20); //limit size
        firstbutton->setMaximumWidth(100);
        firstbutton->setText(buttonName);
        firstbutton->nextButton = nullptr;
        firstbutton->prevButton = nullptr;
        firstbutton->cmd = cmd;
        temp = firstbutton;
    }
    else
    {
        qDebug() << "firstbutton is not null";
        temp = firstbutton;
        ptemp = nullptr; // last button
        while (temp->nextButton != nullptr) {
            //qDebug() << temp->text();
            //qDebug() << temp->cmd;
            temp = temp->nextButton;
        }

        temp->nextButton = new myQPushButton(WidgetContents);
        if (temp->nextButton == nullptr){
            qDebug() << "new is FAIL";
            return;
        }
        temp = temp->nextButton;
        temp->setMaximumHeight(20); //limit size
        temp->setMaximumWidth(100);
        temp->setText(buttonName);
        temp->nextButton = nullptr;
        temp->prevButton = ptemp;
        temp->cmd = cmd;
    }

    if (temp == nullptr){
        qDebug() << "temp is null";
        return;
    }

    connect(temp,SIGNAL(myclick(myQPushButton*)),this,SLOT(bleCmdSendData(myQPushButton*)));

    if (row <= maxrow)
    {
        gridlayout->addWidget(temp,line,row);
        row++;
    }
    else
    {
        row = 0;
        line++;
        gridlayout->addWidget(temp,line,row);
    }

    temp = firstbutton;
    while (temp != nullptr) {
        qDebug() << temp->text();
        qDebug() << temp->cmd;
        temp = temp->nextButton;
    }
}

void MainWindow::bleCmdSendData(myQPushButton* temp)
{
    qDebug() << temp->cmd.toUtf8(); //"aa11"
    QByteArray array = temp->cmd.toUtf8(); //"aa11"
    array = calGetBleData(array);
    if (deviceHandler.setChar.isValid())
    {
        deviceHandler.characteristicWrite(deviceHandler.setChar,array);
    }
    else
    {
        SetTextEdit(1, "deviceHandler.setChar is null");
    }
}


void MainWindow::bleCmdSaveFile()
{
    if (firstbutton == nullptr)
    {
        return;
    }
    myQPushButton *temp = firstbutton;
    QByteArray buf;
    while (temp != nullptr) {
        buf += temp->text().toUtf8();
        buf += "+";
        buf += temp->cmd.toUtf8();
        buf += "+";
        temp = temp->nextButton;
    }

    QWidget *qwidget = new QWidget();
    QString dir = QFileDialog::getSaveFileName(qwidget,"save file","",nullptr);
    QFile file(dir);


    file.open(QFile::WriteOnly);
    file.write(buf);
    file.close();
}

void MainWindow::bleCmdLoadFile()
{
    line = 1;
    row  = 0;

    QWidget *qwidget = new QWidget();
    QString dir = QFileDialog::getOpenFileName(qwidget,"load file","",nullptr);

    qDebug() << dir;
    if (dir == nullptr){
        return;
    }

    QFile file(dir);

    QByteArray buf;
    file.open(QFile::ReadOnly);
    buf = file.readAll();
    file.close();

    qDebug() << buf;
    myQPushButton *temp = firstbutton;
    if (firstbutton != nullptr)
    {
        while (temp->nextButton != nullptr){
            temp = temp->nextButton;
        }

        temp = temp->prevButton;

        while (temp->nextButton != nullptr && temp->prevButton != nullptr){
            delete  temp->nextButton;
            temp = temp->prevButton;
        }
        delete temp;
    }

    dockBleUart->close();

    QList<QByteArray> list = buf.split('+');
    for (int i = 0; i < list.count() - 2; i++)
    {
        qDebug() << list.at(i) << i << list.count();
        if (i == 0){
            myQPushButton *newbutton = nullptr;
            newbutton = new myQPushButton(WidgetContents);
            newbutton->setMaximumHeight(20); //limit size
            newbutton->setMaximumWidth(100);
            newbutton->setText(list.at(i));
            newbutton->cmd = list.at(i + 1);
            firstbutton = newbutton;
            firstbutton->nextButton = nullptr;
            firstbutton->prevButton = nullptr;
            temp = firstbutton;
        }
        else if (i % 2 == 0)
        {
            myQPushButton *newbutton = nullptr;
            newbutton = new myQPushButton(WidgetContents);
            newbutton->setMaximumHeight(20); //limit size
            newbutton->setMaximumWidth(100);
            newbutton->setText(list.at(i));
            newbutton->cmd = list.at(i + 1);
            newbutton->prevButton = temp;
            newbutton->nextButton = nullptr;

            temp->nextButton = newbutton;
            temp = newbutton;
        }
    }

    toolBarBleUartButtonClick(); //reload widget

    temp = firstbutton;
    while (temp != nullptr)
    {
        qDebug() << temp->text();
        connect(temp,SIGNAL(myclick(myQPushButton*)),this,SLOT(bleCmdSendData(myQPushButton*)));
        if (row < maxrow)
        {
            gridlayout->addWidget(temp,line,row);
            row++;
        }
        else
        {
            row = 0;
            line++;
            gridlayout->addWidget(temp,line,row);
            row = 1;
        }

        temp = temp->nextButton;
    }

}
