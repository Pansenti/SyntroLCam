#
#  Copyright (c) 2013 Pansenti, LLC.
#
#  This file is part of Syntro
#

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += SyntroLCam.h \
        SyntroLCamConsole.h \
	VideoDriver.h \
	CamClient.h \	
	AudioDriver.h \	
	StreamsDlg.h \
        CameraDlg.h \
        MotionDlg.h \
        AudioDlg.h

SOURCES += main.cpp \
        SyntroLCam.cpp \
        SyntroLCamConsole.cpp \
	VideoDriver.cpp \
	CamClient.cpp \
  	AudioDriver.cpp \
	StreamsDlg.cpp \
        CameraDlg.cpp \
        MotionDlg.cpp \
        AudioDlg.cpp

FORMS += SyntroLCam.ui

