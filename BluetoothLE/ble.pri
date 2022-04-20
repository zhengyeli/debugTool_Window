QT       += qml quick bluetooth
QT       += network

include($$PWD/api/api.pri)
include($$PWD/form/form.pri)

#实际编译寻找的头路径
INCLUDEPATH += $$PWD/form
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/Window

SOURCES += \
    $$PWD/Window/src/myQPushButton.cpp \
#   $$PWD/Window/bledebugwindow.cpp \
    $$PWD/Window/blelinkwindow.cpp \
    $$PWD/Window/bleuartwindow.cpp \
    $$PWD/Window/tcpsocketclient.cpp \

HEADERS += \
    $$PWD/Window/src/myQPushButton.h \
#   $$PWD/Window/bledebugwindow.h \
    $$PWD/Window/blelinkwindow.h \
    $$PWD/Window/bleuartwindow.h \
    $$PWD/Window/tcpsocketclient.h \


RESOURCES   += $$PWD/source/src.qrc
