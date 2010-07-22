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
#include "mywindow.h"
#include <HbMenu>
#include <HbAction>
#include <HbMainWindow>
#include <HbSlider>
#include <QTimer>
#include <QtGui>
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include <QGraphicsLinearLayout>
#include <QCoreApplication>
#include <HbGridView>
#include <HbInstance>
#include <HbInputDialog>
#include <QFileSystemWatcher>
#include <hgwidgets/hgcacheproxymodel.h>
#include <hgwidgets/hgmediawall.h>
#include <hgwidgets/hggrid.h>

#include "mydataprovider.h"
#include "hglogger.h"

const int KGridViewCommand = 1;
const int KListViewCommand = 2;
const int KMediaWallViewCommand = 3;
const int KHgGridViewCommand = 4;

const int KSort1Command = 1101;
const int KSort2Command = 1102;
const int KSort3Command = 1103;
const int KSortCaseSensitiveCommand = 1104;
const int KSortCaseInsensitiveCommand = 1105;
const int KDisableFilterCommand = 1200;
const int KFilter1Command = 1201;
const int KFilter2Command = 1202;
const int KTogleDynamicCommand = 1000;
const int KTogleSortTestCommand = 1001;
const int KTogleFilterTestCommand = 1002;

const int KInsert0Command = 2100;
const int KInsert50Command = 2101;
const int KInsert101Command = 2102;
const int KInsert257Command = 2103;
const int KInsert1000Command = 2104;
const int KInsertEndCommand = 2105;
const int KInsertEnd10Command = 2106;
const int KInsertCustomCommand = 2107;
const int KRemove0Command = 2200;
const int KRemove50Command = 2201;
const int KRemove101Command = 2202;
const int KRemove257Command = 2203;
const int KRemove1000Command = 2204;
const int KRemoveLastCommand = 2205;
const int KRemoveEnd10Command = 2206;
const int KRemoveCustomCommand = 2207;
const int KShowImagesCommand = 2300;
const int KShowAudioCommand = 2301;
const int KHbIconModeCommand = 2400;
const int KQIconModeCommand = 2401;
const int KQImageModeCommand = 2402;
const int KQPixmapModeCommand = 2403;

const int KResetCommand = 10000;

MyWindow::MyWindow()
    : HbMainWindow(), 
    mView(0),
    mModel(0),
    mMyDataProvider(0),
    mSortTestTimer(new QTimer(this)),
    mSortTestVal(-1),
    mFilterTestTimer(new QTimer(this)),
    mFilterTestVal(-1)
{
    mMainView = new HbView();
    addView( mMainView );

    mMainView->setMenu( createMainMenu() ); 
    
    mMyDataProvider = new MyDataProvider(this);
    mModel = new HgCacheProxyModel(this);
    mModel->setDataProvider(mMyDataProvider, 120, 20);

    connect(mSortTestTimer, SIGNAL(timeout()), this, SLOT(sortTestTimeout()));
    connect(mSortTestTimer, SIGNAL(timeout()), this, SLOT(filterTestTimeout()));
    
    HbAction action;
	action.setData ( QVariant(KGridViewCommand) );	//select Grid
	processAction(&action);
}

MyWindow::~MyWindow()
{
//	delete mMyDataProvider;
}

HbMenu *MyWindow::createMainMenu()
{
    HbMenu* mainMenu = new HbMenu( );
    connect(mainMenu, SIGNAL(triggered(HbAction*)),this, SLOT(processAction(HbAction*)));

    addChangeViewMenu(mainMenu);
    addCacheProxyModelMenu(mainMenu);
    addDataProviderMenu(mainMenu);
    
    HbAction* action = mainMenu->addAction("Reset");
    action->setData(QVariant(KResetCommand));

    return mainMenu;
}

void MyWindow::addChangeViewMenu(HbMenu* parent)
{
    HbMenu *viewSubMenu = parent->addMenu("Change view");
    HbAction* action = viewSubMenu->addAction("Orbit Grid View");
    action->setData(QVariant(KGridViewCommand));
    action = viewSubMenu->addAction("Orbit List View");
    action->setData(QVariant(KListViewCommand));
    action = viewSubMenu->addAction("Hg MediaWall");
    action->setData(QVariant(KMediaWallViewCommand));
    action = viewSubMenu->addAction("Hg Grid");
    action->setData(QVariant(KHgGridViewCommand));
    
    
}

