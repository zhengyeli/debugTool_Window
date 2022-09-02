#ifndef BLELINKWINDOW_H
#define BLELINKWINDOW_H

#include <QWidget>

#include "mainwindow.h"

class blelinkwindow : public QWidget
{
    Q_OBJECT
public:
    explicit blelinkwindow(QWidget *parent = nullptr);
    ~blelinkwindow();
    void init();
    static void receive(QString);

public slots:
    void addBleDevToList(const QBluetoothDeviceInfo& info);
    void bleConnectSuccess();
    void scanButton_clicked();
    void sendButton_clicked();
    void bleDevlist_itemClicked(QListWidgetItem* i);
    void disconButton_clicked();
    void pauseButton_clicked();
    void continueButton_clicked();
    void clearButton_clicked();
    void closeWindow();
    void keepalive();
    void ble_get_base_info();

private:
    QPushButton * button_scan_sku,*button_ble_send ,*button_stop,*button_continue ,*button_discon, *button_clear;
    QLineEdit *cmd_send;
    static QLineEdit *cmd_receive;
    QLineEdit   *text_sku,*text_ble_send;
    QTextEdit   *infowindow;
    QToolButton *toolBtn = nullptr;

 public:
    QDockWidget *dockblelink;
    static QListWidget *sku_list;


signals:

};

#endif // BLELINKWINDOW_H
