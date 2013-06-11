# This file is part of Syntro
#
# Copyright (c) 2012 Pansenti, LLC. All rights reserved.
#

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += SyntroLCam.h \
	SyntroLCamConsole.h \
	V4LCam.h \
	CamClient.h

SOURCES += main.cpp \
	SyntroLCam.cpp \
	SyntroLCamConsole.cpp \
	V4LCam.cpp \
	CamClient.cpp

FORMS += SyntroLCam.ui

