#include "frmcomtool.h"
#include "ui_frmcomtool.h"
#include "comtool_quihelper.h"
#include "comtool_quihelperdata.h"
#include <QSerialPortInfo>

#define rxBufSize  512
using namespace comTool;
bool isinputText = true;

QString netMode[]{
    "Tcp_client",
    "Tcp_Server",
    "Udp_Client",
    "Udp_Server",
};

QString hotkey[]{
    "govee",
    "dev_info",
    "ifconfig",
    "reboot",
    "log print 0",
    "log print 1",
    "log print 2",
    "log print 3",
    "log print 4",
    "log enable mcu",
    "log disable mcu",
    "log enable ble",
    "log disable ble",
    "log enable scan",
    "log disable scan",
};

void frmComTool::btnFunction()
{
    QObject *obj = sender();
    QPushButton *button = (QPushButton *)obj;
    if (com != nullptr)
    {
        QByteArray data = button->text().toLatin1();
        data.append(0x0d);
        com->write(data);
    }

    if (udpOk == true)
    {
        QByteArray data = button->text().toLatin1();
        data.append(0x0d);
        udpsocket->writeDatagram(data.data(), QHostAddress(AppConfig::ServerIP), AppConfig::ServerPort);
        ui->txtMain->insertPlainText(data);
    }

    if (tcpOk == true)
    {
        QByteArray data = button->text().toLatin1();
        data.append(0x0d);
        tcpsocket->write(data.data());
        ui->txtMain->insertPlainText(data);
    }
}

frmComTool::frmComTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmComTool)
{
    //设置编码以及加载中文翻译文件
    QUIHelper::initAll();
    //读取配置文件
    AppConfig::ConfigFile = QString("%1/%2.ini").arg(QUIHelper::appPath()).arg(QUIHelper::appName());
    AppConfig::readConfig();

    AppData::Intervals << "1" << "10" << "20" << "50" << "100" << "200" << "300" << "500" << "1000" << "1500" << "2000" << "3000" << "5000" << "10000";
    AppData::readSendData();
    AppData::readDeviceData();

    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    QUIHelper::setFormInCenter(this);

    // 自己定义一个光标
    ui->txtMain->setFontWeight(QFont::Weight::Bold);
    ui->txtMain->insertPlainText("#");

    // init hot key
    int i = 0;
    QWidget *w = new QWidget(ui->tabWidget);
    ui->tabWidget->addTab(w, "hotkey");

    //QGridLayout gridlayout = QGridLayout(ui->tab_3);
    QVBoxLayout *verticalLayout = new QVBoxLayout(w);
    for (i = 0; i < sizeof(hotkey)/sizeof(QString); i++) {
        QPushButton *btn = new QPushButton(w);
        btn->setText(hotkey[i]);
        connect(btn, &QPushButton::clicked, this, &frmComTool::btnFunction);
        verticalLayout->addWidget(btn);
    }
    w->setLayout(verticalLayout);
}

frmComTool::~frmComTool()
{
    delete ui;
}

