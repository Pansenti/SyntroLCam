# This file is part of Syntro
#
# Copyright (c) 2012 Pansenti, LLC. All rights reserved.
#

TEMPLATE = app
TARGET = SyntroLCam

DESTDIR = Output

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug_and_release link_pkgconfig

PKGCONFIG += syntro

unix {
	target.path = /usr/bin
	INSTALLS += target
}

INCLUDEPATH += GeneratedFiles

MOC_DIR += GeneratedFiles/release

OBJECTS_DIR += release  

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(SyntroLCam.pri)

