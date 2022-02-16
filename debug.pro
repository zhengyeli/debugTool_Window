TARGET = software
QT       += core gui
QT       += qml quick bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/head

# cpp/ 会项目栏自动添加一个cpp文件夹
SOURCES += \
    cpp/bluetoothbaseclass.cpp \
    cpp/connectionhandler.cpp \
    cpp/devicefinder.cpp \
    cpp/devicehandler.cpp \
    cpp/deviceinfo.cpp \
    cpp/main.cpp \
    cpp/mainwindow.cpp \
    cpp/myQPushButton.cpp

HEADERS += \
    head/bluetoothbaseclass.h \
    head/connectionhandler.h \
    head/devicefinder.h \
    head/devicehandler.h \
    head/deviceinfo.h \
    head/mainwindow.h \
    head/myQPushButton.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    debug_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src.qrc

#QMAKE_POST_LINK = sudo setcap CAP_NET_ADMIN=eip xxx(生成文件名)

