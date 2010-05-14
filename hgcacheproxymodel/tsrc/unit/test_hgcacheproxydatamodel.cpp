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
*  Version     : %version: 5 %
*/
#include <QtTest/QtTest>
#include <hgwidgets/hgcacheproxymodel.h>
#include "test_hgcacheproxydatamodel.h"

#include "../../inc/hgbuffermanager.h"
#include "bmhelper.h"

#include "dataproviderhelper.h"
#include "cacheproxyhelper.h"
#include <QtDebug>

// ======== MEMBER FUNCTIONS ========
void TestCacheProxy::testQAbstractItemModel(QAbstractItemModel* model, int rows, int columns)
{
    QVERIFY(model != 0);
    QModelIndex idx = model->index(0, 0);
    if ( rows > 0 && columns > 0){
        QVERIFY(idx.isValid() == true);
        QVERIFY(idx.row() == 0);
        QVERIFY(idx.column() == 0);
        
        idx = model->index(rows/2, 0);
        QVERIFY(idx.row() == rows/2);
        QVERIFY(idx.column() == 0);
        
        idx = model->index(0, columns/2);
        QVERIFY(idx.row() == 0);
        QVERIFY(idx.column() == columns/2);
    
        idx = model->index(-10, 0);
        QVERIFY(idx.row() == -10);
        QVERIFY(idx.column() == 0);
    
        idx = model->index(0, -10);
        QVERIFY(idx.row() == 0);
        QVERIFY(idx.column() == -10);
        
        idx = model->index(rows*2, 0);
        QVERIFY(idx.row() == -1);
        QVERIFY(idx.column() == 0);
    
        idx = model->index(0, columns*2);
        QVERIFY(idx.row() == 0);
        QVERIFY(idx.column() == -1);
    }else{
        QVERIFY(idx.row() == -1);
        QVERIFY(idx.column() == -1);
        
        idx = model->index(-10, 0);
        QVERIFY(idx.row() == -10);
        QVERIFY(idx.column() == -1);

        idx = model->index(0, -10);
        QVERIFY(idx.row() == -1);
        QVERIFY(idx.column() == -10);

        idx = model->index(10, 0);
        QVERIFY(idx.row() == -1);
        QVERIFY(idx.column() == -1);
        
        idx = model->index(0, 10);
        QVERIFY(idx.row() == -1);
        QVERIFY(idx.column() == -1);
    }
    
    idx = model->parent(idx);
    QVERIFY(idx.isValid() == false);
    QVERIFY(model->columnCount() == columns);
    QVERIFY(model->rowCount() == rows);
    
//    QVERIFY(model->hasChildren() == false);
//    QVERIFY(model->headerData(0, Qt::Horizontal).isValid == false);
    
}

void TestCacheProxy::initTestCase()
{
    bm = 0;
    bmh = 0;
    dph = 0;
    cp = 0;   
    cph = 0;
}
    
void TestCacheProxy::cleanupTestCase()
{
    delete bm;
    delete bmh;
    delete dph;   
    delete cp;
    delete cph;
}

void TestCacheProxy::init()
{
    delete bm;
    delete bmh;
    bm = 0;
    bmh = 0;

    delete dph;
    dph = 0;
    
    delete cp;
    cp = 0;
    
    delete cph;
    cph = 0;
}

void TestCacheProxy::cleanup()
{    
    delete bm;
    delete bmh;
    bm = 0;
    bmh = 0;
    
    delete dph;
    dph = 0;

    delete cp;
    cp = 0;
    
    delete cph;
    cph = 0;
}

//BufferManager
void TestCacheProxy::testBM_SetPosition()
{
    int totalsize = 100;
    int bufferSize = 100;
    bmh = new BMHelper(totalsize);
    bm = new HgBufferManager(bmh, bufferSize, 40, 110, totalsize);
    delete bmh;
    delete bm;
    
    totalsize = 100;
    bufferSize = 200;
    bmh = new BMHelper(totalsize);
    bm = new HgBufferManager(bmh, bufferSize, 40, -20, totalsize);
    delete bmh;
    delete bm;

    totalsize = 1000;
    bufferSize = 100;
    bmh = new BMHelper(totalsize);
    bm = new HgBufferManager(bmh, bufferSize, 40, 0, totalsize);
    
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(5);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(23);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(50);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(111);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(120);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(130);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(271);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(507);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(800);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(880);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(899);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(910);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(950);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(996);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(1000);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(1200);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(950);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(501);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(160);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(150);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(140);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(100);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(37);
    QVERIFY(bmh->isIntergal(bufferSize));
   
    bm->setPosition(1);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(-200);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(250);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(-100);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(891);
    QVERIFY(bmh->isIntergal(bufferSize));
}

