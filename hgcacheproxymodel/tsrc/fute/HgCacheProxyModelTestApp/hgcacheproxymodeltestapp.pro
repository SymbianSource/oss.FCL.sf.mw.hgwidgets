# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = app
CONFIG += qtestlib console hb

TARGET = hgcacheproxymodeltestapp
DEPENDPATH += . \
    inc \
    src
	
symbian { 
	TARGET.EPOCSTACKSIZE = 0x14000
	TARGET.EPOCHEAPSIZE = 0x020000 0x4000000
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
}
LIBS += -lhgcacheproxymodel \
        -lharvesterclient.dll \
        -lmdeclient.dll \ 
		-lthumbnailmanagerqt.dll \
		-lflogger

INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../inc

#DEFINES += ENABLETRACE
#if TRACE_FILENAME not set, then RDebug is used
DEFINES += TRACE_FILE


HEADERS += src\mywindow.h \
    src/mydataprovider.h \
    ../../../inc/txlogger.h
	
SOURCES += src\mywindow.cpp \
    src/mydataprovider.cpp \
    src/main.cpp 
    
RESOURCES += hgcacheproxymodeltestapp.qrc
