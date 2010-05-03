/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*  Version     : %version: 1 %
*/
#include <QtGui>
#include <hbapplication.h>
#include <hblistview.h>
#include <hbicon.h>
#include <hbgridview.h>
#include <hbgridviewitem.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <QDirModel>

#include "mywindow.h"
#include "hglogger.h"

TX_MAIN(argc, argv) {
    TX_STATIC_ENTRY
    HbApplication app(argc, argv);
    app.setApplicationName("Cache proxy model demo");
    MyWindow mainWindow;
    mainWindow.show();
    // Enter event loop
    int ret = app.exec();   
    TX_STATIC_EXIT
    return ret;
}
