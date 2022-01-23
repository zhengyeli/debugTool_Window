#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("这是窗口的标题名字");
    w.show();
    return a.exec();
}

