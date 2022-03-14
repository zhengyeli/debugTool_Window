#include "tcpsocketclient.h"

char *str = nullptr;
int i = 0;
int length = 0;
unsigned char send_str[20] = {0};
int len = 0;
QTimer *configWifi_timer = nullptr;

typedef struct{
   char ssid[33];
   char password[65];
   char environment;
   char timezone;
   char iot_environment;
   char time_offset;
   char url_len[2];
   char url[129];
}wifi_para;
tcpSocketClient *tcpSocketClient::Client = nullptr;

tcpSocketClient::tcpSocketClient(QWidget *parent)
    : QWidget{parent}
{
    Client = this;
    tcp_socket = new QTcpSocket();
    //2. 已连接、数据可读、失败信号连接
    connect(tcp_socket, &QTcpSocket::connected, this, &tcpSocketClient::connected);
    connect(tcp_socket, &QTcpSocket::disconnected, this, &tcpSocketClient::disconnected);
    connect(tcp_socket, &QIODevice::readyRead, this, &tcpSocketClient::readyRead);
    connect(tcp_socket, &QTcpSocket::errorOccurred, this, &tcpSocketClient::error);
//    3. 与服务器端建立连接
//    tcp_socket->connectToHost("192.168.56.1", 55555);
//    tcp_socket->connectToHost("192.168.56.1", 1024, QIODeviceBase::ReadWrite, QAbstractSocket::AnyIPProtocol);
//    4. 同步处理-等待数据可读
//    tcp_socket->waitForReadyRead();

    dockSocket = new QDockWidget();
    dockSocket->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockSocket->setWindowTitle("Tcp");
    dockSocket->setObjectName("tcpSocket窗口");

    QWidget *dockWidgetContents;
    dockWidgetContents = new QWidget(dockSocket);

    ip = new QLineEdit(dockWidgetContents);
    ip->setText("192.168.56.255");
    ip->setMaximumSize(100, 25);

    port = new QLineEdit(dockWidgetContents);
    port->setText("55555");
    port->setMaximumSize(60, 25);

    link = new QPushButton(dockWidgetContents);
    link->setText("连接");
    link->setMaximumSize(50, 25);

    button_clear = new QPushButton(dockWidgetContents);
    button_clear->setText("清除");
    button_clear->setMaximumSize(50, 25);

    message_box = new QTextEdit(dockWidgetContents);

    text_Ssid = new QTextEdit(dockWidgetContents);
    text_Ssid->setText("Govee");
    text_Ssid->setMaximumSize(100, 25);

    text_Password = new QTextEdit(dockWidgetContents);
    text_Password->setText("starstarlight");
    text_Password->setMaximumSize(100, 25);

    button_set_wifi = new QPushButton(dockWidgetContents);
    button_set_wifi->setMaximumSize(50, 25);
    button_set_wifi->setText("确认");

    QHBoxLayout *HBoxLayout0;
    HBoxLayout0 = new QHBoxLayout();
    HBoxLayout0->addWidget(text_Ssid);
    HBoxLayout0->addWidget(text_Password);
    HBoxLayout0->addWidget(button_set_wifi);

    QHBoxLayout *HBoxLayout1;
    HBoxLayout1 = new QHBoxLayout();
    HBoxLayout1->addWidget(ip);
    HBoxLayout1->addWidget(port);
    HBoxLayout1->addWidget(link);
    HBoxLayout1->addWidget(button_clear);

    QHBoxLayout *HBoxLayout2;
    HBoxLayout2 = new QHBoxLayout();
    HBoxLayout2->addWidget(message_box);

    QVBoxLayout *VBoxLayout;
    VBoxLayout = new QVBoxLayout(dockWidgetContents);
    VBoxLayout->addItem(HBoxLayout1);
    VBoxLayout->addItem(HBoxLayout0);
    VBoxLayout->addItem(HBoxLayout2);

    dockWidgetContents->setLayout(VBoxLayout);
    dockSocket->setWidget(dockWidgetContents);
    MainWindow::mutualUi->creatNewDockWindow(dockSocket, Qt::TopDockWidgetArea,  false);

    connect(link           , &QPushButton::clicked, this, &tcpSocketClient::clicked);
    connect(button_clear   , &QPushButton::clicked, this, [this](bool){message_box->setText("");});
    connect(button_set_wifi, SIGNAL(clicked(bool)), this, SLOT(setwifi()));

    QToolButton *toolBtn = new QToolButton(this);              //创建QToolButton
    toolBtn->setText(dockSocket->windowTitle());
    MainWindow::mutualUi->toolbar->connect(toolBtn, &QToolButton::clicked, this, &tcpSocketClient::closeWindow);
    MainWindow::mutualUi->toolbar->addWidget(toolBtn);
    dockSocket->setVisible(false);
}

