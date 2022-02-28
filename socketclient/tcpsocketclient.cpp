#include "tcpsocketclient.h"

#include "mythread.h"

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
    dockSocket->setWindowTitle("tcpSocket窗口");
    dockSocket->setObjectName("tcpSocket窗口");

    QWidget *dockWidgetContents;
    dockWidgetContents = new QWidget(dockSocket);

    ip = new QLineEdit(dockWidgetContents);
    ip->setText("192.168.56.255");
    port = new QLineEdit(dockWidgetContents);
    port->setText("55555");
    port->setMaximumHeight(30); //limit size
    port->setMaximumWidth(80);
    link = new QPushButton(dockWidgetContents);
    link->setText("connect");
    link->setMaximumHeight(30); //limit size
    link->setMaximumWidth(100);
    clear = new QPushButton(dockWidgetContents);
    clear->setText("clear");
    clear->setMaximumHeight(30); //limit size
    clear->setMaximumWidth(100);
    message_box= new QTextEdit(dockWidgetContents);

    QHBoxLayout *HBoxLayout1;
    HBoxLayout1 = new QHBoxLayout();
    HBoxLayout1->setSpacing(10);  //间距
    HBoxLayout1->setContentsMargins(11, 11, 11, 11);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    HBoxLayout1->addWidget(ip);
    HBoxLayout1->addWidget(port);
    HBoxLayout1->addWidget(link);
    HBoxLayout1->addWidget(clear);

    QHBoxLayout *HBoxLayout2;
    HBoxLayout2 = new QHBoxLayout();
    HBoxLayout2->setSpacing(10);  //间距
    HBoxLayout2->setContentsMargins(11, 11, 11, 11);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    HBoxLayout2->addWidget(message_box);

    QVBoxLayout *VBoxLayout;
    VBoxLayout = new QVBoxLayout(dockWidgetContents);
    VBoxLayout->addItem(HBoxLayout1);
    VBoxLayout->addItem(HBoxLayout2);
    //VBoxLayout->addWidget(message_box);
    //gridlayout= new QGridLayout(WidgetContents);

    dockWidgetContents->setLayout(VBoxLayout);
    dockSocket->setWidget(dockWidgetContents);
    MainWindow::mutualUi->creatNewDockWindow(dockSocket, Qt::TopDockWidgetArea,  false);

    connect(link, &QPushButton::clicked, this, [this](bool){
        if (tcp_socket->state() == QAbstractSocket::ConnectedState){
                tcp_socket->disconnectFromHost();
                message_box->append("disconnected");
        }else{
                tcp_socket->connectToHost(ip->text(), port->text().toUInt());
                message_box->append("ip:" + ip->text());
                message_box->append("port:" + port->text());
                message_box->append("connecting");
        }

            });
    connect(clear, &QPushButton::clicked, this, [this](bool){
                message_box->setText("");
            });

    //MyThread *t = new MyThread(); // 创建子线程
    //connect(t,SIGNAL(isDone(bool b[255])), this, SLOT(getresult));
    //t->setObjectName("t");
    //t->start(); // 启动子线程
}

tcpSocketClient::~tcpSocketClient()
{
    delete message_box;
    delete tcp_socket;
    delete ip;
    delete port;
    delete link;
    qDebug() << "~tcpSocketClient";
}

void tcpSocketClient::connected()
{
    qDebug() << "connected";
    message_box->append("connected");
    link->setText("disconnected");
}

void tcpSocketClient::disconnected()
{
    qDebug() << "disconnected";
    message_box->append("disconnected");
    link->setText("connected");
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
