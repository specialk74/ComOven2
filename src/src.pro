#-------------------------------------------------
#
# Project created by QtCreator 2013-05-03T21:55:11
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = src
CONFIG   += console debug
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tcpgateway.cpp \
    clientoven.cpp \
    utils.cpp \
    abstractdevice.cpp

HEADERS += \
    tcpgateway.h \
    clientoven.h \
    utils.h \
    abstractdevice.h
