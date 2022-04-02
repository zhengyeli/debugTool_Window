#include "myQPushButton.h"

myQPushButton::myQPushButton(QObject *parent) : QToolButton((QWidget *)parent),
    t_PressTimer(nullptr),
    i_PressTimeSave(0),
    b_longpress(false),
    prevButton(nullptr),
    nextButton(nullptr)
{
    t_PressTimer = new QTimer(this);
    connect(t_PressTimer, SIGNAL(timeout()), this, SLOT(countPressMs()));
    connect(this, SIGNAL(pressed()), this, SLOT(longpressed()));
    connect(this, SIGNAL(released()), this, SLOT(longreleased()));
    connect(this, SIGNAL(clicked(bool)), this ,SLOT(myclickfunc(bool)));
}

void myQPushButton::longpressed()
{
    t_PressTimer->start(20);
}

void myQPushButton::longreleased()
{
    t_PressTimer->stop();
    qDebug() << i_PressTimeSave;
    if (500 < i_PressTimeSave && i_PressTimeSave < 1000)
    {
        emit my1slongpress();
        bool ok;
        QString ButtonName = "button";
        QString text = QInputDialog::getText(this, tr("我只是打酱油的~"),tr("input button name"), QLineEdit::Normal,0, &ok);
        if (ok && !text.isEmpty())
        {
            ButtonName = text; //获取输入
            this->setText(ButtonName);
        }
    }
    else if (1000 <= i_PressTimeSave && i_PressTimeSave < 2000)
    {
        emit my2slongpress();
        bool ok;
        QString cmd = "BB0101";
        QString text = QInputDialog::getText(this, tr("我只是打酱油的~"),tr("input CMD + DATA (BB0101)"), QLineEdit::Normal,0, &ok);
        if (ok && !text.isEmpty())
        {
            cmd = text; //获取输入
            this->cmd = cmd;
        }
    }
    else if (2000 <= i_PressTimeSave)
    {
        // NOT WORK NOW
        int ans = QMessageBox::question(NULL, "tip", "delete button ?", QMessageBox::Yes | QMessageBox::No,
                                                      QMessageBox::Yes);
        if (ans == QMessageBox::Yes){

        }
        else if (ans == QMessageBox::No){

        }
    }
    i_PressTimeSave = 1; //clear
}

void myQPushButton::countPressMs()
{
    i_PressTimeSave += 20;
}

void myQPushButton::myclickfunc(bool)
{
    emit myclick(this); //custom slot
    //qDebug() << "i am myclickfunc";
}
