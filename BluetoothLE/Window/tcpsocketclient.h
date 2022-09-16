#ifndef TCPSOCKETCLIENT_H
#define TCPSOCKETCLIENT_H

#include "mainwindow.h"

#include <QWidget>
#include <QLocalSocket>
#include <QTcpSocket>
#include <QIODevice>
#include <QAbstractSocket>
#include <QDockWidget>                //浮动窗口
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QNetworkInterface>

#include <QVBoxLayout>                //垂直布局
#include <QHBoxLayout>                //水平布局
#include <QGridLayout>                //栅格布局

class tcpSocketClient : public QWidget
{
    Q_OBJECT
public:
    friend class MyThread;
    bool checkValidIp(int Adress);
    explicit tcpSocketClient(QWidget *parent = nullptr);
    ~tcpSocketClient();

signals:

private slots:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readyRead();
    void getresult(bool result[255]);
    void closeWindow();

    void clear();
    void setwifi();
    void timerSendWifi();

private:
    QTcpSocket *tcp_socket;
    void write(const char *data);
    void clicked();

    QLineEdit *ip;
    QLineEdit *port;
    QPushButton *link;
    QPushButton *button_clear;
    QTextEdit *message_box;
    int ipNum = 255;
    bool lastip[255] = {false};

    QPushButton *button_set_wifi;
    QLineEdit *text_Ssid,*text_Password;
public:
    QDockWidget *dockSocket;
    static tcpSocketClient *Client;
    QToolButton *toolBtn = nullptr;
};

#endif // TCPSOCKETCLIENT_H