void tcpSocketClient::closeWindow()
{
    MainWindow::mutualUi->closeAllWindow();
    if (dockSocket->isVisible()){
        dockSocket->setVisible(false);
    }else{
        dockSocket->setVisible(true);
    }
}

tcpSocketClient::~tcpSocketClient()
{
    delete message_box;
    delete tcp_socket;
    delete ip;
    delete port;
    delete link;
}

void tcpSocketClient::connected()
{
    qDebug() << "connected";
    message_box->append("connected");
    link->setText("disconnect");
}

void tcpSocketClient::disconnected()
{
    qDebug() << "disconnected";
    link->setText("connect");
}

void tcpSocketClient::readyRead()
{
    QString string = tcp_socket->readAll();
    qDebug() << "readyRead " << string;
    if (string.contains("From:Server")){
        message_box->append("get server packet : ");
        return;
    }
    message_box->append(string);
    MainWindow::mutualUi->SetInfo(string);
    //write("1234");
}

void tcpSocketClient::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << "error " << socketError;
    if (socketError == QAbstractSocket::SocketTimeoutError){
        message_box->append("timeout");
        return;
    }
    message_box->setText("error");
}

void tcpSocketClient::write(const char *data)
{
    tcp_socket->write(data);
    tcp_socket->waitForBytesWritten();
    //当使用waitForBytesWritten()后，QTcpSocket才真正发送数据。
}

bool tcpSocketClient::checkValidIp(int Adress)
{
    tcp_socket->abort();
    QString ip = "192.168.56.";
    tcp_socket->connectToHost(ip+QString::number(Adress), 55555);
    //等待timeout时间，如果仍然不通，则异常
    return tcp_socket->waitForConnected(10);
}

void tcpSocketClient::getresult(bool result[255])
{
    for (int i = 1; i < 255; i++){
        lastip[i] = result[i];
    }
    qDebug() << "lastip";
}

void tcpSocketClient::clicked()
{
    if (tcp_socket->state() == QAbstractSocket::ConnectedState){
        tcp_socket->disconnectFromHost();
        message_box->append("disconnected");
    }else if (tcp_socket->state() == QAbstractSocket::UnconnectedState){
        tcp_socket->connectToHost(ip->text(), port->text().toUInt());
        message_box->append("connecting to ip: " + ip->text() + " port: " + port->text());
        if (!tcp_socket->waitForConnected(2000)){
            message_box->append("disconnected");
        }
    }
}

void tcpSocketClient::clear()
{
    text_Ssid->clear();
    text_Password->clear();
}