void TestCacheProxy::testBM_ResetBuffer()
{
    int totalsize = 1000;
    int bufferSize = 100;
    bmh = new BMHelper(totalsize);
    bm = new HgBufferManager(bmh, bufferSize, 40, 0, totalsize);

    bm->resetBuffer(0, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(891);
    bm->resetBuffer(0, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->resetBuffer(880, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(891);
    bm->resetBuffer(880, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(999);
    bm->resetBuffer(0, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(999);
    bm->resetBuffer(995, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(10);
    bm->resetBuffer(910, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->resetBuffer(10, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
}

void TestCacheProxy::testBM_ItemCountChanged()
{
    int totalsize = 1000;
    int bufferSize = 100;
    bmh = new BMHelper(totalsize);
    bm = new HgBufferManager(bmh, bufferSize, 40, 0, totalsize);    
    totalsize--;
    bmh->itemCountChanged(0, true, totalsize);    
    bm->itemCountChanged(0, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));

    totalsize--;
    bmh->itemCountChanged(150, true, totalsize);
    bm->itemCountChanged(150, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));

    totalsize--;
    bm->setPosition(100);
    bmh->itemCountChanged(150, true, totalsize);
    bm->itemCountChanged(150, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));

    totalsize--;
    bm->setPosition(891);
    bmh->itemCountChanged(150, true, totalsize);
    bm->itemCountChanged(150, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize--;
    bm->setPosition(900);
    bmh->itemCountChanged(890, true, totalsize);
    bm->itemCountChanged(890, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize--;
    bm->setPosition(totalsize);
    bmh->itemCountChanged(totalsize, true, totalsize);
    bm->itemCountChanged(totalsize, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize--;
    bm->setPosition(totalsize+200);
    bmh->itemCountChanged(totalsize+200, true, totalsize);
    bm->itemCountChanged(totalsize+200, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    
    totalsize= totalsize - 10;
    bm->setPosition(0);
    bmh->itemCountChanged(0, true, totalsize);
    bm->itemCountChanged(0, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize= totalsize - 10;
    bm->setPosition(totalsize);
    bmh->itemCountChanged(totalsize, true, totalsize);
    bm->itemCountChanged(totalsize, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize= totalsize - 10;
    bm->setPosition(totalsize/2);
    bmh->itemCountChanged(totalsize/2, true, totalsize);
    bm->itemCountChanged(totalsize/2, true, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize++;
    bm->setPosition(0);
    bmh->itemCountChanged(0, false, totalsize);
    bm->itemCountChanged(0, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize++;    
    bmh->itemCountChanged(50, false, totalsize);
    bm->itemCountChanged(50, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize++;    
    bm->setPosition(100);
    bmh->itemCountChanged(80, false, totalsize);
    bm->itemCountChanged(80, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize++;    
    bm->setPosition(500);
    bmh->itemCountChanged(480, false, totalsize);
    bm->itemCountChanged(480, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize++;    
    bm->setPosition(totalsize-20);
    bmh->itemCountChanged(totalsize, false, totalsize);
    bm->itemCountChanged(totalsize, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize=totalsize+10;
    bm->setPosition(totalsize-20);
    bmh->itemCountChanged(totalsize, false, totalsize);
    bm->itemCountChanged(totalsize, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    totalsize=totalsize+10;
    bm->setPosition(20);
    bmh->itemCountChanged(totalsize, false, totalsize);
    bm->itemCountChanged(totalsize, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));

    totalsize=totalsize+10;
    bm->setPosition(totalsize/2-20);
    bmh->itemCountChanged(totalsize, false, totalsize);
    bm->itemCountChanged(totalsize, false, totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));   
}

void TestCacheProxy::testBM_ResizeCache()
{
    int totalsize = 1000;
    int bufferSize = 100;
    bmh = new BMHelper(totalsize);
    bm = new HgBufferManager(bmh, bufferSize, 40, 0, totalsize);
    
    bm->resizeCache(100, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 50;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 150;
    bm->resizeCache(bufferSize, 50);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(30);
    bm->resizeCache(bufferSize, 30);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 50;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 150;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));

    bm->setPosition(500);
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 50;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 151;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(totalsize-20);
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 53;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 150;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bm->setPosition(totalsize);
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 50;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
    
    bufferSize = 175;
    bm->resizeCache(bufferSize, 40);
    QVERIFY(bmh->isIntergal(bufferSize));
}

//DataProvider
void TestCacheProxy::testDP_QAbstractItemModel()
{
    dph = new DataProviderHelper();
    delete dph;
    dph = new DataProviderHelper(200);
    testQAbstractItemModel(dph, 200, 1);
}

void TestCacheProxy::testDP_RequestReleaseAndData()
{
    dph = new DataProviderHelper();
    QModelIndex idx = dph->index(0,0);
    QVariant res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == true);
    QString s = res.toString(); 
    QVERIFY(s.isNull() == false);

    res = dph->data(idx, Qt::UserRole+1); //int value (not less then 0)
    QVERIFY(res.isValid() == true);
    QVERIFY(res.toInt() >= 0);
    
    res = dph->data(idx, Qt::UserRole+100); //should be empty
    QVERIFY(res.isValid() == false);

    res = dph->data(idx, Qt::DecorationRole); //should give defaultIcon();
    QVERIFY(res.isValid() == true);
    s = res.toString(); 
    QVERIFY(s.isNull() == false);
    QVERIFY(res == dph->defaultIcon());

    idx = dph->index(-10, 0);
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == false);

    idx = dph->index(1000000, 0);
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == false);

    QList<int> list;
    list<<-1;
    list<<0;
    list<<1;
    list<<2;
    list<<1000000;
    dph->registerObserver(dph);
    dph->request(list);
    QList< QPair< int, int > > updates = dph->getObserverDataUpdated();
    QVERIFY(updates.count() == 3); //3 updates - for 0 1 and 2
    QVERIFY(updates[0].first == 0); //for 0
    QVERIFY(updates[0].second == 0); //for 0
    QVERIFY(updates[1].first == 1); //for 1
    QVERIFY(updates[1].second == 1); //for 1
    QVERIFY(updates[2].first == 2); //for 2
    QVERIFY(updates[2].second == 2); //for 2
    
    idx = dph->index(0,0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res.isValid() == true);
    s = res.toString(); 
    QVERIFY(s.isNull() == false);
    s = QString("MyNiceIcon %1").arg(0);
    QVERIFY(res.toString() == s);

    idx = dph->index(1,0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res.isValid() == true);
    s = res.toString(); 
    QVERIFY(s.isNull() == false);
    s = QString("MyNiceIcon 1");
    QVERIFY(res.toString() == s);
    
    idx = dph->index(2,0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res.isValid() == true);
    s = res.toString(); 
    QVERIFY(s.isNull() == false);
    s = QString("MyNiceIcon 2");
    QVERIFY(res.toString() == s);
    
    idx = dph->index(3,0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res.isValid() == true);
    s = res.toString(); 
    QVERIFY(s.isNull() == false);
    s = QString("MyNiceIcon 3");
    QVERIFY(res.toString() != s);
    QVERIFY(res == dph->defaultIcon());    
    
    dph->release(list, false);
    updates = dph->getObserverDataUpdated();
    QVERIFY(updates.count() == 0); //0 updates - updates from emit not observer
    updates = dph->getSignalDataUpdated();
    QVERIFY(updates.count() == 1); //1 update for all items
    QVERIFY(updates[0].first == 0); //for 0
    QVERIFY(updates[0].second == 2); //for 0
    
    idx = dph->index(0, 0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res == dph->defaultIcon());    

    idx = dph->index(1, 0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res == dph->defaultIcon());    

    idx = dph->index(2, 0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res == dph->defaultIcon());    
    
    idx = dph->index(3, 0);
    res = dph->data(idx, Qt::DecorationRole);
    QVERIFY(res == dph->defaultIcon());    
    dph->registerObserver(NULL);
    dph->request(list);
    updates = dph->getObserverDataUpdated();
    QVERIFY(updates.count() == 0); //0 updates - no observer
    dph->getSignalDataUpdated();
    dph->release(list, false);
    updates = dph->getObserverDataUpdated();
    QVERIFY(updates.count() == 0); //0 updates - no observer
}

void TestCacheProxy::testDP_CacheManagment()
{
    dph = new DataProviderHelper(100);
    //signals status reset
    dph->getObserverDataUpdated();
    dph->getSignalDataUpdated();
    dph->getSignalRowsAboutToBeInserted();    
    dph->getSignalRowsInserted();
    dph->getSignalRowsAboutToBeRemoved();    
    dph->getSignalRowsRemoved();
    dph->getSignalAboutToBeReset();
    dph->getSignalReset();
    
    int size = dph->rowCount();
    QModelIndex idx;
    QVariant res;
    for ( int i=0; i<size; i++){
        idx = dph->index(i,0);
        res = dph->data(idx, Qt::UserRole+1);
        QVERIFY(res.isValid() == true);
        QVERIFY(res.toInt() == i);   
    }
    idx = dph->index(0,0);
    dph->testClearCache();    
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == false);
    
    QList< QPair< QVariant, int > > list;
    QString s = "NEWITEM";
    list.append( QPair< QVariant, int >(s, Qt::DisplayRole) );
    
    dph->testUpdate(0, &list, false); //update shouldn't change - no item
    dph->testUpdate(-1, &list, true); //update shouldn't change - no item
    dph->testUpdate(120000, &list, false); //update shouldn't change - no item
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == false);
    QVERIFY(dph->getObserverDataUpdated().count() == 0);

    dph->testNewItem(&list, false);
    QList< QPair< int, int > > resList = dph->getSignalRowsAboutToBeInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0); //for item 0
    QVERIFY(resList[0].second == 0); //for item 0
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0); //for item 0
    QVERIFY(resList[0].second == 0); //for item 0
    
    QVERIFY(dph->rowCount() == 1);
    
    idx = dph->index(0,0);
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == true);
    QVERIFY(res == s);
    dph->testNewItem(&list, true);
    QVERIFY(dph->getSignalRowsAboutToBeInserted().count() == 0);//no updates it was suuposed to be silent
    QVERIFY(dph->getSignalRowsInserted().count() == 0);//no updates it was suuposed to be silent    
    QVERIFY(dph->rowCount() == 2);

    dph->testNewItem(&list, false);
    dph->testNewItem(&list, false);
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 2);
    QVERIFY(resList[0].first == 2); //for item 2
    QVERIFY(resList[0].second == 2); //for item 2
    QVERIFY(resList[1].first == 3); //for item 3
    QVERIFY(resList[1].second == 3); //for item 3

    list.clear();
    QString s2 = "NEWITEM2";
    list.append( QPair< QVariant, int >(s2, Qt::DisplayRole) );
    dph->testUpdate(0, &list, false);
    resList = dph->getSignalDataUpdated();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == true);
    QVERIFY(res == s2);
    
    dph->testRemoveItems(0, 1);//should remove item
    resList = dph->getSignalRowsAboutToBeRemoved();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    resList = dph->getSignalRowsRemoved();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    
    QVERIFY(dph->rowCount() == 3);
    dph->testRemoveItems(-2, 10);//should remove remaining items (0,1, 2)
    resList = dph->getSignalRowsAboutToBeRemoved();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 2);

    QVERIFY(dph->rowCount() == 0);
    idx = dph->index(0,0);
    res = dph->data(idx, Qt::DisplayRole); //invalid variant, for not existing item
    QVERIFY(res.isValid() == false);
    
    QVERIFY(dph->getSignalAboutToBeReset() == false);
    QVERIFY(dph->getSignalReset() == false);    
    dph->resetModel();
    QVERIFY(dph->getSignalAboutToBeReset() == true);
    QVERIFY(dph->getSignalReset() == true);
    QVERIFY(dph->columnCount() == 1);
    QVERIFY(dph->rowCount() == 1000);
    idx = dph->index(0,0);
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.toString().startsWith(QString("NEW ITEM")) == true);    
    
    dph->testRemoveItems(0, 100);//should remove items 0 - 99
    QVERIFY(dph->rowCount() == 900);
    idx = dph->index(0,0);
    res = dph->data(idx, Qt::UserRole+1);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.toInt() == 100);
    
    dph->testUpdate(0, QString(""), Qt::DisplayRole, false);
    idx = dph->index(0,0);
    res = dph->data(idx, Qt::DisplayRole);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.toString() == QString(""));    

    resList = dph->getSignalDataUpdated();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    
    dph->testNewItem( QPair< QVariant, int >(QString(""), Qt::DisplayRole), false);
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == dph->rowCount()-1);
    QVERIFY(resList[0].second == dph->rowCount()-1);
    
    dph->getSignalRowsInserted();
    dph->getSignalRowsRemoved();
    dph->testInsertItem(100, QPair< QVariant, int >(QString(""), Qt::DisplayRole), false);
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 100);
    QVERIFY(resList[0].second == 100);
    dph->testRemoveItem(100);
    resList = dph->getSignalRowsRemoved();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 100);
    QVERIFY(resList[0].second == 100); 
    
    dph->getSignalRowsInserted();
    dph->getSignalRowsRemoved();
    dph->testInsertItem(-10, QPair< QVariant, int >(QString(""), Qt::DisplayRole), false); //insert with -10 puts it at begining
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    dph->testRemoveItem(-10);
    resList = dph->getSignalRowsRemoved();
    QVERIFY(resList.count() == 0);

    dph->getSignalRowsInserted();
    dph->getSignalRowsRemoved();
    dph->testInsertItem(100000, QPair< QVariant, int >(QString(""), Qt::DisplayRole), false); //insert with beyond range puts it at end
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == dph->rowCount()-1);
    QVERIFY(resList[0].second == dph->rowCount()-1);    
    dph->testRemoveItem(100000);
    resList = dph->getSignalRowsRemoved();
    QVERIFY(resList.count() == 0);
    
    idx = dph->index(579,0);
    QMap<int, QVariant> it = dph->itemData(idx);
    QCOMPARE(it.size(), 4);
    QCOMPARE(it.contains(Qt::DisplayRole), true);
    QVariant r2 = it[Qt::DisplayRole];
    QCOMPARE(r2.isValid(), true);
    res = dph->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toString(), r2.toString());
    
}

