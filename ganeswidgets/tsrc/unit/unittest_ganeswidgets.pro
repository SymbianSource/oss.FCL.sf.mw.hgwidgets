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
CONFIG += qtestlib \
    hb
TARGET = unittest_ganeswidgets
DEFINES += _UNITTEST_GANESWIDGETS_LOG_TO_C_
TARGET.CAPABILITY = All \
    -TCB
INCLUDEPATH += . \
    inc \
    $$APP_LAYER_SYSTEMINCLUDE \
    $$MW_LAYER_SYSTEMINCLUDE \
    /sf/mw/hb/src/hbcore/gui
LIBS += -lestor.dll \
    -lganeswidgets.dll

# Input
SOURCES += hbautotest.cpp \
    unittest_ganeswidgets.cpp
RESOURCES = unittest_ganeswidgets.qrc
HEADERS = 
