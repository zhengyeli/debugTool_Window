#ifndef BLELINKWINDOW_H
#define BLELINKWINDOW_H

#include <QWidget>

#include "mainwindow.h"

class blelinkwindow : public QWidget
{
    Q_OBJECT
public:
    explicit blelinkwindow(QWidget *parent = nullptr);
    void init();
    static void addBleDevToList(QString);
    static void receive(QString);

private slots:
    void scanButton_clicked();
    void sendButton_clicked();
    void bleDevlist_itemClicked(QListWidgetItem* i);
    void disconButton_clicked();
    void pauseButton_clicked();
    void continueButton_clicked();
    void clearButton_clicked();
    void closeWindow();

private:
    QPushButton * button_scan_sku,*button_ble_send ,*button_stop,*button_continue ,*button_discon, *button_clear;
    QLabel *cmd_send;
    static QLabel *cmd_receive;
    QTextEdit *text_sku,*text_ble_send, *infowindow;

 public:
    QDockWidget *dockblelink;
    static QListWidget *sku_list;

signals:

};

#endif // BLELINKWINDOW_H
