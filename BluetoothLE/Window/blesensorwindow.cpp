#include "blesensorwindow.h"
#include "ui_blesensorwindow.h"
#include "mainwindow.h"

enum
{
    BLE_MSG_SENSOR_SWITCH = 1,
    BLE_MSG_SENSOR_SCAN_AREA,
    BLE_MSG_SENSOR_SENSITIVITY,
    BLE_MSG_SENSOR_LATENCY,
    BLE_MSG_SENSOR_INHIBIT,
    BLE_MSG_SENSOR_ECHO,
    BLE_MSG_SENSOR_UARTOUTPUT,
    BLE_MSG_SENSOR_SAVE,
    BLE_MSG_SENSOR_REFACTORY,
    BLE_MSG_SENSOR_REBOOT,
};

static QWidget *WidgetContents = nullptr;
static QGridLayout *gridlayout = nullptr;

bleSensorWindow::bleSensorWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bleSensorWindow)
{
    // ui->setupUi(this);
    init();
}

bleSensorWindow::~bleSensorWindow()
{
    delete ui;
}

void bleSensorWindow::init()
{
    // dockwidget自带布局
    dockBleSensor = new QDockWidget(this);
    dockBleSensor->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockBleSensor->setWindowTitle("Sensor");
    dockBleSensor->setObjectName("Sensor");
    //dockBleUart->setFloating(true);

    WidgetContents = new QWidget();
    ui->setupUi(WidgetContents);
    dockBleSensor->setWidget(WidgetContents);
    MainWindow::mutualUi->creatNewDockWindow(dockBleSensor, Qt::TopDockWidgetArea,  false);

    toolBtn = new QToolButton(this);              //创建QToolButton
    //toolBtn1->setIcon(QIcon(":/src/menu.png"));                 //添加图标
    toolBtn->setText(dockBleSensor->windowTitle());
    //toolBtn1->setFixedSize(30,20);                              //调图标大小（不是setIconSize)
    MainWindow::mutualUi->toolbar->connect(toolBtn, &QToolButton::clicked, this, &bleSensorWindow::closeWindow);
    MainWindow::mutualUi->toolbar->addWidget(toolBtn);                               //向工具栏添加QToolButton按钮
    dockBleSensor->setVisible(false);

    connect(ui->comboBox, &QComboBox::currentIndexChanged, this, &bleSensorWindow::comboxChange);
    connect(ui->pushButton_Send, &QPushButton::clicked, this, &bleSensorWindow::send);
    connect(ui->stop, &QPushButton::clicked, this, &bleSensorWindow::stop);
    connect(ui->start, &QPushButton::clicked, this, &bleSensorWindow::start);
    connect(ui->save, &QPushButton::clicked, this, &bleSensorWindow::save);
    connect(ui->factory, &QPushButton::clicked, this, &bleSensorWindow::refactory);
}

void bleSensorWindow::comboxChange(int index)
{
    switch(index)
    {
    case 0:
        ui->lineEdit->setText(" 0 7");
        break;
    case 1:
        ui->lineEdit->setText(" 7 1");
        break;
    case 2:
        ui->lineEdit->setText(" 0.5 1.5");
        break;
    case 3:
        ui->lineEdit->setText(" 0.5");
        break;
    case 4:
        ui->lineEdit->setText(" 2 1 1 0.25");
        break;
    default:
        qDebug("Error data");
        return;
        break;
    }
}

void bleSensorWindow::send()
{
    qDebug() << ui->comboBox->currentIndex();
    QString arg = ui->lineEdit->text();
     QByteArray data;
     data.append(0x33);
     data.append(0xCC);
    switch(ui->comboBox->currentIndex())
    {
    case 0:
        data.append(("setRange" + arg).toLocal8Bit());
        break;
    case 1:
        data.append(("setSensitivity" + arg).toLocal8Bit());
        break;
    case 2:
        data.append(("setLatency" + arg).toLocal8Bit());
        break;
    case 3:
        data.append(("setInhibit" + arg).toLocal8Bit());
        break;
    case 4:
        data.append(("setUartOutput" + arg).toLocal8Bit());
        break;
    default:
        qDebug("Error data");
        return;
        break;
    }
    // qDebug() << data << data.toHex();
    MainWindow::mutualUi->ble_pt_send(data);
    MainWindow::mutualUi->ShowInfo(data);
}

void bleSensorWindow::stop()
{
    QByteArray data;
    data.append(0x33);
    data.append(0xCC);
    data.append("sensorStop");
    MainWindow::mutualUi->ble_pt_send(data);
}

void bleSensorWindow::start()
{
    QByteArray data;
    data.append(0x33);
    data.append(0xCC);
    data.append("sensorStart");
    MainWindow::mutualUi->ble_pt_send(data);
}

void bleSensorWindow::save()
{
    QByteArray data;
    data.clear();
    data.append(0x33);
    data.append(0xCC);
    data.append("saveConfig");
    MainWindow::mutualUi->ble_pt_send(data);
}

void bleSensorWindow::refactory()
{
    QByteArray data;
    data.clear();
    data.append(0x33);
    data.append(0xCC);
    data.append("resetCfg");
    MainWindow::mutualUi->ble_pt_send(data);
}

void bleSensorWindow::closeWindow()
{
    MainWindow::mutualUi->closeAllWindow();
    if (dockBleSensor->isVisible()){
        dockBleSensor->setVisible(false);
    }else{
        dockBleSensor->setVisible(true);
    }
    toolBtn->setChecked(true);
}

// handle ble msg
void bleSensorWindow::sensor_blemsg_handle(QByteArray array)
{
    qDebug() << "sensor_blemsg_handle:" << array;
    switch (array.at(1))
    {
    case BLE_MSG_SENSOR_SWITCH:
        break;
    case BLE_MSG_SENSOR_SCAN_AREA:
        ui->label_ScanArea->setText(array);
        break;
    case BLE_MSG_SENSOR_SENSITIVITY:
        ui->label_Sensitivity->setText(array);
        break;
    case BLE_MSG_SENSOR_LATENCY:
        ui->label_Latency->setText(array);
        break;
    case BLE_MSG_SENSOR_INHIBIT:
        ui->label_Inhibit->setText(array);
        break;
    case BLE_MSG_SENSOR_ECHO:
        break;
    case BLE_MSG_SENSOR_UARTOUTPUT:
        ui->label_UartOutput->setText(array);
        break;
    case BLE_MSG_SENSOR_SAVE:
        break;
    case BLE_MSG_SENSOR_REFACTORY:
        break;
    case BLE_MSG_SENSOR_REBOOT:
        break;
    default:
        break;
    }
}

