#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("这是窗口的标题名字");

    blelinkwindow blelinkwindow;
    w.DockWidgetblelink = blelinkwindow.dockblelink;
    w.DockWidgetblelink->setVisible(true);

    tcpSocketClient SocketClient;
    w.DockWidgetsocket = SocketClient.dockSocket;
    w.DockWidgetsocket->setVisible(false);

    bleUartWindow bleuartwindow;
    w.DockWidgetBleUart = bleuartwindow.dockBleUart;
    w.DockWidgetBleUart->setVisible(false);

    bledebugwindow bledebugwindow;
    w.DockWigetbleDebug = bledebugwindow.dockbledebug;
    w.DockWigetbleDebug->setVisible(false);
    w.text_debug = bledebugwindow.bledebugText;

#ifdef Q_OS_ANDROID
    w.showMaximized();
#else
    w.show();
#endif
    return a.exec();
}

