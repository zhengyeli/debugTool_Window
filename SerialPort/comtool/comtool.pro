QT += core gui network serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

RC_FILE      = $$PWD/qrc/comtool_main.rc

HEADERS     += $$PWD/head.h
RESOURCES   += $$PWD/qrc/comtool_main.qrc
CONFIG      += warn_off

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/api
INCLUDEPATH += $$PWD/form

include ($$PWD/api/api.pri)
include ($$PWD/form/form.pri)

INCLUDEPATH += $$PWD/../3rd_qextserialport
include ($$PWD/../3rd_qextserialport/3rd_qextserialport.pri)