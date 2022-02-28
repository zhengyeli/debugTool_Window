#include "bledebugwindow.h"

QTextEdit *bledebugwindow::bledebugText = nullptr;

bledebugwindow::bledebugwindow(QWidget *parent)
    : QWidget{parent}
{
    dockbledebug = new QDockWidget(this);
    QWidget *dockWidgetContents = new QWidget(dockbledebug);

    //给浮动窗口添加文本编辑区
    bledebugText = new QTextEdit(dockWidgetContents);
    bledebugText->setReadOnly(true);
    button_blelog_send = new QPushButton(dockWidgetContents);
    button_blelog_send->setText("log send");
    button_clear = new QPushButton(dockWidgetContents);
    button_clear->setText("clear");

    text_blelog_send = new QTextEdit(dockWidgetContents);
    text_blelog_send->setText("govee");
    text_blelog_send->setMinimumHeight(30);
    text_blelog_send->setMaximumHeight(30);

    QHBoxLayout  *HBoxLayout;             //栅格布局
    HBoxLayout = new QHBoxLayout();
    HBoxLayout->addWidget(text_blelog_send);
    HBoxLayout->addWidget(button_blelog_send);
    HBoxLayout->addWidget(button_clear);

    QVBoxLayout *VBoxLayout;
    VBoxLayout = new QVBoxLayout(dockWidgetContents);
    VBoxLayout->addItem(HBoxLayout);
    VBoxLayout->addWidget(bledebugText);

    dockbledebug->setWidget(dockWidgetContents);
    dockbledebug->setObjectName("无线蓝牙打印窗口");
    dockbledebug->setWindowTitle("无线蓝牙打印窗口");
    dockbledebug->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    //dockbledebug->setVisible(false);

    connect(button_blelog_send,       SIGNAL(clicked(bool)),           this,SLOT(blelog_send()));
    connect(button_clear,       SIGNAL(clicked(bool)),                 this,SLOT(clear()));
    MainWindow::mutualUi->creatNewDockWindow(dockbledebug,Qt::TopDockWidgetArea, false);
}

void bledebugwindow::blelog_send()
{
    QString data = text_blelog_send->toPlainText();
    QByteArray array = data.toUtf8(); //"aa11"
    MainWindow::mutualUi->ble_send(array);
}

void bledebugwindow::clear()
{
    bledebugText->clear();
}

void bledebugwindow::debugtext_append(QString str)
{
    bledebugText->append(str);
}

