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

symbian:TARGET.UID3 = 0x10207C66
TEMPLATE = lib
CONFIG += hb
TARGET = ganeswidgets
DEFINES += BUILD_HG_WIDGETS
TARGET.CAPABILITY = All \
    -TCB
DEPENDPATH += .
INCLUDEPATH += . \
    inc \
    $$MW_LAYER_SYSTEMINCLUDE \
    /sf/mw/hb/src/hbcore/gui
LIBS += -lestor.dll \
    -lhbcore \
    -lhbwidgets \
    -llibOpenVG.dll \
    -llibOpenVGU.dll
symbian { 
    TARGET.EPOCALLOWDLLDATA = 1
}

# Input
SOURCES += src/hgwidgets.cpp \
    src/hgwidgets_p.cpp \
    src/HgContainer.cpp \
    src/hgwidgetitem.cpp \
    src/HgScrollBufferManager.cpp \
    src/hgquad.cpp \
    src/hgquadrenderer.cpp \
    src/hgvgquadrenderer.cpp \
    src/hgvgimage.cpp \
    src/hgmediawallrenderer.cpp \
    src/hgspring.cpp \
    src/hgdrag.cpp \
    src/HgImageFader.cpp \
    src/hglongpressvisualizer.cpp \
    src/hgcoverflowcontainer.cpp \
    src/hggridcontainer.cpp \
#    src/hgindexfeedback.cpp \
#    src/hgindexfeedback_p.cpp \
    src/hggrid.cpp \
    src/hggrid_p.cpp \
    src/hgmediawall.cpp \
    src/hgmediawall_p.cpp \
    src/hgqtquadrenderer.cpp \
    src/hgtransformedquad.cpp \
    src/hgtransformedquadrenderer.cpp \
    src/hgcenteritemarea.cpp
HEADERS = inc/trace.h \
    ../hgwidgets_plat/ganeswidgets_api/inc/hgwidgets.h \
    ../hgwidgets_plat/ganeswidgets_api/inc/hggrid.h \
    ../hgwidgets_plat/ganeswidgets_api/inc/hgmediawall.h \
    inc/hgwidgets_p.h \
    inc/hggrid_p.h \
    inc/hgmediawall_p.h \
    inc/hgmediawallrenderer.h \
    inc/hgcontainer.h \
    inc/hgspring.h \
    inc/hgdrag.h \
    inc/hgscrollbuffermanager.h \
    inc/hgimagefader.h \
    inc/hglongpressvisualizer.h \
    inc/hgcoverflowcontainer.h \
    inc/hggridcontainer.h \
#    inc/hgindexfeedback.h \
    inc/hgqtquadrenderer.h \
    inc/hgtransformedquad.h \
    inc/hgtransformedquadrenderer.h \
    inc/hgcenteritemarea.h
RESOURCES += data/ganeswidgets.qrc

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

include(rom/rom.pri)
include(sis/stub.pri)