#include "bleconfigwifi.h"

#ifdef Q_OS_WIN

#else

#endif

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

bleconfigwifi::bleconfigwifi(QWidget *parent)
    : QWidget{parent}
{
    init();
}

void bleconfigwifi::init()
{
    QWidget *WidgetContents = nullptr;
    QGridLayout *gridlayout = nullptr;
    dockBleWifi = new QDockWidget(this);
    dockBleWifi->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dockBleWifi->setWindowTitle("wifi");
    dockBleWifi->setObjectName("config wifi窗口");
    //dockBleUart->setFloating(true);

    WidgetContents = new QWidget(dockBleWifi);
    gridlayout     = new QGridLayout(WidgetContents);

    text_Ssid = new QTextEdit(WidgetContents);
    text_Ssid->setText("Govee");
    text_Ssid->setMinimumHeight(30);
    text_Ssid->setMaximumHeight(30);

    text_Password = new QTextEdit(WidgetContents);
    text_Password->setText("starstarlight");
    text_Password->setMinimumHeight(30);
    text_Password->setMaximumHeight(30);

    button_clear_wifi = new QPushButton(WidgetContents);
    button_clear_wifi->setMaximumHeight(20); //limit size
    button_clear_wifi->setMinimumWidth(80);
    button_clear_wifi->setText("clear all");

    button_set_wifi = new QPushButton(WidgetContents);
    button_set_wifi->setMaximumHeight(20); //limit size
    button_set_wifi->setMinimumWidth(80);
    button_set_wifi->setText("config wifi now");

    gridlayout->addWidget(text_Ssid,0,0);
    gridlayout->addWidget(button_clear_wifi,0,1);
    gridlayout->addWidget(text_Password,1,0);
    gridlayout->addWidget(button_set_wifi,1,1);

    connect(button_clear_wifi , SIGNAL(clicked(bool)), this ,SLOT(clear()));
    connect(button_set_wifi, SIGNAL(clicked(bool)), this ,SLOT(setwifi()));

    MainWindow::mutualUi->creatNewDockWindow(dockBleWifi, Qt::TopDockWidgetArea,  false);
    dockBleWifi->setWidget(WidgetContents);
    dockBleWifi->setVisible(false);

    QToolButton *toolBtn = new QToolButton(this);              //创建QToolButton
    toolBtn->setText(dockBleWifi->windowTitle());
    MainWindow::mutualUi->toolbar->connect(toolBtn, &QToolButton::clicked, this, &bleconfigwifi::closeWindow);
    MainWindow::mutualUi->toolbar->addWidget(toolBtn);                               //向工具栏添加QToolButton按钮
}


void bleconfigwifi::closeWindow()
{
    MainWindow::mutualUi->closeAllWindow();
    if (dockBleWifi->isVisible()){
        dockBleWifi->setVisible(false);
    }else{
        dockBleWifi->setVisible(true);
    }
}

void bleconfigwifi::clear()
{
    text_Ssid->clear();
    text_Password->clear();
}

void bleconfigwifi::setwifi()
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

void bleconfigwifi::timerSendWifi()
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