void MyWindow::addCacheProxyModelMenu(HbMenu* parent)
{
    
    HbMenu *cpSubMenu = parent->addMenu("CacheProxyModel");
    
    HbMenu *sortSubMenu = cpSubMenu->addMenu("Sort");
    HbAction* action = sortSubMenu->addAction("Sort1");
    action->setData(QVariant(KSort1Command));
    action = sortSubMenu->addAction("Sort2");
    action->setData(QVariant(KSort2Command));    
    action = sortSubMenu->addAction("Sort3");
    action->setData(QVariant(KSort3Command));
    HbMenu *sort4SubMenu = sortSubMenu->addMenu("Sort4 (String)");
    action = sort4SubMenu->addAction("CaseSensitive");
    action->setData(QVariant(KSortCaseSensitiveCommand));
    action = sort4SubMenu->addAction("CaseInsensitive");
    action->setData(QVariant(KSortCaseInsensitiveCommand));
    
    HbMenu *filterSubMenu = cpSubMenu->addMenu("Filter");
    action = filterSubMenu->addAction("Disable filter");
    action->setData(QVariant(KDisableFilterCommand));
    action = filterSubMenu->addAction("Filter ITEM* (Case Sensitive)");
    action->setData(QVariant(KFilter1Command));
    action = filterSubMenu->addAction("Filter ITEM1*(Case Insensitive)");
    action->setData(QVariant(KFilter2Command));
    
    action = cpSubMenu->addAction("Enable dynamic Sort/Filter");
    action->setData(QVariant(KTogleDynamicCommand));
    
    action = cpSubMenu->addAction("Start Sort Test");
    action->setData(QVariant(KTogleSortTestCommand));

    action = cpSubMenu->addAction("Start Filter Test");
    action->setData(QVariant(KTogleFilterTestCommand));

    
}

void MyWindow::addDataProviderMenu(HbMenu* parent)
{
    HbMenu *dpSubMenu = parent->addMenu("DataProvider");
    
    HbMenu *insertSubMenu = dpSubMenu->addMenu("Insert Item");
    HbAction* action = insertSubMenu->addAction("at 0");
    action->setData(QVariant(KInsert0Command));
    action = insertSubMenu->addAction("at 50");
    action->setData(QVariant(KInsert50Command));    
    action = insertSubMenu->addAction("at 101");
    action->setData(QVariant(KInsert101Command));
    action = insertSubMenu->addAction("at 257");
    action->setData(QVariant(KInsert257Command));
    action = insertSubMenu->addAction("at 1000");
    action->setData(QVariant(KInsert1000Command));
    action = insertSubMenu->addAction("at end");
    action->setData(QVariant(KInsertEndCommand));
    action = insertSubMenu->addAction("at end-10");
    action->setData(QVariant(KInsertEnd10Command));
    action = insertSubMenu->addAction("at ...");
    action->setData(QVariant(KInsertCustomCommand));
    
    HbMenu *removeSubMenu = dpSubMenu->addMenu("Remove Item");
    action = removeSubMenu->addAction("at 0");
    action->setData(QVariant(KRemove0Command));
    action = removeSubMenu->addAction("at 50");
    action->setData(QVariant(KRemove50Command));    
    action = removeSubMenu->addAction("at 101");
    action->setData(QVariant(KRemove101Command));
    action = removeSubMenu->addAction("at 257");
    action->setData(QVariant(KRemove257Command));
    action = removeSubMenu->addAction("at 1000");
    action->setData(QVariant(KRemove1000Command));
    action = removeSubMenu->addAction("last");
    action->setData(QVariant(KRemoveLastCommand));
    action = removeSubMenu->addAction("at end-10");
    action->setData(QVariant(KRemoveEnd10Command));
    action = removeSubMenu->addAction("at ...");
    action->setData(QVariant(KRemoveCustomCommand));

    action = dpSubMenu->addAction("Show Images");
    action->setData(QVariant(KShowImagesCommand));
    action = dpSubMenu->addAction("Show Audio");
    action->setData(QVariant(KShowAudioCommand));

    action = dpSubMenu->addAction("HbIcon Mode");
    action->setData(QVariant(KHbIconModeCommand));
    action = dpSubMenu->addAction("QIcon Mode");
    action->setData(QVariant(KQIconModeCommand));
    action = dpSubMenu->addAction("QImage Mode");
    action->setData(QVariant(KQImageModeCommand));
    action = dpSubMenu->addAction("QPixmap Mode");
    action->setData(QVariant(KQPixmapModeCommand));
    
}



