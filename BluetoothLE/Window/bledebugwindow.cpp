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
    button_blelog_send->setText("send");

    button_clear = new QPushButton(dockWidgetContents);
    button_clear->setText("clear");

    text_blelog_send = new QLineEdit(dockWidgetContents);
    text_blelog_send->setText("govee");
    text_blelog_send->setMaximumSize(100, 25);

    QHBoxLayout  *HBoxLayout;             //栅格布局
    HBoxLayout = new QHBoxLayout();
    HBoxLayout->setContentsMargins(5, 5, 5, 5);  //setMargin可以设置左、上、右、下的外边距，设置之后，该函数可以主动设置
    HBoxLayout->setSpacing(20);  //间距
    HBoxLayout->addWidget(text_blelog_send);
    HBoxLayout->addWidget(button_blelog_send);
    HBoxLayout->addWidget(button_clear);

    QVBoxLayout *VBoxLayout;
    VBoxLayout = new QVBoxLayout(dockWidgetContents);
    VBoxLayout->addItem(HBoxLayout);
    VBoxLayout->addWidget(bledebugText);

    dockbledebug->setWidget(dockWidgetContents);
    dockbledebug->setObjectName("无线调试窗口");
    dockbledebug->setWindowTitle("调试");
    dockbledebug->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockbledebug->setVisible(false);

    connect(button_blelog_send,       SIGNAL(clicked(bool)),           this,SLOT(blelog_send()));
    connect(button_clear,       SIGNAL(clicked(bool)),                 this,SLOT(clear()));
    MainWindow::mutualUi->creatNewDockWindow(dockbledebug,Qt::TopDockWidgetArea, false);

    toolBtn = new QToolButton(this);              //创建QToolButton
    toolBtn->setText(dockbledebug->windowTitle());
    MainWindow::mutualUi->toolbar->connect(toolBtn, &QToolButton::clicked, this, &bledebugwindow::closeWindow);
    MainWindow::mutualUi->toolbar->addWidget(toolBtn);                               //向工具栏添加QToolButton按钮
}


void bledebugwindow::closeWindow()
{
    MainWindow::mutualUi->closeAllWindow();
    if (dockbledebug->isVisible()){
        dockbledebug->setVisible(false);
    }else{
        dockbledebug->setVisible(true);
    }
    toolBtn->setChecked(true);
}

void bledebugwindow::blelog_send()
{
    QString data = text_blelog_send->text();
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

