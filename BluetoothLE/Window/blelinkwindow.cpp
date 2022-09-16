#include "blelinkwindow.h"

QListWidget *blelinkwindow::sku_list = nullptr;
QLineEdit *blelinkwindow::cmd_receive = nullptr;
QTimer *timer = nullptr;
QTimer *first_cont_timer = nullptr;
blelinkwindow::blelinkwindow(QWidget *parent)
    : QWidget{parent}
{
    init();
}

blelinkwindow::~blelinkwindow()
{
    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("blelinkwindow");
    settings.setValue("sku", text_sku->text());
    settings.endGroup();
}

void blelinkwindow::init()
{

    QSettings settings("Software Inc.","Icon Editor");
    settings.beginGroup("blelinkwindow");
    QString sku = settings.value("sku").toByteArray();
    settings.endGroup();
    //----------------------------------------- 在界面添加窗口
    dockblelink = new QDockWidget(this);
    dockblelink->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockblelink->setWindowTitle("连接");
    dockblelink->setObjectName("连接蓝牙窗口");

    QWidget *dockWidgetContents;
    dockWidgetContents = new QWidget(dockblelink);
    //dockWidgetContents->setGeometry(QRect(10, 10, 100, 400)); // 从屏幕上（10，10）位置开始（即为最左上角的点），显示一个30*35的界面（宽30，高35）

    /* 所有控件 */
    text_sku = new QLineEdit(dockWidgetContents);
    if (sku.isEmpty())
        text_sku->setText("7160");
    else
        text_sku->setText(sku);
        //text_sku->setUpdatesEnabled(1);
        //text_sku->setMinimumSize(200, 50);
    text_ble_send = new QLineEdit(dockWidgetContents);
        text_ble_send->setText("aa01");
        //text_ble_send->setMaximumSize(200, 25);
    button_scan_sku = new QPushButton(dockWidgetContents);
        button_scan_sku->setText("scan");
    button_ble_send = new QPushButton(dockWidgetContents);
        button_ble_send->setText("发送");
    button_stop = new QPushButton(dockWidgetContents);
        button_stop->setText("stop");
    button_continue = new QPushButton(dockWidgetContents);
        button_continue->setText("继续");
    sku_list= new QListWidget(dockWidgetContents);
    sku_list->setMaximumHeight(250);
    button_discon = new QPushButton(dockWidgetContents);
        button_discon->setText("断开");
    cmd_send = new QLineEdit(dockWidgetContents);
        //cmd_send->setMinimumHeight(30);
        //cmd_send->setMaximumHeight(30);
        cmd_send->setReadOnly(true);
    cmd_receive = new QLineEdit(dockWidgetContents);
        //cmd_receive->setMinimumHeight(30);
        //cmd_receive->setMaximumHeight(30);
        cmd_receive->setReadOnly(true);
    button_clear = new QPushButton(dockWidgetContents);
        button_clear->setText("clear");
    //new QPushButton();     在空板上增加按键
    //new QPushButton(this); 在当前板上增加按键
    //new QPushButton(dock); 在dock板上增加按键

    /* 左布局 */
    QVBoxLayout *left_verticalLayout;
    left_verticalLayout = new QVBoxLayout();
    //left_verticalLayout->setSpacing(20);  //间距
    left_verticalLayout->setContentsMargins(5, 5, 5, 5);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    //left_verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    left_verticalLayout->addWidget(text_sku);
    left_verticalLayout->addWidget(text_ble_send);

    /* 右布局 */
    QVBoxLayout *right_verticalLayout;
    right_verticalLayout = new QVBoxLayout();
    //right_verticalLayout->setSpacing(20);  //间距
    right_verticalLayout->setContentsMargins(5, 5, 5, 5);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    right_verticalLayout->addWidget(button_scan_sku);
    right_verticalLayout->addWidget(button_ble_send);

    /* 将两个垂直布局放入水平布局 */
    QHBoxLayout *horizontalLayout;
    horizontalLayout = new QHBoxLayout();
    //horizontalLayout->setSpacing(10);  //间距
    horizontalLayout->setContentsMargins(5, 5, 5, 5);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    horizontalLayout->addItem(left_verticalLayout);
    horizontalLayout->addItem(right_verticalLayout);

    /* 将剩余按钮放入水平布局 */
    QHBoxLayout *horizontalLayout1;
    horizontalLayout1 = new QHBoxLayout();
    //horizontalLayout1->setSpacing(10);  //间距
    horizontalLayout1->setContentsMargins(5, 5, 5, 5);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    horizontalLayout1->addWidget(button_discon);
    horizontalLayout1->addWidget(button_stop);
    horizontalLayout1->addWidget(button_continue);
    horizontalLayout1->addWidget(button_clear);

    /* 将两个水平布局放入新的垂直布局 */
    QVBoxLayout *updown_verticalLayout;
    updown_verticalLayout = new QVBoxLayout(dockWidgetContents);
    updown_verticalLayout->addItem(horizontalLayout);
    updown_verticalLayout->addItem(horizontalLayout1);
    updown_verticalLayout->addWidget(cmd_send);
    updown_verticalLayout->addWidget(cmd_receive);
    updown_verticalLayout->addWidget(sku_list);

    //绑定控件的回调函数;
    connect(button_scan_sku,     SIGNAL(clicked(bool)),                 this,SLOT(scanButton_clicked()));
    connect(sku_list,            SIGNAL(itemClicked(QListWidgetItem*)), this,SLOT(bleDevlist_itemClicked(QListWidgetItem*)));
    connect(button_ble_send,     SIGNAL(clicked(bool)),                 this,SLOT(sendButton_clicked()));
    connect(button_stop,         SIGNAL(clicked(bool)),                 this,SLOT(pauseButton_clicked()));
    connect(button_continue,     SIGNAL(clicked(bool)),                 this,SLOT(continueButton_clicked()));
    connect(button_discon,       SIGNAL(clicked(bool)),                 this,SLOT(disconButton_clicked()));
    connect(button_clear,        SIGNAL(clicked(bool)),                 this,SLOT(clearButton_clicked()));
    dockblelink->setWidget(dockWidgetContents);
    // 进行布局
    MainWindow::mutualUi->creatNewDockWindow(dockblelink, Qt::TopDockWidgetArea, false);

    toolBtn = new QToolButton(this);              //创建QToolButton
    toolBtn->setText(dockblelink->windowTitle());
    MainWindow::mutualUi->toolbar->connect(toolBtn, &QToolButton::clicked, this, &blelinkwindow::closeWindow);
    MainWindow::mutualUi->toolbar->addWidget(toolBtn);                               //向工具栏添加QToolButton按钮

    dockblelink->setVisible(true);
}


