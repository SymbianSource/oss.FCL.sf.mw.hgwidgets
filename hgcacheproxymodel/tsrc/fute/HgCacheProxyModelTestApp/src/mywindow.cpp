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
#include "mywindow.h"
#include <hbmenu.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbslider.h>
#include <QTimer>
#include <QtGui>
#include <QDebug>
#include <QPushButton>
#include <QApplication>
#include <QGraphicsLinearLayout>
#include <QCoreApplication>
#include <hbgridview.h>
#include <hbinstance.h>
#include <QFileSystemWatcher>
#include <hgwidgets/hgcacheproxymodel.h>

#include "mydataprovider.h"
#include "hglogger.h"


const QStringList KViewNames = (QStringList() << "GridView" << "ListView" );

MyWindow::MyWindow()
    : HbMainWindow(), 
    mView(0),
    mModel(0),
    mMyDataProvider(0),
    mTimer(new QTimer(this))
{
    mMainView = new HbView();
    addView( mMainView );

    mMainView->setMenu( createMainMenu() ); 
    
    mMyDataProvider = new MyDataProvider(this);
    mModel = new HgCacheProxyModel(this);
    mModel->setDataProvider(mMyDataProvider, 120, 20);

    connect(mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    
    HbAction action;
	action.setData ( QVariant(1) );	//select Grid
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

    HbMenu *viewSubMenu = mainMenu->addMenu("Change view");
    QString temporaryString;
    int i(0);
    foreach (temporaryString , KViewNames) {
        HbAction* subAction = viewSubMenu->addAction(temporaryString);
        subAction->setData (QVariant(++i));
    }
    
    HbMenu *sortSubMenu = mainMenu->addMenu("Sort");
    HbAction* action = sortSubMenu->addAction("Sort1");
    action->setData(QVariant(101));
    action = sortSubMenu->addAction("Sort2");
    action->setData(QVariant(102));    
    action = sortSubMenu->addAction("Sort3");
    action->setData(QVariant(103));
    HbMenu *sort4SubMenu = sortSubMenu->addMenu("Sort4 (String)");
    action = sort4SubMenu->addAction("CaseSensitive");
    action->setData(QVariant(104));
    action = sort4SubMenu->addAction("CaseInsensitive");
    action->setData(QVariant(105));
    
    HbMenu *filterSubMenu = mainMenu->addMenu("Filter");
    action = filterSubMenu->addAction("Disable filter");
    action->setData(QVariant(201));
    action = filterSubMenu->addAction("Filter ITEM* (Case Sensitive)");
    action->setData(QVariant(202));
    action = filterSubMenu->addAction("Filter ITEM1*(Case Insensitive)");
    action->setData(QVariant(203));
    
    action = mainMenu->addAction("Reset");
    action->setData(QVariant(1000));
    action = mainMenu->addAction("Enable dynamic Sort/Filter");
    action->setData(QVariant(1001));    

    action = mainMenu->addAction("Test");
    action->setData(QVariant(1002));

    action = mainMenu->addAction("Stop Test");
    action->setData(QVariant(1003));
    
    return mainMenu;
}


void MyWindow::processAction( HbAction* action )
{
    int command = action->data().toInt();
    if ( command == 1) {
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
		//this could be done to change mode, for images it doesn't make sence, becouse images thumbnails are created by default only for medium and fullsize
//		AbstractDataProvider *data = mModel->DataProvider();
//		MyDataProvider *prov = static_cast<MyDataProvider*>(data); //of course we have already poiter to that - mMyDataProvider, but it is example how to get it from model
//		prov->changeIconSize(ThumbnailManager::ThumbnailMedium);
		view->setModel(mModel);
		mMainView->setWidget( view );
        mView = view;
    } else if (command == 2) {
        HbListView* view = new HbListView();
		view->setUniformItemSizes( true );
		view->setItemRecycling( true );
		//this could be done to change mode, for images it doesn't make sence, becouse images thumbnails are created by default only for medium and fullsize
//		AbstractDataProvider *data = mModel->DataProvider();
//		MyDataProvider *prov = static_cast<MyDataProvider*>(data); //of course we have already poiter to that - mMyDataProvider, but it is example how to get it from model
//		prov->changeIconSize(ThumbnailManager::ThumbnailSmall);
		view->setModel(mModel);
		mMainView->setWidget( view );
        mView = view;
	} else if (command == 101) { //sort by KSort1Role
        mModel->setSortRole(Qt::UserRole+2);
        mModel->sort(0);
	} else if (command == 102) { //sort by KSort2Role
        mModel->setSortRole(Qt::UserRole+3);
        mModel->sort(0);
    } else if (command == 103) { //sort by KSort3Role
        mModel->setSortRole(Qt::UserRole+4);
        mModel->sort(0);
    } else if (command == 104) { //sort by DisplayRole CaseSensitive
        mModel->setSortRole(Qt::DisplayRole);
        mModel->setSortCaseSensitivity(Qt::CaseSensitive);  
        mModel->sort(0);
    } else if (command == 105) { //sort by DisplayRole CaseInsensitive
        mModel->setSortRole(Qt::DisplayRole);
        mModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        mModel->sort(0);
    } else if (command == 201) { //disable filter
        QRegExp reg = QRegExp();
        mModel->setFilterRegExp(reg);
    } else if (command == 202) { //Filter ITEM* (Case Sensitive)
        QRegExp reg = QRegExp("ITEM*", Qt::CaseSensitive, QRegExp::Wildcard);
        mModel->setFilterCaseSensitivity(Qt::CaseSensitive);
        mModel->setFilterRegExp(reg);        
    } else if (command == 203) { //Filter ITEM1*(Case Insensitive)
        QRegExp reg = QRegExp("ITEM1*", Qt::CaseInsensitive, QRegExp::Wildcard);
        mModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        mModel->setFilterRegExp(reg);
    } else if (command == 1000) { //reset
        mMyDataProvider->resetModel();
    } else if (command == 1001) { //reset
        bool current = !mModel->dynamicSortFilter();
        mModel->setDynamicSortFilter(current);
        if (current){
            action->setText("Disable dynamic Sort/Filter");
        }else{
            action->setText("Enable dynamic Sort/Filter");            
        }
    } else if (command == 1002) {
        mTestVal = 500;
        timeout();
    } else if (command == 1003) {
        mTestVal = -1;
    }
}

void MyWindow::setIndex(int /*index*/)
{
    update();
}

void MyWindow::timeout()
{   
    HbAction action;
    action.setData ( QVariant(101 + mTestVal%4) );
    processAction(&action);

    
    if (mTestVal>=0){
        mTestVal--;
        if (mTestVal==0){
            mTestVal = 500;
        }
        mTimer->start(2000);
    }
}

//eof
