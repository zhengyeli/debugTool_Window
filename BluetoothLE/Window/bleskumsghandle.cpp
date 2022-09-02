#include "mainwindow.h"
// #include "bleskumsghandle.h"

bleskumsghandle::bleskumsghandle(QWidget *parent)
    : QWidget{parent}
{
    ;
}

bleskumsghandle::~bleskumsghandle()
{

}

void bleskumsghandle::govee_sku_blemsg_handle(const QByteArray &d)
{
    uint8_t msg[20] = {0};
    QByteArray arraydata = d.toHex(); // "aa11"
    MainWindow::mutualUi->calGetBleData(arraydata, msg);
    //qDebug() << "arraydata" << arraydata << arraydata[2];

    uint8_t head = msg[0];
    uint8_t type = msg[1];
    uint8_t sub_type = msg[2];
    uint8_t *data = (uint8_t*)msg + 2;
    if (head == 0xaa)
    {
        switch (type)
        {
        case 0x06:
        {
            QString str;
            str = "wifi soft version : " + (QString)(char*)data;
            MainWindow::mutualUi->ShowInfo(str);
            break;
        }
        case 0x07:
        {
            switch (sub_type)
            {
            case 0x03:
            {
                QString str;
                str = "wifi hard version : " + (QString)(char*)(data + 1);
                MainWindow::mutualUi->ShowInfo(str);
                break;
            }
            case 0x0a:
            {
                QString str;
                str = "mcu soft version : " + (QString)(char*)(data + 1);
                MainWindow::mutualUi->ShowInfo(str);
                break;
            }
            case 0x0b:
            {
                QString str;
                str = "mcu hard version : " + (QString)(char*)(data + 1);
                MainWindow::mutualUi->ShowInfo(str);
                break;
            }
            }
        break;
        }

        }
    }
    else if (head == 0x33)
    {

    }
    else if (head == 0x66)
    {
        switch (type)
        {
        case 0x01:
        {
            break;
        }
        case 0x02:
        {
            ble_test_handle(data[0]);
        }
        }
    }
    // qDebug() << arraydata;
    MainWindow::mutualUi->SetInfo("recd :" + arraydata.toHex(' '));
    MainWindow::mutualUi->blelink->receive(arraydata.toHex(' '));
}

// ble test
void bleskumsghandle::govee_ble_test_stop_check(QByteArray array)
{
    if (array.at(3) != '0' && array.at(3) != 0)
    {
        ble_test_timer_sec = (array.at(3) - 0x30) * 1;
    }

    MainWindow::mutualUi->SetInfo("ble test start...");
    if (ble_test_timer == nullptr){
        ble_test_timer = new QTimer(this);
        connect(ble_test_timer, SIGNAL(timeout()), this, SLOT(ble_test()));
    }else{
        ble_test_timer->stop();
    }

    // ble test init
    index = 0;
    ble_test_count_sum = 0;
    ble_test_count_miss = 0;
    ble_test_timer->start(ble_test_timer_sec);
}

// ble press test on diff platform
void bleskumsghandle::ble_test()
{
    uchar data[20];
    memset(data, 0, 20);
    data[0] = 0x66;
    data[1] = 0x02;
    data[2] = index++;
    MainWindow::mutualUi->calGetBleData(data);
    MainWindow::mutualUi->ble_char_send(data);
}

// ble press test on diff platform
void bleskumsghandle::ble_test_stop()
{
    if (ble_test_timer != nullptr){
        ble_test_timer->stop();
        index = 0;
    }
}

void bleskumsghandle::ble_test_handle(uint8_t type)
{
    // ble test
    static uint8_t ble_test_index = 0;
    if (type == 0)
    {
        // start
        ble_test_index = 0;

    }
    else
    {
        // continue
        ble_test_index++;
        if (type == ble_test_index)
        {
            // get same back data

        }
        else
        {
            ble_test_index = type;
            ble_test_count_miss++;
        }
        ble_test_count_sum++;
    }
    if (DockWidgetBleTest == nullptr){
        // ble test window init
        DockWidgetBleTest = new QDockWidget(this);
        dockWidgetContents = new QWidget(DockWidgetBleTest);
        QTE_bleTest = new QTextEdit(dockWidgetContents);
        QVBoxLayout *verticalLayout;
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        //left_verticalLayout->setSpacing(20);  //间距
        verticalLayout->setContentsMargins(5, 5, 5, 5);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
        //left_verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->addWidget(QTE_bleTest);

        // important
        DockWidgetBleTest->setWidget(dockWidgetContents);
        // 进行布局
        MainWindow::mutualUi->creatNewDockWindow(DockWidgetBleTest, Qt::TopDockWidgetArea, false);
        DockWidgetBleTest->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        DockWidgetBleTest->setWindowTitle("测试");
        DockWidgetBleTest->setObjectName("测试窗口");
        DockWidgetBleTest->setFloating(true);
        DockWidgetBleTest->setVisible(true);
        toolBtn1 = new QToolButton(this);              //创建QToolButton
        toolBtn1->setText(DockWidgetBleTest->windowTitle());
        MainWindow::mutualUi->toolbar->connect(toolBtn1, &QToolButton::clicked, this,
                                               [this]{
            MainWindow::mutualUi->closeAllWindow();
            if (DockWidgetBleTest->isVisible()){
                DockWidgetBleTest->setVisible(false);
            }else{
                DockWidgetBleTest->setVisible(true);
            }
            toolBtn1->setChecked(true);
        });
        MainWindow::mutualUi->toolbar->addWidget(toolBtn1);
        qDebug() << "new again";
    }

    QString str = QString("count sum : [%1], \n\r count miss : [%2]").arg(ble_test_count_sum).arg(ble_test_count_miss);
    QTE_bleTest->setText(str);
    qDebug() << str;
}