void TestCacheProxy::testDP_QPixmapPool()
{
    dph = new DataProviderHelper(100);
    QPixmap pix;
    QVariant res = dph->testCreateIcon(0, pix);
    QVERIFY(res.isValid() == false);

    dph->resizeQPixmapPool(10);
    
    res = dph->testCreateIcon(0, pix);
    QVERIFY(res.isValid() == true);
    QVERIFY(res != dph->defaultIcon());
    
    QVariant res2 = dph->testCreateIcon(0, pix);
    QVERIFY(res2.isValid() == true);
    QVERIFY(res != dph->defaultIcon());   

    dph->testCreateIcon(0, pix);
    dph->testCreateIcon(1, pix);
    dph->testCreateIcon(2, pix);    
    dph->testCreateIcon(3, pix);
    dph->testCreateIcon(4, pix);
    
    dph->testReleasePixmap(-10);
    dph->testReleasePixmap(4);
    dph->testReleasePixmap(5);
    dph->testReleasePixmap(200);
    dph->resizeQPixmapPool(20);
    dph->resizeQPixmapPool(5);
    dph->testReleasePixmap(100);
    dph->resizeQPixmapPool(100);
    dph->resizeQPixmapPool(2);
    dph->testReleasePixmap(1);
    dph->testReleasePixmap(2);
    dph->testReleasePixmap(3);
    dph->testReleasePixmap(4);
    dph->testCreateIcon(1, pix);
    dph->testCreateIcon(2, pix);
    dph->testCreateIcon(3, pix);
    dph->testCreateIcon(4, pix);
    dph->testCreateIcon(6, pix);
    dph->resizeQPixmapPool(1);
    res = dph->testCreateIcon(10, pix);
    QVERIFY(res.isValid() == false);    
}

