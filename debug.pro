TARGET = software
QT       += core gui
QT       += qml quick bluetooth
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/ble
INCLUDEPATH += $$PWD/main

# cpp/ 会项目栏自动添加一个cpp文件夹
SOURCES += \
    Window/src/myQPushButton.cpp \
    Window/bleconfigwifi.cpp \
    Window/bledebugwindow.cpp \
    Window/blelinkwindow.cpp \
    Window/bleuartwindow.cpp \
    Window/tcpsocketclient.cpp \
    ble/bluetoothbaseclass.cpp \
    ble/connectionhandler.cpp \
    ble/devicehandler.cpp \
    ble/devicefinder.cpp \
    ble/deviceinfo.cpp \
    main/main.cpp \
    main/mainwindow.cpp \

HEADERS += \
    Window/src/myQPushButton.h \
    Window/bleconfigwifi.h \
    Window/bledebugwindow.h \
    Window/blelinkwindow.h \
    Window/bleuartwindow.h \
    Window/tcpsocketclient.h \
    ble/bluetoothbaseclass.h \
    ble/connectionhandler.h \
    ble/devicehandler.h \
    ble/devicefinder.h \
    ble/deviceinfo.h \
    main/mainwindow.h \

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    debug_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ui/src.qrc



# build depend on dev
win32 {

    contains(QT_ARCH, i386) {
        message("32-bit")
        #LIBS += ......
    }else {
        message("64-bit")
        #LIBS += ......
    }
}

unix {
#QMAKE_POST_LINK = sudo setcap CAP_NET_ADMIN=eip software
        message("unix")
}

macx {
        message("macx")

}
