TARGET       = software
TEMPLATE     = app

QT           += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG       += warn_off
CONFIG       += c++11

DEFINES      += QT_DEPRECATED_WARNINGS


include ($$PWD/ui/form/form.pri)

include ($$PWD/ui/core_common/core_common.pri)

include ($$PWD/BluetoothLE/ble.pri)

INCLUDEPATH  += $$PWD/main.h
HEADERS      += $$PWD/main.h
SOURCES      += $$PWD/main.cpp

RESOURCES    += $$PWD/ui/other/main.qrc
RESOURCES    += $$PWD/ui/core_qss/qss.qrc

TRANSLATIONS += debug_zh_CN.ts

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