void frmComTool::initForm()
{
    comOk = false;
    com = 0;
    sleepTime = 10;
    receiveCount = 0;
    sendCount = 0;
    isShow = true;

    ui->cboxSendInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);

    //读取数据
    timerRead = new QTimer(this);
    timerRead->setInterval(100);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readData()));

    //发送数据
    timerSend = new QTimer(this);
    connect(timerSend, SIGNAL(timeout()), this, SLOT(sendData()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(sendData()));

    //保存数据
    timerSave = new QTimer(this);
    connect(timerSave, SIGNAL(timeout()), this, SLOT(saveData()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(saveData()));

    ui->tabWidget->setCurrentIndex(0);
    changeEnable(false);

    //TCP
    tcpOk = false;
    tcpsocket = new QTcpSocket(this);
    tcpsocket->abort();
    connect(tcpsocket, SIGNAL(readyRead()), this, SLOT(readDataNet()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(tcpsocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#else
    connect(tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#endif

    //UDP
    udpOk = false;
    udpsocket = new QUdpSocket(this);
    udpsocket->abort();
    connect(udpsocket, SIGNAL(readyRead()), this, SLOT(readUdpDataNet()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(tcpsocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#else
    connect(udpsocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));
#endif

    timerConnect = new QTimer(this);
    connect(timerConnect, SIGNAL(timeout()), this, SLOT(connectNet()));
    timerConnect->setInterval(3000);
    timerConnect->start();

#ifdef __arm__
    ui->widgetRight->setFixedWidth(280);
#endif

    //
    ui->txtMain->installEventFilter(this);
}

void frmComTool::initConfig()
{
#if 0
    QStringList comList ;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        comList << info.portName();
    }

    if (infos.isEmpty()){
        ui->btnOpen->setText("扫描");
    }
#else
    QStringList comList ;
    char str[16] = {0};
    for (int i = 0; i < 40; i++) {
        sprintf(str, "COM%d", i);
        comList << str;
    }
#endif

    ui->cboxPortName->addItems(comList);
    ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(AppConfig::PortName));
    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList baudList;
    baudList << "50" << "75" << "100" << "134" << "150" << "200" << "300" << "600" << "1200"
             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

    ui->cboxBaudRate->addItems(baudList);
    ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(AppConfig::BaudRate)));
    connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList dataBitsList;
    dataBitsList << "5" << "6" << "7" << "8";

    ui->cboxDataBit->addItems(dataBitsList);
    ui->cboxDataBit->setCurrentIndex(ui->cboxDataBit->findText(QString::number(AppConfig::DataBit)));
    connect(ui->cboxDataBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList parityList;
    parityList << "无" << "奇" << "偶";
#ifdef Q_OS_WIN
    parityList << "标志";
#endif
    parityList << "空格";

    ui->cboxParity->addItems(parityList);
    ui->cboxParity->setCurrentIndex(ui->cboxParity->findText(AppConfig::Parity));
    connect(ui->cboxParity, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList stopBitsList;
    stopBitsList << "1";
#ifdef Q_OS_WIN
    stopBitsList << "1.5";
#endif
    stopBitsList << "2";

    ui->cboxStopBit->addItems(stopBitsList);
    ui->cboxStopBit->setCurrentIndex(ui->cboxStopBit->findText(QString::number(AppConfig::StopBit)));
    connect(ui->cboxStopBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexSend->setChecked(AppConfig::HexSend);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceive);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::Debug);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoClear->setChecked(AppConfig::AutoClear);
    connect(ui->ckAutoClear, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSend);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSave->setChecked(AppConfig::AutoSave);
    connect(ui->ckAutoSave, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    QStringList sendInterval;
    QStringList saveInterval;
    sendInterval << "100" << "300" << "500";

    for (int i = 1000; i <= 10000; i = i + 1000) {
        sendInterval << QString::number(i);
        saveInterval << QString::number(i);
    }

    ui->cboxSendInterval->addItems(sendInterval);
    ui->cboxSaveInterval->addItems(saveInterval);

    ui->cboxSendInterval->setCurrentIndex(ui->cboxSendInterval->findText(QString::number(AppConfig::SendInterval)));
    connect(ui->cboxSendInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    ui->cboxSaveInterval->setCurrentIndex(ui->cboxSaveInterval->findText(QString::number(AppConfig::SaveInterval)));
    connect(ui->cboxSaveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    timerSend->setInterval(AppConfig::SendInterval);
    timerSave->setInterval(AppConfig::SaveInterval);

    if (AppConfig::AutoSend) {
        timerSend->start();
    }

    if (AppConfig::AutoSave) {
        timerSave->start();
    }

    //串口转网络部分
    ui->cboxMode->setCurrentIndex(ui->cboxMode->findText(AppConfig::Mode));
    connect(ui->cboxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::ServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::ServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::ListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    QStringList values;
    values << "0" << "10" << "50";

    for (int i = 100; i < 1000; i = i + 100) {
        values << QString("%1").arg(i);
    }

    ui->cboxSleepTime->addItems(values);

    ui->cboxSleepTime->setCurrentIndex(ui->cboxSleepTime->findText(QString::number(AppConfig::SleepTime)));
    connect(ui->cboxSleepTime, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoConnect->setChecked(AppConfig::AutoConnect);
    connect(ui->ckAutoConnect, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
}

void frmComTool::txtMainInsertText(const QString &text)
{

}

bool frmComTool::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->txtMain)                    //判断是不是我的事件
    {
        if (event->type() == QEvent::MouseButtonPress)    //如果label触发了鼠标点击的事件
        {
            qDebug()<<"press";
            return true;                            //表示停止处理该事件，此时目标对象和后面安装的事件过滤器就无法获得该事件
        }
        else if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            ui->txtMain->verticalScrollBar()->setSliderPosition(ui->txtMain->verticalScrollBar()->maximum());
            QTextCursor cursor = ui->txtMain->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->txtMain->setTextCursor(cursor);
            if (com != nullptr)
            {
                com->write(keyEvent->text().toLocal8Bit());
            }

            if (udpOk == true)
            {
                QByteArray data = keyEvent->text().toLocal8Bit();
                udpsocket->writeDatagram(data.data(), QHostAddress(AppConfig::ServerIP), AppConfig::ServerPort);
                ui->txtMain->insertPlainText(data);
            }

            if (tcpOk == true)
            {
                QByteArray data = keyEvent->text().toLocal8Bit();
                tcpsocket->write(data.data());
                ui->txtMain->insertPlainText(data);
            }
#if 1
            if (keyEvent->key() == 32) //空格
            {
                //获取当前文本光标
                QTextCursor cursor = ui->txtMain->textCursor();
                //将光标移动到文本结尾
                cursor.movePosition(QTextCursor::PreviousCharacter);
                ui->txtMain->setTextCursor(cursor);
                ui->txtMain->insertPlainText(" ");
            }
            else if (keyEvent->key() == 0x0d) // enter
            {
                ui->txtMain->insertPlainText("\n");
            }
#endif
            return true;
        }
        else
        {
            //qDebug()<<"nopress";
            return false;                            //返回false，则表示事件需要作进一步处理
        }
    }
    return frmComTool::eventFilter(obj,event);        //返回默认的事件过滤器
}

void frmComTool::saveConfig()
{
    AppConfig::PortName = ui->cboxPortName->currentText();
    AppConfig::BaudRate = ui->cboxBaudRate->currentText().toInt();
    AppConfig::DataBit = ui->cboxDataBit->currentText().toInt();
    AppConfig::Parity = ui->cboxParity->currentText();
    AppConfig::StopBit = ui->cboxStopBit->currentText().toDouble();

    AppConfig::HexSend = ui->ckHexSend->isChecked();
    AppConfig::HexReceive = ui->ckHexReceive->isChecked();
    AppConfig::Debug = ui->ckDebug->isChecked();
    AppConfig::AutoClear = ui->ckAutoClear->isChecked();

    AppConfig::AutoSend = ui->ckAutoSend->isChecked();
    AppConfig::AutoSave = ui->ckAutoSave->isChecked();

    int sendInterval = ui->cboxSendInterval->currentText().toInt();
    if (sendInterval != AppConfig::SendInterval) {
        AppConfig::SendInterval = sendInterval;
        timerSend->setInterval(AppConfig::SendInterval);
    }

    int saveInterval = ui->cboxSaveInterval->currentText().toInt();
    if (saveInterval != AppConfig::SaveInterval) {
        AppConfig::SaveInterval = saveInterval;
        timerSave->setInterval(AppConfig::SaveInterval);
    }

    AppConfig::Mode = ui->cboxMode->currentText();
    AppConfig::ServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::ServerPort = ui->txtServerPort->text().toInt();
    AppConfig::ListenPort = ui->txtListenPort->text().toInt();
    AppConfig::SleepTime = ui->cboxSleepTime->currentText().toInt();
    AppConfig::AutoConnect = ui->ckAutoConnect->isChecked();

    AppConfig::writeConfig();
}

void frmComTool::changeEnable(bool b)
{
    ui->cboxBaudRate->setEnabled(!b);
    ui->cboxDataBit->setEnabled(!b);
    ui->cboxParity->setEnabled(!b);
    ui->cboxPortName->setEnabled(!b);
    ui->cboxStopBit->setEnabled(!b);
    ui->btnSend->setEnabled(b);
    ui->ckAutoSend->setEnabled(b);
    ui->ckAutoSave->setEnabled(b);
}

void frmComTool::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 10000;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (!isShow) {
        return;
    }

    //过滤回车换行符
    QString strData = data;
    //strData = strData.trimmed();
    //strData = strData.simplified();
#ifdef Q_OS_LINUX
    //strData = strData.replace("\r", "");
#endif

    strData = strData.replace("\n\r", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "串口发送 ";
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    } else if (type == 1) {
        strType = "串口接收 ";
        ui->txtMain->setTextColor(QColor("red"));
        strData = strData.replace("\n", "<br />"); // html's \r
    } else if (type == 2) {
        strType = "处理延时 ";
        ui->txtMain->setTextColor(QColor("gray"));
    } else if (type == 3) {
        strType = "正在校验 ";
        ui->txtMain->setTextColor(QColor("green"));
    } else if (type == 4) {
        strType = "网络发送 ";
        ui->txtMain->setTextColor(QColor(24, 189, 155));
    } else if (type == 5) {
        strType = "网络接收 ";
        ui->txtMain->setTextColor(QColor(255, 107, 107));
        strData = strData.replace("\r", "");
    } else if (type == 6) {
        strType = "提示信息 ";
        ui->txtMain->setTextColor(QColor(100, 184, 255));
    }

    if (type != 1)
    strData = QString("时间[%1] [%2] %3\r\n").arg(TIMEMS).arg(strType).arg(strData);
    qDebug() << "strData :" << strData;
    // 进度条在尾部，实时显示打印
    if (ui->txtMain->verticalScrollBar()->value() == ui->txtMain->verticalScrollBar()->maximum()){
        isinputText = true;
    }else{
        isinputText = false;
    }
    // 光标的处理
    QTextCursor cursor = ui->txtMain->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.deletePreviousChar();
    ui->txtMain->setTextCursor(cursor);

    switch (type) {
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        ui->txtMain->insertPlainText(strData);
        break;
    case 1:
        // 文本替换
        strData.replace("ERR","<font color=red>ERR</font>",Qt::CaseSensitive);
        strData.replace("WARN","<font color=yellow>WARN</font>",Qt::CaseSensitive);
        strData.replace("INF","<font color=yellow>INF</font>",Qt::CaseSensitive);
        strData.replace("success","<font color=green>success</font>",Qt::CaseSensitive);

        // insertHtml 支持html格式颜色文字
        ui->txtMain->insertHtml(strData);
        break;
    }

    ui->txtMain->insertPlainText("#");

    if (isinputText){
        ui->txtMain->verticalScrollBar()->setSliderPosition(ui->txtMain->verticalScrollBar()->maximum());
    }

    currentCount++;
}

void frmComTool::readData()
{
    if (com->bytesAvailable() <= 0) {
        return;
    }

    QUIHelper::sleep(sleepTime);
    QByteArray data = com->readAll();
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }

    if (data.contains("\b \b")) // backspace
    {
        //qDebug() << data;
        //获取当前文本光标
        QTextCursor cursor = ui->txtMain->textCursor();
        //将光标移动到文本结尾
        cursor.movePosition(QTextCursor::PreviousCharacter);
        //判断当前是否选中了文本，如果选中了文本则取消选中的文本，再删除前一个字符
        if(cursor.hasSelection())
            cursor.clearSelection();
        //删除前一个字符
        while (data.indexOf("\b \b") != -1)
        {
           //cursor.movePosition(QTextCursor::End);
           cursor.deletePreviousChar();
           ui->txtMain->setTextCursor(cursor);
           data.remove(data.indexOf("\b \b"), data.indexOf("\b \b")+3);
        }
        ui->txtMain->verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
        //data = data.replace("\b \b", "");
    }

    if (isShow) {
        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QUIHelperData::byteArrayToHexStr(data);
        } else {
            //buffer = QUIHelperData::byteArrayToAsciiStr(data);
            buffer = QString::fromLocal8Bit(data);
        }

        //启用调试则模拟调试数据
        if (ui->ckDebug->isChecked()) {
            int count = AppData::Keys.count();
            for (int i = 0; i < count; i++) {
                if (buffer.startsWith(AppData::Keys.at(i))) {
                    sendData(AppData::Values.at(i));
                    break;
                }
            }
        }

        append(1, buffer);
        receiveCount = receiveCount + data.size();
        ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));

        //启用网络转发则调用网络发送数据
        if (tcpOk) {
            tcpsocket->write(data);
            append(4, QString(buffer));
        }
    }
}

void frmComTool::sendData()
{
    QString str = ui->cboxData->currentText();
    if (str.isEmpty()) {
        ui->cboxData->setFocus();
        return;
    }

    sendData(str);

    if (ui->ckAutoClear->isChecked()) {
        ui->cboxData->setCurrentIndex(-1);
        ui->cboxData->setFocus();
    }
}

void frmComTool::sendData(QString data)
{
    if (com == 0 || !com->isOpen()) {
        return;
    }

    //短信猫调试
    if (data.startsWith("AT")) {
        data += "\r";
    }

    QByteArray buffer;
    if (ui->ckHexSend->isChecked()) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    }

    com->write(buffer);
    append(0, data);
    sendCount = sendCount + buffer.size();
    ui->btnSendCount->setText(QString("发送 : %1 字节").arg(sendCount));
}

void frmComTool::saveData()
{
    QString tempData = ui->txtMain->toPlainText();

    if (tempData == "") {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
    QString fileName = QString("%1/%2.txt").arg(QUIHelper::appPath()).arg(name);

    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << tempData;
    file.close();

    on_btnClear_clicked();
}

void frmComTool::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开串口") {
        com = new QextSerialPort(ui->cboxPortName->currentText(), QextSerialPort::Polling);
        comOk = com->open(QIODevice::ReadWrite);

        if (comOk) {
            //改变tab名字
            Tab->setTabText(tabIndex, ui->cboxPortName->currentText());
            //清空缓冲区
            com->flush();
            //设置波特率
            com->setBaudRate((BaudRateType)ui->cboxBaudRate->currentText().toInt());
            //设置数据位
            com->setDataBits((DataBitsType)ui->cboxDataBit->currentText().toInt());
            //设置校验位
            com->setParity((ParityType)ui->cboxParity->currentIndex());
            //设置停止位
            com->setStopBits((StopBitsType)ui->cboxStopBit->currentIndex());
            com->setFlowControl(FLOW_OFF);
            com->setTimeout(10);

            changeEnable(true);
            ui->btnOpen->setText("关闭串口");
            timerRead->start();
        }
    } else if (ui->btnOpen->text() == "扫描"){
        QStringList comList ;
        const auto infos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : infos) {
            comList << info.portName();
        }
        if (infos.isEmpty()){
            ui->btnOpen->setText("扫描");
        }

        ui->cboxPortName->addItems(comList);
    }else{
        timerRead->stop();
        com->close();
        com->deleteLater();

        changeEnable(false);
        ui->btnOpen->setText("打开串口");
        //on_btnClear_clicked();
        comOk = false;
    }
}