void MyWindow::processAction( HbAction* action )
{
    int command = action->data().toInt();
    
    switch (command){
        case KGridViewCommand : {
            HbGridView* view = new HbGridView();
            if ( orientation() == Qt::Horizontal ) {
                view->setColumnCount( 5 );
                view->setRowCount( 3 );
            }else {
                view->setColumnCount( 3 );
                view->setRowCount( 5 );         
            }
    //        view->setTextVisible(false);
            view->setUniformItemSizes( true );
            view->setItemRecycling( true );
            view->setModel(mModel);
            mMainView->setWidget( view );
            mView = view;
            break;
        }
        case KListViewCommand : {
            HbListView* view = new HbListView();
            view->setUniformItemSizes( true );
            view->setItemRecycling( true );
            view->setModel(mModel);
            mMainView->setWidget( view );
            mView = view;            
            break;
        }
        case KMediaWallViewCommand : {
            HgMediawall * view = new HgMediawall();
            view->setModel(mModel);
            mMainView->setWidget( view );
            mView = view;
            break;
        }
        case KHgGridViewCommand : {
        HgGrid * view = new HgGrid(Qt::Vertical);
            view->setModel(mModel);
            mMainView->setWidget( view );
            mView = view;
            break;
        }
        case KSort1Command : {
            mModel->setSortRole(Qt::UserRole+2);
            mModel->sort(0);
            break;
        }
        case KSort2Command : {
            mModel->setSortRole(Qt::UserRole+3);
            mModel->sort(0);
            break;
        }
        case KSort3Command : {
            mModel->setSortRole(Qt::UserRole+4);
            mModel->sort(0);
            break;
        }
        case KSortCaseSensitiveCommand : {
            mModel->setSortRole(Qt::DisplayRole);
            mModel->setSortCaseSensitivity(Qt::CaseSensitive);  
            mModel->sort(0);
            break;
        }
        case KSortCaseInsensitiveCommand : {
            mModel->setSortRole(Qt::DisplayRole);
            mModel->setSortCaseSensitivity(Qt::CaseInsensitive);
            mModel->sort(0);
            break;
        }
        case KDisableFilterCommand : {
            QRegExp reg = QRegExp();
            mModel->setFilterRegExp(reg);
            break;
        }
        case KFilter1Command : {
            QRegExp reg = QRegExp("ITEM*", Qt::CaseSensitive, QRegExp::Wildcard);
            mModel->setFilterCaseSensitivity(Qt::CaseSensitive);
            mModel->setFilterRegExp(reg);        
            break;
        }
        case KFilter2Command : {
            QRegExp reg = QRegExp("ITEM1*", Qt::CaseInsensitive, QRegExp::Wildcard);
            mModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            mModel->setFilterRegExp(reg);
            break;
        }
        case KTogleDynamicCommand : {
            bool current = !mModel->dynamicSortFilter();
            mModel->setDynamicSortFilter(current);
            if (current){
                action->setText("Disable dynamic Sort/Filter");
            }else{
                action->setText("Enable dynamic Sort/Filter");            
            }
            break;
        }
        case KTogleSortTestCommand : {
            if (mSortTestVal == -1){
                mSortTestVal = 500;
                action->setText("Stop Sort Test");            
                sortTestTimeout();
            }else{
                mSortTestVal = -1;
                action->setText("Start Sort Test");            
            }
            break;
        }
        case KTogleFilterTestCommand : {
            if (mFilterTestVal == -1){
                mFilterTestVal = 500;
                action->setText("Stop Filter Test");            
                sortTestTimeout();
            }else{
                mFilterTestVal = -1;
                action->setText("Start Filter Test");            
            }
            break;
        }
        case KInsert0Command : {
            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
            mMyDataProvider->testInsertItem(0, data);
            delete data;
            break;
        }
        case KInsert50Command : {
            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
            mMyDataProvider->testInsertItem(50, data);
            delete data;
            break;
        }
        case KInsert101Command : {
            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
            mMyDataProvider->testInsertItem(101, data);
            delete data;
            break;
        }
        case KInsert257Command : {
            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
            mMyDataProvider->testInsertItem(257, data);
            delete data;
            break;
        }
        case KInsert1000Command : {
            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
            mMyDataProvider->testInsertItem(1000, data);
            delete data;
            break;
        }
        case KInsertEndCommand : {
            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
            mMyDataProvider->testInsertItem(mMyDataProvider->rowCount(QModelIndex()), data);
            delete data;
            break;
        }
        case KInsertEnd10Command : {
            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
            mMyDataProvider->testInsertItem(mMyDataProvider->rowCount(QModelIndex())-10, data);
            delete data;
            break;
        }
        case KInsertCustomCommand : {
		//TODO fix me
//            int val = HbInputDialog::getInteger( QString("Insert at position:"));
//            QList< QPair< QVariant, int > >* data = new QList< QPair< QVariant, int > >();
//            data->append( QPair< QVariant, int >(QString("NEW ITEM!!!!"), Qt::DisplayRole) );
//            mMyDataProvider->testInsertItem(val, data);
            break;
        }
        case KRemove0Command : {
            mMyDataProvider->testRemoveItem(0);
            break;
        }
        case KRemove50Command : {
            mMyDataProvider->testRemoveItem(50);
            break;
        }
        case KRemove101Command : {
            mMyDataProvider->testRemoveItem(101);
            break;
        }
        case KRemove257Command : {
            mMyDataProvider->testRemoveItem(257);
            break;
        }
        case KRemove1000Command : {
        mMyDataProvider->testRemoveItem(1000);            
            break;
        }
        case KRemoveLastCommand : {
                mMyDataProvider->testRemoveItem(mMyDataProvider->rowCount(QModelIndex())-1);
            break;
        }
        case KRemoveEnd10Command : {
            mMyDataProvider->testRemoveItem(mMyDataProvider->rowCount(QModelIndex()) - 10);
            break;
        }
        case KRemoveCustomCommand : {
		//TODO fix me		
//            int val = HbInputDialog::getInteger( QString("Remove from position:"));
//            mMyDataProvider->testRemoveItem(val);
            break;
        }
        case KShowImagesCommand : {
            mMyDataProvider->changeIconSize(ThumbnailManager::ThumbnailMedium);
            mMyDataProvider->changeMode(0);
            break;
        }
        case KShowAudioCommand : {
            mMyDataProvider->changeIconSize(ThumbnailManager::ThumbnailSmall);
            mMyDataProvider->changeMode(1);
            break;
        }
        
        case KHbIconModeCommand : {
            mMyDataProvider->setIconMode(HgDataProviderModel::HgDataProviderIconHbIcon);
            break;
        }
        case KQIconModeCommand : {
            mMyDataProvider->setIconMode(HgDataProviderModel::HgDataProviderIconQIcon);
            break;
        }
            case KQImageModeCommand : {
            mMyDataProvider->setIconMode(HgDataProviderModel::HgDataProviderIconQImage);
            break;
        }
            case KQPixmapModeCommand : {
            mMyDataProvider->setIconMode(HgDataProviderModel::HgDataProviderIconQPixmap);
            break;
        }
        case KResetCommand : {
            mMyDataProvider->resetModel();
            break;
        }
        default: {
            break;
        }
    }
}

void MyWindow::sortTestTimeout()
{   
    if (mSortTestVal>0){
        HbAction action;
        action.setData ( QVariant(KSort1Command + mSortTestVal%(KSortCaseInsensitiveCommand - KSort1Command + 1) ) );
        processAction(&action);
        mSortTestVal--;
        if (mSortTestVal==0){
            mSortTestVal = 500;
        }
        mSortTestTimer->start(2000);
    }
}

void MyWindow::filterTestTimeout()
{
    if (mFilterTestVal>0){
        HbAction action;
        action.setData ( QVariant(KDisableFilterCommand + mFilterTestVal%(KFilter2Command - KDisableFilterCommand + 1 ) ) );
        processAction(&action);
        mFilterTestVal--;
        if (mFilterTestVal==0){
            mFilterTestVal = 500;
        }
        mFilterTestTimer->start(2000);
    }
}

//eof
