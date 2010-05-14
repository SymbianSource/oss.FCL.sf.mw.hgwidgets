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
TARGET = unittest_hgcacheproxymodel

#fix that later
#CONFIG += symbian_test

INCLUDEPATH += . \
    $$APP_LAYER_SYSTEMINCLUDE \
    $$MW_LAYER_SYSTEMINCLUDE \
	../../inc

TARGET.CAPABILITY = ALL -TCB

DEFINES += _CACHEPROXYDATAMODEL_UNITTEST_LOG_TO_C

# Input
LIBS +=     -lhgcacheproxymodel \
            -lthumbnailmanagerqt

SOURCES += test_hgcacheproxydatamodel.cpp \
			bmhelper.cpp \
			../../src/hgbuffermanager.cpp \
			dataproviderhelper.cpp \
			cacheproxyhelper.cpp

HEADERS += test_hgcacheproxydatamodel.h \
			bmhelper.h \
			../../inc/hgbuffermanager.h \
			dataproviderhelper.h \
			cacheproxyhelper.h

CONFIG += qtestlib console

#RESOURCES += unittest_hgcacheproxymodel.qrc