void tcpSocketClient::setwifi()
{
    QString url = "https://dev-device.govee.com";
    if (text_Password->toPlainText().isNull() || text_Password->toPlainText().isNull()){
        qDebug() << "ssid or password is null";
    }

    if (text_Ssid->toPlainText().length() == 0 || text_Password->toPlainText().length() < 8){
        MainWindow::mutualUi->SetInfo("err ssid or password");
    }
    len = text_Ssid->toPlainText().length() + text_Password->toPlainText().length() + url.length() + 8;

    wifi_para wifi_buf;
    memcpy(wifi_buf.ssid,qPrintable(text_Ssid->toPlainText()),text_Ssid->toPlainText().length()+1);
    memcpy(wifi_buf.password,qPrintable(text_Password->toPlainText()),text_Password->toPlainText().length()+1);
    memcpy(wifi_buf.url,&url,url.length());

    wifi_buf.environment = 1;
    wifi_buf.iot_environment = 1;
    wifi_buf.time_offset = 8;
    wifi_buf.url_len[0] = (url.length() & 0xff00) >> 8;
    wifi_buf.url_len[1] = url.length() & 0x00ff;

    str = (char*)malloc(len);
    sprintf(str, "%c%s%c%s%c%c%c%c%x%s",int(text_Ssid->toPlainText().length()),wifi_buf.ssid,
                                        int(text_Password->toPlainText().length()),wifi_buf.password,
                                        wifi_buf.environment,wifi_buf.time_offset,wifi_buf.iot_environment,
                                        wifi_buf.url_len[0],wifi_buf.url_len[1],qPrintable(url));

    qDebug() << "start";
    for (int j = 0; j < len; j++ ){
        printf("%x ",*(str+j));
    }
    qDebug() << "end";

    send_str[0] = 0xa1;
    send_str[1] = 0x11;
    length = ((len/16 == 0) ? len/16 : len/16 + 1) + 2;
    qDebug() << "len:" << len << "length:" << length;


    configWifi_timer = new QTimer(this);
    connect(configWifi_timer, SIGNAL(timeout()), this, SLOT(timerSendWifi()));
    configWifi_timer->start(500);


}

void tcpSocketClient::timerSendWifi()
{

    //for (; i < length; i++) {
        if (i == 0){
            // first packet
            memset((send_str+3), 0, 16);
            send_str[2] = 0x00;
            send_str[3] = length - 2;
        }else if (1 <= i && i < length - 1){
            memset((send_str+3), 0, 16);
            send_str[2] = i;
            if ((len - 16 * (i-1)) / 16 > 0){
                memcpy((send_str+3), str+16*(i-1),16);
            }else{
                memcpy((send_str+3), str+16*(i-1),(len - 16 * (i-1)) % 16);
            }
        }else{
            i = 0;
            memset(send_str+3, 0, 16);
            send_str[2] = 0xff;
            len = 0;
            length = 0;

            if (configWifi_timer != nullptr){
                configWifi_timer->stop();
                delete  configWifi_timer;
                //configWifi_timer = nullptr;
            }

            MainWindow::mutualUi->ble_char_send(send_str);
            if (str != nullptr){
#ifdef Q_OS_LINUX
                free((void *)str);
                //str = nullptr;
#endif
            }
            return;
        }
        i++;
        MainWindow::mutualUi->ble_char_send(send_str);
    //}
}
//    int i=0, j=0;
//       QList<QNetworkInterface> networkInterface = QNetworkInterface::allInterfaces();
//       for (QList<QNetworkInterface>::const_iterator it = networkInterface.constBegin(); it != networkInterface.constEnd(); ++it)
//       {
//           qDebug() << "[" << i << "] : " << (*it).name();
//           qDebug() << "  " << (*it).humanReadableName();
//           qDebug() << "  " << (*it).hardwareAddress();

//           //获取连接地址列表
//           QList<QNetworkAddressEntry> addressEntriesList = (*it).addressEntries();
//           for (QList<QNetworkAddressEntry>::const_iterator jIt = addressEntriesList.constBegin(); jIt != addressEntriesList.constEnd(); ++jIt)
//           {
//               qDebug() << "\t(" << j << ") :";
//               //输出 ip
//               qDebug() << "\t\tIP : " <<(*jIt).ip().toString();
//               //输出 netmask
//               qDebug() << "\t\tnetmask(子网掩码) : " << (*jIt).netmask().toString();
//               qDebug() << "\t\tBroadcast(广播地址) : "<< (*jIt).broadcast().toString();
//               qDebug() << "";
//               j ++;
//           }
//           i ++;
//       }
