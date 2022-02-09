#ifndef MYQPUSHBUTTON_H
#define MYQPUSHBUTTON_H

#include <QPushButton>                //浮动窗口
#include <QTimer>
#include <QWidget>
#include <QMessageBox>
#include <QInputDialog>

class myQPushButton : public QPushButton
{
    Q_OBJECT
    //Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)

public:
    explicit myQPushButton(QObject *parent = nullptr);
    //~myQPushButton();

signals:
    void longPressDown();
    void longPressuP();
    void myclick(myQPushButton *temp);
    void my1slongpress();
    void my2slongpress();

public slots:
    void longpressed();
    void longreleased();
    void countPressMs();
    void myclickfunc(bool);

private:
    QTimer *t_PressTimer;
    int i_PressTimeSave;
    bool b_longpress;

public:
    myQPushButton *prevButton; //prev button
    myQPushButton *nextButton; //next button
    QString name;
    QString cmd;
};


#endif // MYQPUSHBUTTON_H
