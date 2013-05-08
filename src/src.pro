#-------------------------------------------------
#
# Project created by QtCreator 2013-05-03T21:55:11
#
#-------------------------------------------------

QT       += core network serialport

QT       -= gui

TARGET = ComOven2
CONFIG   += console debug serialport
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tcpgateway.cpp \
    clientoven.cpp \
    utils.cpp \
    abstractdevice.cpp \
    rs232device.cpp \
    rs232deviceprivate.cpp \
    candevice.cpp

HEADERS += \
    tcpgateway.h \
    clientoven.h \
    utils.h \
    abstractdevice.h \
    rs232device.h \
    rs232deviceprivate.h \
    candevice.h
