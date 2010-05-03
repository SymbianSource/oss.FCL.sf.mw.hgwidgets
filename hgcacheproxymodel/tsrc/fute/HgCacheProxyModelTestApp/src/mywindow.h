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
#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <hbview.h>
#include <QGraphicsWidget>
#include <QAbstractItemModel>
#include <QtGui>
#include <hbgridviewitem.h>

#include <QtGui>
#include <hbapplication.h>
#include <hblistview.h>
#include <hbicon.h>
#include <hblabel.h>
#include <hbgridview.h>
#include <hbgridviewitem.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <QDirModel>

class QTimer;
class HbMenu;
class HbMainWindow;
class HbGridView;
class HbAction;
class HbMenuItem;
class QFileSystemWatcher;
class HgCacheProxyModel;
class HbSlider;
class MyDataProvider;

class MyWindow : public HbMainWindow
{
    Q_OBJECT

public:
    MyWindow();
    ~MyWindow();

private slots:
    void processAction ( HbAction* action );
    void setIndex(int index);
    void timeout();
    
private:
    HbMenu *createMainMenu();
    
private:
    HbAbstractItemView* mView;      
    HgCacheProxyModel *mModel;
	
	MyDataProvider *mMyDataProvider;
    HbView* mMainView;	
    int mTestVal;
    QTimer *mTimer;

};

#endif // CONTENTWIDGET_H
