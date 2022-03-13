#include "mythread.h"
bool lastip[255] = {false};

// not work now
MyThread::MyThread(QObject* parent)
{
    //connect(this, SIGNAL(finished), this, SLOT(finish));
}

void MyThread::run()
{
    int i = 1;
    for (;i < 5;i++){
        qDebug() << "i";
        if (tcp.checkValidIp(i)){
            lastip[i] = true;
            qDebug() << i;
        }
    }
    emit isDone(lastip);
    //exec();
}

void MyThread::finish()
{
    emit isDone(lastip);
}