void frmComTool::on_btnSendCount_clicked()
{
    sendCount = 0;
    ui->btnSendCount->setText("发送 : 0 字节");
}

void frmComTool::on_btnReceiveCount_clicked()
{
    receiveCount = 0;
    ui->btnReceiveCount->setText("接收 : 0 字节");
}

void frmComTool::on_btnStopShow_clicked()
{
    if (ui->btnStopShow->text() == "停止显示") {
        isShow = false;
        ui->btnStopShow->setText("开始显示");
    } else {
        isShow = true;
        ui->btnStopShow->setText("停止显示");
    }
}

void frmComTool::on_btnData_clicked()
{
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg("send.txt");
    QFile file(fileName);
    if (!file.exists()) {
        return;
    }

    if (ui->btnData->text() == "管理数据") {
        ui->txtMain->setReadOnly(false);
        ui->txtMain->clear();
        file.open(QFile::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        ui->txtMain->setText(in.readAll());
        file.close();
        ui->btnData->setText("保存数据");
    } else {
        ui->txtMain->setReadOnly(true);
        file.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << ui->txtMain->toPlainText();
        file.close();
        ui->txtMain->clear();
        ui->btnData->setText("管理数据");
        AppData::readSendData();
    }
}

void frmComTool::on_btnClear_clicked()
{
    append(0, "", true);
    ui->txtMain->setFontWeight(QFont::Weight::Bold);
    ui->txtMain->insertPlainText("#");
    ui->txtMain->setFontWeight(QFont::Weight::Normal);
}

void frmComTool::on_btnStart_clicked()
{
    int i;
    QString mode = ui->cboxMode->currentText();
    for (i = 0; i < sizeof(netMode)/sizeof(QString); i++) {
        if (netMode[i] == mode){
            break;
        }
    }
    if (ui->btnStart->text() == "启动") {
        if (AppConfig::ServerIP == "" || AppConfig::ServerPort == 0) {
            append(6, "IP地址和远程端口不能为空");
            return;
        }

        switch (i)
        {
        case 0: // tcp client
            tcpsocket->connectToHost(AppConfig::ServerIP, AppConfig::ServerPort);
            if (tcpsocket->waitForConnected(100)) {
                ui->btnStart->setText("停止");
                append(6, "连接tcp服务器成功");
                tcpOk = true;
            }else{
                append(6, "连接tcp服务器fail");
            }
            break;
        case 1: // tcp server
            tcpsocket->bind(QHostAddress::LocalHost, AppConfig::ServerPort, QTcpSocket::BindFlag::DefaultForPlatform);
            ui->btnStart->setText("停止");
            append(6, "bind成功");
            break;
        case 2: // udp client
        {
            QByteArray data = "govee";
            if (udpsocket->writeDatagram(data.data(), QHostAddress(AppConfig::ServerIP), AppConfig::ServerPort) > 0){
                append(6, "连接udp服务器成功");
                udpOk = true;
            }
            else{
                append(6, "连接udp服务器fail");
            }
        }
            break;
        case 3: // udp server
            if (udpsocket->bind(QHostAddress::LocalHost, AppConfig::ServerPort,  QUdpSocket::BindFlag::DefaultForPlatform))
            {
                ui->btnStart->setText("停止");
                append(6, "bind udp 成功");
                udpOk = true;

            }else{
                append(6, "bind udp服务器fail");
            }
            break;
        }
    } else {


        switch (i)
        {
        case 0: // tcp client
            tcpsocket->disconnectFromHost();
            if (tcpsocket->state() == QAbstractSocket::UnconnectedState || tcpsocket->waitForDisconnected(100)) {
                ui->btnStart->setText("启动");
                append(6, "断开服务器成功");
                tcpOk = false;
            }
            break;
        case 1: // tcp server
            tcpsocket->close();
            append(6, "断开服务器成功");
            break;
        case 2: // udp client
        case 3: // udp server
            udpsocket->close();
            append(6, "断开成功");
            break;
        }
        ui->btnStart->setText("启动");
    }
}

void frmComTool::on_ckAutoSend_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->cboxSendInterval->setEnabled(false);
        timerSend->stop();
    } else {
        ui->cboxSendInterval->setEnabled(true);
        timerSend->start();
    }
}