void TestCacheProxy::testCP_QAbstractItemModel()
{
    cp = new HgCacheProxyModel();
    dph = new DataProviderHelper(200);
    testQAbstractItemModel(cp, 0, 0);

    cp->setDataProvider(dph);
    QVERIFY(cp->DataProvider() == dph);
    testQAbstractItemModel(cp, 200, 1);
    cp->setDataProvider(0);
    QVERIFY(cp->DataProvider() == 0);

    cp->setDataProvider(dph);
    QVERIFY(cp->DataProvider() == dph);

    cp->setDataProvider(dph);
    QVERIFY(cp->DataProvider() == dph);
}

void TestCacheProxy::testCP_SignalsForward()
{
    cp = new HgCacheProxyModel();
    cph = new CacheProxyHelper(cp);
    dph = new DataProviderHelper(200);    
    cp->setDataProvider(dph, 50, 20);
    
    QList< QPair< int, int > > resList = cph->getSignalDataChanged();
    QVERIFY(resList.count() == 50);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    QVERIFY(resList[49].first == 49);
    QVERIFY(resList[49].second == 49);
    
    QList< QPair< QVariant, int > > list;
    QString s = "NEWITEM";
    list.append( QPair< QVariant, int >(s, Qt::DisplayRole) );

    dph->testNewItem(&list, false);
    resList = cph->getSignalRowsAboutToBeInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == dph->rowCount()-1);
    QVERIFY(resList[0].second == dph->rowCount()-1);
    resList = cph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == dph->rowCount()-1);
    QVERIFY(resList[0].second == dph->rowCount()-1);
    
    dph->testRemoveItems(0, 1);//should remove item
    resList = cph->getSignalRowsAboutToBeRemoved();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    resList = cph->getSignalRowsRemoved();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    
    cph->getSignalModelAboutToBeReset();
    cph->getSignalModelReset();
    dph->resetModel();
    QVERIFY(cph->getSignalModelAboutToBeReset() == true);
    QVERIFY(cph->getSignalModelReset() == true);
    QVERIFY(cp->columnCount() == 1);
    QVERIFY(cp->rowCount() == 1000);

}

