#-------------------------------------------------
#
# Project created by QtCreator 2016-12-08T18:32:24
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = chatroom
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    nfdc.cpp \
    chatroom.cpp \
    chatroombackend.cpp \
    chatmessage.cpp \
    sendrandommsg.cpp

HEADERS += \
    nfdc.h \
    common.h \
    chatroom.h \
    chatroombackend.h \
    chatmessage.h \
    sendrandommsg.h

QMAKE_CXXFLAGS += -std=c++11
LIBS += -lndn-cxx -lChronoSync -lboost_system -lboost_filesystem -lboost_thread -lcryptopp -lsqlite3
