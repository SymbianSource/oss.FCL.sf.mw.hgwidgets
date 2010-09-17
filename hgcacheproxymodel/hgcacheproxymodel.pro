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

TEMPLATE = lib
TARGET = hgcacheproxymodel
symbian:TARGET.UID3 = 0x2002EA2A

CONFIG += qt hb
QT -= gui

symbian {
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.CAPABILITY = ALL -TCB
	
	#the def files are located in the root of hgwidgets so this is needed
	defFilePath=..
}

DEFINES += BUILD_CACHEPROXYMODEL
INCLUDEPATH += /sf/mw/qt/src/gui/image/

#DEFINES += ENABLETRACE
#if TRACE_FILE not set, then RDebug is used
#DEFINES += TRACE_FILE

# Input
HEADERS +=  \
	../hgwidgets_plat/hgcacheproxymodel_api/inc/hgcacheproxymodel.h \
	../hgwidgets_plat/hgcacheproxymodel_api/inc/hgdataprovidermodel.h \
	inc/hgbuffermanager.h \
	inc/hglogger.h


SOURCES +=  \
	src/hgcacheproxymodel.cpp \
	src/hgdataprovidermodel.cpp \	
	src/hgbuffermanager.cpp

include(rom/rom.pri)
include(sis/stub.pri)
