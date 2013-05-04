#-------------------------------------------------
#
# Project created by QtCreator 2013-05-03T21:56:05
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = ComOven2Test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
INCLUDEPATH += ../src

SOURCES +=  tcpgatewaytest.cpp \
    QtTestUtil/TestRegistry.cpp \
    QtTestUtil/SimpleChecker.cpp \
    ../src/tcpgateway.cpp \
    ../src/clientoven.cpp \
    ../src/utils.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../src/tcpgateway.h \
    ../src/clientoven.h \
    ../src/utils.h \
    QtTestUtil/TestRegistry.h \
    QtTestUtil/TestRegistration.h \
    QtTestUtil/QtTestUtil.h

OTHER_FILES += \
    QtTestUtil/QtTestUtil.pri