void frmComTool::on_ckAutoSave_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->cboxSaveInterval->setEnabled(false);
        timerSave->stop();
    } else {
        ui->cboxSaveInterval->setEnabled(true);
        timerSave->start();
    }
}

void frmComTool::connectNet()
{
    if (!tcpOk && AppConfig::AutoConnect && ui->btnStart->text() == "启动") {
        if (AppConfig::ServerIP != "" && AppConfig::ServerPort != 0) {
            tcpsocket->connectToHost(AppConfig::ServerIP, AppConfig::ServerPort);
            if (tcpsocket->waitForConnected(100)) {
                ui->btnStart->setText("停止");
                append(6, "连接服务器成功");
                tcpOk = true;
            }
        }
    }
}

void frmComTool::readDataNet()
{
    if (tcpsocket->bytesAvailable() > 0) {
        QUIHelper::sleep(AppConfig::SleepTime);
        QByteArray data = tcpsocket->readAll();

        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QUIHelperData::byteArrayToHexStr(data);
        } else {
            buffer = QUIHelperData::byteArrayToAsciiStr(data);
        }

        append(5, buffer);

        //将收到的网络数据转发给串口
//        if (comOk) {
//            com->write(data);
//            append(0, buffer);
//        }
    }
}

void frmComTool::readUdpDataNet()
{
    char data[rxBufSize] = {'\0'};
    if (udpsocket->bytesAvailable() > 0) {
        //QUIHelper::sleep(AppConfig::SleepTime);
        udpsocket->readDatagram(data, rxBufSize);
//        QString buffer;
//        if (ui->ckHexReceive->isChecked()) {
//            buffer = QUIHelperData::byteArrayToHexStr(data);
//        } else {
//            buffer = QUIHelperData::byteArrayToAsciiStr(data);
//        }
//        qDebug() << buffer;
        append(5, data);

        //将收到的网络数据转发给串口
//        if (comOk) {
//            com->write(data);
//            append(0, buffer);
//        }
    }

    foreach (QUdpSocket *sock, m_udpSocketlist) {
       if (sock->bytesAvailable() > 0){
           QHostAddress addr;
           quint16 port;
           sock->readDatagram(data, rxBufSize, &addr, &port);

           QString buffer;
           if (ui->ckHexReceive->isChecked()) {
               buffer = QUIHelperData::byteArrayToHexStr(data);
           } else {
               buffer = QUIHelperData::byteArrayToAsciiStr(data);
           }
           if (strncmp(data, "Server", 6) == 0){
               m_ipServerlist << addr.toString();
               append(5,buffer);
           }
       }
    }
}