void TestCacheProxy::testCP_ResizeCache()
{
    cp = new HgCacheProxyModel();
    cph = new CacheProxyHelper(cp);
    dph = new DataProviderHelper(200);
    cp->setDataProvider(dph, 50, 20);
    dph->getLastRelease();
    dph->getLastRequest();
    cp->resizeCache(20, 5);
    
    QList<int> l = dph->getLastRelease();
    QVERIFY(l.count() > 0);
    l = dph->getLastRequest();
    QVERIFY(l.count() == 0);
    
    dph->getLastRelease();
    dph->getLastRequest();
    cp->resizeCache(100, 5);
    dph->getLastRelease();
    QVERIFY(l.count() == 0);
    l = dph->getLastRequest();
    QVERIFY(l.count() > 0);
}

void TestCacheProxy::testCP_Data()
{
    cp = new HgCacheProxyModel();
    cph = new CacheProxyHelper(cp);
    dph = new DataProviderHelper(200);
    cp->setDataProvider(dph, 50, 20);
    dph->getLastRelease();
    dph->getLastRequest();
    
    QModelIndex idx = cp->index(0, 0);
    QVariant res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), 0);
    
    idx = cp->index(100, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), 100);
    
    idx = cp->index(101, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), 101);

    idx = cp->index(50, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), 50);
    
    idx = cp->index(40, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), 40);
    
    idx = cp->index(35, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), 35);
    
    idx = cp->index(25, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), 25);
    
    int v = 234234;
    res = QVariant(v);
    idx = cp->index(0, 0);
    cp->setData(idx, res, Qt::UserRole+1);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), v);
    
    QMap<int, QVariant> map = cp->itemData(idx);
    QCOMPARE(map.size(), 5);
    map[v]=v;
    cp->setItemData(idx, map);
    res = cp->data(idx, v);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), v);

    
}

