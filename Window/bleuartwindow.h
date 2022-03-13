#ifndef BLEUARTWINDOW_H
#define BLEUARTWINDOW_H
#include "mainwindow.h"
#include <QWidget>
#include "src/myQPushButton.h"

#include <QDockWidget>                //浮动窗口
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include <QVBoxLayout>                //垂直布局
#include <QHBoxLayout>                //水平布局
#include <QGridLayout>                //栅格布局

class bleUartWindow : public QWidget
{
    Q_OBJECT
public:
    explicit bleUartWindow(QWidget *parent = nullptr);

private:
    void init();

private slots:
    void addButton();
    void bleSendData(myQPushButton* temp);
    void saveFile();
    void loadFile(bool b);
    void closeWindow();

public:
    QDockWidget *dockBleUart;

private:
    QPushButton *button_add_bleUart, *button_load_bleUart, *button_save_bleUart;

signals:

};

#endif // BLEUARTWINDOW_H