void frmComTool::readErrorNet()
{
    ui->btnStart->setText("启动");
    append(6, QString("连接服务器失败,%1").arg(tcpsocket->errorString()));
    tcpsocket->disconnectFromHost();
    tcpOk = false;

    append(6, QString("连接服务器失败,%1").arg(udpsocket->errorString()));
    udpsocket->disconnectFromHost();
    udpOk = false;
}

void frmComTool::on_scan_clicked(bool b)
{
    if (b)
        return;
    int i;
    QString mode = ui->cboxMode->currentText();
    for (i = 0; i < sizeof(netMode)/sizeof(QString); i++) {
        if (netMode[i] == mode){
            break;
        }
    }
    switch (i)
    {
    case 2: {// udp client
        QByteArray data = "where are you?";
        if (m_udpSocketlist.isEmpty()){
            QList<QNetworkInterface> networkinterfaces = QNetworkInterface::allInterfaces();
            foreach (QNetworkInterface interfaces, networkinterfaces)
            {
                foreach (QNetworkAddressEntry entry, interfaces.addressEntries())
                {
                    QHostAddress broadcastAddress = entry.broadcast();
                    if (broadcastAddress != QHostAddress::Null
                            && entry.ip() != QHostAddress::LocalHost
                            && entry.ip().protocol() == QAbstractSocket::IPv4Protocol
                            )
                    {

                        QUdpSocket * sock = new QUdpSocket();
                        if(sock->bind(entry.ip(),AppConfig::ServerPort))
                        {
                            qDebug() << "bind ok" << entry.ip();
                        }
                        connect(sock,SIGNAL(readyRead()),this,SLOT(readUdpDataNet()));
                        sock->writeDatagram(data.data(), QHostAddress::Broadcast, AppConfig::ServerPort);
                        m_udpSocketlist.append(sock);
                    }
                }
            }
        }
        else{
            foreach (QUdpSocket *sock, m_udpSocketlist) {
               if (sock->state() == QAbstractSocket::SocketState::BoundState){
                   sock->writeDatagram(data.data(), QHostAddress::Broadcast, AppConfig::ServerPort);
               }
            }
        }
        }
        break;
    default:
        append(6, "only work at udp client mode");
        break;
    }
}

