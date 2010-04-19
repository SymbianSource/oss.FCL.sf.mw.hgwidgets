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
CONFIG += hb
TARGET = hgwidgettest
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE $$APP_LAYER_SYSTEMINCLUDE
TARGET.CAPABILITY = All -TCB

INCLUDEPATH += /sf/mw/hb/src/hbwidgets/itemviews /sf/mw/hb/src/hbcore/gui

symbian {
    TARGET.EPOCHEAPSIZE = 0x10000 0x2000000
    BLD_INF_RULES.prj_exports += "rom/hgwidgettest.iby CORE_APP_LAYER_IBY_EXPORT_PATH(hgwidgettest.iby)" \
                                 "rom/hgwidgettestresource.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(hgwidgettestresource.iby)"
}

SOURCES += src/main.cpp \
	src/hgwidgettestdatamodel.cpp \
	src/hgwidgettestalbumartmanager.cpp \
	src/hgwidgettestview.cpp \
	src/hgtestview.cpp \
	src/hgselectiondialog.cpp \
	src/hgflipwidget.cpp
HEADERS += inc/hgwidgettestdatamodel.h \
	inc/hgwidgettestalbumartmanager.h \
	inc/hgwidgettestview.h \
	inc/hgtestview.h \
	inc/hgselectiondialog.h \
	inc/hgflipwidget.h
LIBS += -lganeswidgets.dll \
    -lthumbnailmanagerqt.dll

RESOURCES += data/hgwidgettest.qrc

