TARGET = software
TEMPLATE    = app

QT       += core gui
QT       += qml quick bluetooth
QT       += network

#把所有警告都关掉眼不见为净
CONFIG += warn_off
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

INCLUDEPATH += $$PWD/ble
INCLUDEPATH += $$PWD/main
INCLUDEPATH += $$PWD/Window
INCLUDEPATH += $$PWD/ui/form
include ($$PWD/ui/form/form.pri)
INCLUDEPATH += $$PWD/ui/core_common
include ($$PWD/ui/core_common/core_common.pri)

# cpp/ 会项目栏自动添加一个cpp文件夹
SOURCES += \
    Window/src/myQPushButton.cpp \
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

HEADERS += \
    Window/src/myQPushButton.h \
    Window/bledebugwindow.h \
    Window/blelinkwindow.h \
    Window/bleuartwindow.h \
    Window/tcpsocketclient.h \
    ble/bluetoothbaseclass.h \
    ble/connectionhandler.h \
    ble/devicehandler.h \
    ble/devicefinder.h \
    ble/deviceinfo.h \

#FORMS += mainwindow.ui

RESOURCES   += $$PWD/ui/other/main.qrc
RESOURCES   += $$PWD/ui/core_qss/qss.qrc
RESOURCES   += $$PWD/ui/src.qrc

TRANSLATIONS += \
    debug_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target







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

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
