#ifndef BLECONFIGWIFI_H
#define BLECONFIGWIFI_H

#include <QWidget>
#include "mainwindow.h"
#include <QTimer>
class bleconfigwifi : public QWidget
{
    Q_OBJECT
public:
    explicit bleconfigwifi(QWidget *parent = nullptr);
    void init();

    QDockWidget *dockBleWifi;
private:

    QPushButton *button_set_wifi, *button_clear_wifi;
    QTextEdit *text_Ssid,*text_Password;

public slots:
    void clear();
    void setwifi();
    void timerSendWifi();

signals:

};

#endif // BLECONFIGWIFI_H
