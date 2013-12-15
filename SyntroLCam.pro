#
#  Copyright (c) 2013 Pansenti, LLC.
#
#  This file is part of Syntro
#

cache()

TEMPLATE = app

TARGET = SyntroLCam

DESTDIR = Output

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug_and_release link_pkgconfig

PKGCONFIG += syntro

LIBS += -lasound

target.path = /usr/bin

INSTALLS += target

DEFINES += QT_NETWORK_LIB

INCLUDEPATH += GeneratedFiles

MOC_DIR += GeneratedFiles/release

OBJECTS_DIR += release  

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(SyntroLCam.pri)

