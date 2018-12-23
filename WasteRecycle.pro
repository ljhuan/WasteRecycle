#-------------------------------------------------
#
# Project created by QtCreator 2018-10-06T21:28:36
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += charts
QT       += serialport
QT       += network
RC_ICONS = dragon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ./utils ./inc

TARGET = WasteRecycle
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        wasterecycle.cpp \
    pricesetdialog.cpp \
    sqloper.cpp \
    utilitytools.cpp \
    utils/json/json_internalarray.inl \
    utils/json/json_internalmap.inl \
    utils/json/json_reader.cpp \
    utils/json/json_value.cpp \
    utils/json/json_valueiterator.inl \
    utils/json/json_writer.cpp \
    cameramodel.cpp \
    opennetstream.cpp \

HEADERS += \
        wasterecycle.h \
    pricesetdialog.h \
    sqloper.h \
    utilitytools.h \
    inc/OpenNetStreamDefine.h \
    inc/OpenNetStreamError.h \
    inc/OpenNetStreamInterFace.h \
    utils/json/autolink.h \
    utils/json/config.h \
    utils/json/features.h \
    utils/json/forwards.h \
    utils/json/json.h \
    utils/json/json_batchallocator.h \
    utils/json/reader.h \
    utils/json/value.h \
    utils/json/writer.h \
    cameramodel.h \
    opennetstream.h \
    opennetstream_p.h \

FORMS += \
        wasterecycle.ui \
    pricesetdialog.ui

CONFIG += c++11

DISTFILES +=

RESOURCES += \
    imgs.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/win32/ -lOpenNetStream
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/win32/ -lOpenNetStreamd

INCLUDEPATH += $$PWD/lib/win32
DEPENDPATH += $$PWD/lib/win32