void blelinkwindow::closeWindow()
{
    MainWindow::mutualUi->closeAllWindow();
    if (dockblelink->isVisible()){
        dockblelink->setVisible(false);
    }else{
        dockblelink->setVisible(true);
    }
    toolBtn->setChecked(true);
}

void blelinkwindow::scanButton_clicked()
{
    MainWindow::mutualUi->deviceFinder->sku = text_sku->text();

    if (button_scan_sku->text() == "scan")
    {
        sku_list->clear(); //清除sku list
        MainWindow::mutualUi->deviceFinder->startSearch();
        button_scan_sku->setText("stop");
    }
    else if (button_scan_sku->text() == "stop")
    {
        MainWindow::mutualUi->deviceFinder->stopSearch();
        button_scan_sku->setText("scan");
    }
}

void blelinkwindow::sendButton_clicked()
{
    QString data = text_ble_send->text();

    if (data.length() % 2 != 0){
        cmd_send->setText("input err data!!!");
        return;
    }
//    QByteArray实际存的是一个个字符，例如
//    QByteArray array1 = data.toUtf8();                           //"3132"
//    qDebug() << array1[0] << array1[1];                          // output : 3 1
//    QByteArray array = QByteArray::fromHex(data.toLatin1());     //"\x31\x32"
//    qDebug() << array[0] << array[1];                            // output : 1 2

    QByteArray array = data.toUtf8();
    cmd_send->setText(data.toUtf8());
    if (array.at(0) == '6' && array.at(1) == '6')
    {
        MainWindow::mutualUi->blesku->govee_ble_test_stop_check(array);
        // close heartbeat timer
        if (timer)
        timer->stop();

        return;
    }
    MainWindow::mutualUi->ble_send(array);
}

