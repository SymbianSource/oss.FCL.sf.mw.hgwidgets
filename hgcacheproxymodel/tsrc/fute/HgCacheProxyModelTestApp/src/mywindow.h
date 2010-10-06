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
*  Version     : %version: 8 %
*/
#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <HbView>
#include <QGraphicsWidget>
#include <QAbstractItemModel>
#include <QtGui>
#include <HbGridViewItem>

#include <QtGui>
#include <HbApplication>
#include <HbListview>
#include <HbIcon>
#include <HbGridView>
#include <HbGridViewItem>
#include <HbMainWindow>
#include <HbMenu>
#include <QDirModel>

class QTimer;
class HbMenu;
class HgGrid;
class HgMediawall;
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
    void processAction( HbAction* action );
    void sortTestTimeout();
    void filterTestTimeout();
    void resetTestTimeout();
    
private:
    HbMenu *createMainMenu();
    void addChangeViewMenu(HbMenu* parent);
    void addCacheProxyModelMenu(HbMenu* parent);
    void addDataProviderMenu(HbMenu* parent);
    
    
    void scrollTo(int pos);
    
private:   
    HbWidget* mCurrentWidget;
    HbGridView* mGridWidget;
    HbListView* mListWidget;    
    HgMediawall * mMediaWallWidget;
    HgGrid * mHgGridWidget;
    
    HgCacheProxyModel *mModel;
	
	MyDataProvider *mMyDataProvider;
    HbView* mMainView;	
    
    QTimer *mSortTestTimer;
    int mSortTestVal;
    
    QTimer *mFilterTestTimer;
    int mFilterTestVal;

    QTimer *mResetTestTimer;
    int mResetTestVal;
    
    bool mUseStyles;
    bool mEnableFriction;
};

#endif // CONTENTWIDGET_H
