#-------------------------------------------------
#
# Project created by QtCreator 2012-12-28T21:05:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sixnet_tool
TEMPLATE = app

OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR=build

DESTDIR = bin

SOURCES += main.cpp\
        mainwindow.cpp \
    sixnetmessage.cpp \
    sixnetmessagehandler.cpp

HEADERS  += mainwindow.h \
    sixnetmessage.h \
    sixnetmessagehandler.h

FORMS    += mainwindow.ui
