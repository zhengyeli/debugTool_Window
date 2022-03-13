#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtCore/QCoreApplication>
#include <QThread>
#include <QDebug>
#include <tcpsocketclient.h>
#include <QObject>

class MyThread : public QThread
{
    Q_OBJECT // emit need this and #include <QObject>, why?

public:
    explicit MyThread(QObject* parent = nullptr);

protected:
    void run() override; // 线程入口函数 override;

    tcpSocketClient tcp;

signals:
    void isDone(bool b[255]);

public slots:
    void finish();
};

#endif // MYTHREAD_H