void blelinkwindow::disconButton_clicked()
{
    if (timer){
        timer->stop();
    }
    MainWindow::mutualUi->blesku->ble_test_stop();
    MainWindow::mutualUi->deviceHandler->disconnectDevice();
    MainWindow::mutualUi->SetInfo("disconnected");
}

void blelinkwindow::pauseButton_clicked()
{
    MainWindow::mutualUi->deviceHandler->disconnectService();
}


void blelinkwindow::continueButton_clicked()
{
    MainWindow::mutualUi->deviceHandler->continueConnectService();
}

void blelinkwindow::bleDevlist_itemClicked(QListWidgetItem* item)
{
    MainWindow::mutualUi->deviceFinder->connectToService(item->data(0).toString());
}

void blelinkwindow::ble_get_base_info()
{
    static uint8_t index = 0;
    switch (index)
    {
    case 0:
    {
        //! wifi software version
        QByteArray array("aa06");
        MainWindow::mutualUi->ble_send(array);
    }
    break;
    case 1:
    {
        //! wifi hardware version
        QByteArray array("aa0703");
        MainWindow::mutualUi->ble_send(array);
    }
    break;
    case 2:
    {
        //! mcu hardware version
        QByteArray array("aa070a");
        MainWindow::mutualUi->ble_send(array);
    }
    break;
    case 3:
    {
        //! mcu software version
        QByteArray array("aa070b");
        MainWindow::mutualUi->ble_send(array);
    }
    break;
#if 1
    case 4:
    {
        QByteArray data;
        data.append(0x33);
        data.append(0xCC);
        data.append("getRange");
        MainWindow::mutualUi->ble_pt_send(data);
    }
    break;
    case 5:
    {
        QByteArray data;
        data.append(0x33);
        data.append(0xCC);
        data.append("getSensitivity");
        MainWindow::mutualUi->ble_pt_send(data);
    }
    break;
    case 6:
    {
        QByteArray data;
        data.append(0x33);
        data.append(0xCC);
        data.append("getLatency");
        MainWindow::mutualUi->ble_pt_send(data);
    }
    break;
    case 7:
    {
        QByteArray data;
        data.append(0x33);
        data.append(0xCC);
        data.append("getInhibit");
        MainWindow::mutualUi->ble_pt_send(data);
    }
    break;
    case 8:
    {
        QByteArray data;
        data.append(0x33);
        data.append(0xCC);
        data.append("getUartOutput");
        // MainWindow::mutualUi->ble_pt_send(data);
    }
    break;
#endif
    default:
    {
        index = 0;
        first_cont_timer->stop();
        delete first_cont_timer;
        return;
    }
    }
    index++;
}

void blelinkwindow::bleConnectSuccess()
{
    // 心跳定时器
    if (timer != nullptr){
        timer->stop();
        free(timer);
        timer = nullptr;
    }
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(keepalive()));
    timer->start(4000);

    first_cont_timer = new QTimer(this);
    connect(first_cont_timer, SIGNAL(timeout()), this, SLOT(ble_get_base_info()));
    first_cont_timer->start(100);

    MainWindow::mutualUi->showMsg("connect sucess");
}

void blelinkwindow::addBleDevToList(const QBluetoothDeviceInfo& device)
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        if (text_sku->text().length() == 0)
        {
            sku_list->addItem(device.name());
        }
        else if (QString(device.name()).contains(text_sku->text(), Qt::CaseInsensitive)) // 匹配不区分大小写
        {
            sku_list->addItem(device.name());
        }
    }
}

void blelinkwindow::receive(QString str)
{
    cmd_receive->setText(str);
}

void blelinkwindow::clearButton_clicked()
{
    cmd_send->setText("");
    cmd_receive->setText("");
    MainWindow::mutualUi->SetInfo("clear");
}

void blelinkwindow::keepalive()
{
    //uint8_t data[20] = {0xaa,0x01,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //QByteArray array = QByteArray((char *)data, 20);
    QByteArray array("aa01");
    MainWindow::mutualUi->ble_send(array);
}