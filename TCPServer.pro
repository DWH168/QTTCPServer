#-------------------------------------------------
#
# Project created by QtCreator 2020-04-21T21:43:33
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TCPServer
TEMPLATE = app


SOURCES += main.cpp\
        tcpserver.cpp

HEADERS  += tcpserver.h

FORMS    += tcpserver.ui