void TestCacheProxy::testCP_Sort()
{
    cp = new HgCacheProxyModel();
    cph = new CacheProxyHelper(cp);
    dph = new DataProviderHelper(200);
    cp->setDataProvider(dph, 50, 20);

    cp->setSortRole(Qt::UserRole+2);
    QCOMPARE(cp->sortRole(), Qt::UserRole+2);
    cp->sort(0, Qt::AscendingOrder);
    QCOMPARE(cp->sortColumn(), 0);
    QCOMPARE(cp->sortOrder(), Qt::AscendingOrder);
    
    QModelIndex idx = cp->index(10, 0);
    QVariant res = cp->data(idx, Qt::UserRole+2);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), -(cp->rowCount(idx) -1 -10));
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), cp->rowCount(idx) -1 -10);
    
    idx = cp->index(50, 0);
    res = cp->data(idx, Qt::UserRole+2);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), -(cp->rowCount(idx) -1 -50));
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt(), cp->rowCount(idx) -1 -50);
    
    cp->setSortRole(Qt::DisplayRole);
    cp->setSortCaseSensitivity(Qt::CaseSensitive);
    QCOMPARE(cp->sortCaseSensitivity(), Qt::CaseSensitive);
    cp->sort(0, Qt::AscendingOrder);
    
    idx = cp->index(10, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toInt()%2, 0);
    
    idx = cp->index(110, 0);
    res = cp->data(idx, Qt::UserRole+1);
    QCOMPARE(res.isValid(), true);
