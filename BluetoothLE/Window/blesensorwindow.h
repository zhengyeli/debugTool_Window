#ifndef BLESENSORWINDOW_H
#define BLESENSORWINDOW_H

#include <QWidget>
#include <QDockWidget>                //浮动窗口
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include <QVBoxLayout>                //垂直布局
#include <QHBoxLayout>                //水平布局
#include <QGridLayout>                //栅格布局

namespace Ui {
class bleSensorWindow;
}

class bleSensorWindow : public QWidget
{
    Q_OBJECT

public:
    explicit bleSensorWindow(QWidget *parent = nullptr);
    ~bleSensorWindow();
    QDockWidget *dockBleSensor;
    QToolButton *toolBtn = nullptr;
    void sensor_blemsg_handle(QByteArray array);

private:
    Ui::bleSensorWindow *ui;
    void init();
    void closeWindow();

private slots:
    void comboxChange(int index);
    void send();
    void stop();
    void start();
    void save();
    void refactory();
    // void reboot();
};

#endif // BLESENSORWINDOW_H