//    qWarning("i=%d",res.toInt());
    QCOMPARE(res.toInt()%2, 1);
    
    cp->setSortLocaleAware(true);
    QCOMPARE(cp->isSortLocaleAware(), true);
    cp->setSortLocaleAware(false);
    QCOMPARE(cp->isSortLocaleAware(), false);
        
    QString s = QString("AAA");
    cp->setDynamicSortFilter(true);
    QCOMPARE(cp->dynamicSortFilter(), true);
    cp->setSortRole(Qt::DisplayRole);
    cp->setSortCaseSensitivity(Qt::CaseSensitive);
    dph->testInsertItem(10, QPair< QVariant, int >(s, Qt::DisplayRole), false);
    idx = cp->index(0, 0);
    res = cp->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), true);
//    qWarning()<<res.toString();
    QCOMPARE(res.toString(), s);
    
}

void TestCacheProxy::testCP_Filter()
{
    cp = new HgCacheProxyModel();
    cph = new CacheProxyHelper(cp);
    dph = new DataProviderHelper(200);
    cp->setDataProvider(dph, 50, 20);

    QModelIndex idx = cp->index(1, 0);
    QVariant res = cp->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toString().startsWith("item"), true);
    
    QRegExp regexp = QRegExp("ITEM*", Qt::CaseSensitive, QRegExp::Wildcard);
    
    cp->setFilterRegExp(regexp);
    QVERIFY(cp->filterRegExp() == regexp);
    cp->setFilterCaseSensitivity(Qt::CaseSensitive);
    QCOMPARE(cp->filterCaseSensitivity(), Qt::CaseSensitive);   
    
    idx = cp->index(1, 0);
    res = cp->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toString().startsWith("item"), false);
    
    cp->setFilterKeyColumn(1);
    QCOMPARE(cp->filterKeyColumn (), 1);
    cp->setFilterKeyColumn(0);
    
    cp->setFilterRole(Qt::UserRole+1);
    QCOMPARE(cp->filterRole(), Qt::UserRole+1);
    cp->setFilterRole(Qt::DisplayRole);     
    
    regexp = QRegExp("ITEM*", Qt::CaseInsensitive, QRegExp::Wildcard);
    cp->setFilterRegExp(regexp);
    QVERIFY(cp->filterRegExp() == regexp);
    cp->setFilterCaseSensitivity(Qt::CaseInsensitive);
    QCOMPARE(cp->filterCaseSensitivity(), Qt::CaseInsensitive);   

    idx = cp->index(1, 0);
    res = cp->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toString().startsWith("item"), true);

}


#ifdef _CACHEPROXYDATAMODEL_UNITTEST_LOG_TO_C
    int main (int argc, char* argv[]) 
    {
        QApplication app(argc, argv);
        TestCacheProxy tc;
        int c = 5;
        char* v[] = {argv[0], "-o", "c:/test.txt", "-maxwarnings", "0"};
        return QTest::qExec(&tc, c, v);
    }
#else
    QTEST_MAIN(TestCacheProxy)
#endif
