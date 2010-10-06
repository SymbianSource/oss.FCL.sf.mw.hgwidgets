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
*  Version     : %version: 11 %
*/
#include <QtTest/QtTest>
#include <hgwidgets/hgcacheproxymodel.h>
#include "test_hgcacheproxydatamodel.h"

#include "../../inc/hgbuffermanager.h"
#include "bmhelper.h"

#include "dataproviderhelper.h"
#include "cacheproxyhelper.h"
#include <QtDebug>
#include <HbIcon>
#include <QIcon>
#include <QImage>
#include <QPixmap>


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
    bm = new HgBufferManager(bmh, bufferSize, 40, 2*bufferSize, totalsize);    
    bufferSize = 300;
    bm->resizeCache(bufferSize, 20);
    bm->setPosition(5);
    QVERIFY(bmh->isIntergal(bufferSize));
    bm->setPosition(5000);
    QVERIFY(bmh->isIntergal(bufferSize));
    delete bmh;
    delete bm;

    bmh = new BMHelper(totalsize);
    bm = new HgBufferManager(bmh, bufferSize, 40, -10, totalsize);
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
    bm->aboutToRemoveItem(0);
    bmh->remove(0);
    bm->removedItem(0);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize--;
    QCOMPARE(bmh->totalSize(), totalsize);

    bm->aboutToRemoveItem(150);
    bmh->remove(150);
    bm->removedItem(150);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize--;
    QCOMPARE(bmh->totalSize(), totalsize);

    bm->setPosition(100);
    bm->aboutToRemoveItem(150);
    bmh->remove(150);
    bm->removedItem(150);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize--;
    QCOMPARE(bmh->totalSize(), totalsize);

    bm->setPosition(891);
    bm->aboutToRemoveItem(150);
    bmh->remove(150);
    bm->removedItem(150);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize--;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(0);
    bm->aboutToRemoveItem(150);
    bmh->remove(150);
    bm->removedItem(150);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize--;
    QCOMPARE(bmh->totalSize(), totalsize);    
    
    bm->setPosition(900);
    bm->aboutToRemoveItem(890);
    bmh->remove(890);
    bm->removedItem(890);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize--;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(totalsize);
    bm->aboutToRemoveItem(totalsize);
    bmh->remove(totalsize);
    bm->removedItem(totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
//    totalsize--;//no item remove since totalsize is outside
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(totalsize);
    bm->aboutToRemoveItem(totalsize-1);
    bmh->remove(totalsize-1);
    bm->removedItem(totalsize-1);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize--;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(totalsize+200);
    bm->aboutToRemoveItem(totalsize+200);
    bmh->remove(totalsize+200);
    bm->removedItem(totalsize+200);
    QVERIFY(bmh->isIntergal(bufferSize));
//    totalsize--; //no item remove since totalsize+200 is outside
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->aboutToRemoveItem(-20);
    bmh->remove(-20);
    bm->removedItem(-20);
    QVERIFY(bmh->isIntergal(bufferSize));
//    totalsize--; //no item remove since totalsize+200 is outside
    QCOMPARE(bmh->totalSize(), totalsize);
    
    
    bm->aboutToRemoveItem(totalsize+10);
    bmh->remove(totalsize+10);
    bm->removedItem(totalsize+10);
    QVERIFY(bmh->isIntergal(bufferSize));
//    totalsize--; //no item remove since totalsize+200 is outside
    QCOMPARE(bmh->totalSize(), totalsize);

    
    bm->resetBuffer(0, 10);
    bmh->resizeCache(10);
    bm->aboutToRemoveItem(1);
    bmh->remove(1);
    bm->removedItem(1);
    QCOMPARE(bmh->totalSize(), 9);

    bmh->resizeCache(totalsize);
    bm->resetBuffer(0, totalsize);
    
    //insert
    
    bm->setPosition(0);
    bm->aboutToInsertItem(0);
    bmh->insert(0); 
    bm->insertedItem(0);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize++;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->aboutToInsertItem(50);
    bmh->insert(50);  
    bm->insertedItem(50);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize++;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(100);
    bm->aboutToInsertItem(80);
    bmh->insert(80);   
    bm->insertedItem(80);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize++;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(500);
    bm->aboutToInsertItem(80);
    bmh->insert(80);   
    bm->insertedItem(80);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize++;
    QCOMPARE(bmh->totalSize(), totalsize);    
    
    bm->setPosition(500);
    bm->aboutToInsertItem(480);
    bmh->insert(480); 
    bm->insertedItem(480);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize++;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(totalsize-20);
    bm->aboutToInsertItem(totalsize);
    bmh->insert(totalsize); 
    bm->insertedItem(totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
    totalsize++;
    QCOMPARE(bmh->totalSize(), totalsize);
    
    bm->setPosition(totalsize-20);
    bm->aboutToInsertItem(totalsize+20);
    bmh->insert(totalsize+20); 
    bm->insertedItem(totalsize+20);
    QVERIFY(bmh->isIntergal(bufferSize));
//    totalsize++; //no insert
    QCOMPARE(bmh->totalSize(), totalsize);

    bm->setPosition(2*totalsize);
    bm->aboutToInsertItem(2*totalsize);
    bmh->insert(2*totalsize); 
    bm->insertedItem(2*totalsize);
    QVERIFY(bmh->isIntergal(bufferSize));
//    totalsize++; //no insert
    QCOMPARE(bmh->totalSize(), totalsize);

    
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
    
    dph->testClearItem(2, false);
    updates = dph->getSignalDataUpdated();
    QVERIFY(updates.count() == 1); //1 update
    QVERIFY(updates[0].first == 2); //for 2
    QVERIFY(updates[0].second == 2); //for 2
    QVERIFY(dph->itemData(idx).count() == 0 ); //for 2
    
    QVERIFY(dph->itemData( dph->testCreateIndex(999999, 0)).count() == 0 );

    dph->testClearItem(2, true);
    updates = dph->getSignalDataUpdated();
    QVERIFY(updates.count() == 0); //no updates
    
    dph->testClearItem(-10, true);
    updates = dph->getSignalDataUpdated();
    QVERIFY(updates.count() == 0); //no updates

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
    dph->testUpdate(-1, s, Qt::DisplayRole, true); //update shouldn't change - no item
    dph->testUpdateIcon(-1, s);
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

    dph->testUpdateIcon(0, s, false);
    dph->testUpdateIcon(0, s, true);
    res = dph->data(idx, Qt::DecorationRole);
    QCOMPARE(res.isValid(), true);
    QVERIFY(res == s);
    QCOMPARE(dph->getSignalDataUpdated().count(), 1);
    
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
    dph->testInsertItem(0, QPair< QVariant, int >(QString(""), Qt::DisplayRole), false); //insert with 0 puts at begining
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);    
    QVERIFY(resList[0].first == 0);
    QVERIFY(resList[0].second == 0);
    
    dph->getSignalRowsInserted();
    dph->getSignalRowsRemoved();
    dph->testInsertItem(-10, QPair< QVariant, int >(QString(""), Qt::DisplayRole), false); //insert with -10 not adds to collection
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 0);
    dph->testRemoveItem(-10);
    resList = dph->getSignalRowsRemoved();
    QVERIFY(resList.count() == 0);
    
    dph->getSignalRowsInserted();
    dph->getSignalRowsRemoved();
    dph->testInsertItem(dph->rowCount(), QPair< QVariant, int >(QString(""), Qt::DisplayRole), false); //insert at dph->rowCount() puts at end
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 1);
    QVERIFY(resList[0].first == dph->rowCount()-1);
    QVERIFY(resList[0].second == dph->rowCount()-1);
    
    dph->getSignalRowsInserted();
    dph->getSignalRowsRemoved();
    dph->testInsertItem(100000, QPair< QVariant, int >(QString(""), Qt::DisplayRole), false); //insert with beyond range not adds to collection
    resList = dph->getSignalRowsInserted();
    QVERIFY(resList.count() == 0);
    dph->testRemoveItem(100000);
    resList = dph->getSignalRowsRemoved();
    QVERIFY(resList.count() == 0);
    
    idx = dph->index(579,0);
    QMap<int, QVariant> it = dph->itemData(idx);
    QCOMPARE(it.size(), 4);
    QCOMPARE(it.contains(Qt::DisplayRole), true);
    QVariant r2 = it[Qt::DisplayRole];
    QCOMPARE(r2.isValid(), true);

    idx = dph->index(-10,0);
    it = dph->itemData(idx);
    
    res = dph->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), false);

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
    
    dph->resizeQPixmapPool(110);
    for ( int i = 0; i < 100; i ++)
        dph->testCreateIcon(i, pix);
    
    for ( int i = 20; i < 40; i ++)
        dph->testReleasePixmap(i);
    
    dph->resizeQPixmapPool(50);
    dph->testReleasePixmap(5);
    
    dph->resizeQPixmapPool(1);
    dph->testReleasePixmap(0);
    dph->testReleasePixmap(2);
    dph->testReleasePixmap(1);
    dph->resizeQPixmapPool(10);

    
    
}

void TestCacheProxy::testDP_IconMode()
{
    dph = new DataProviderHelper(100);
    QCOMPARE( dph->testIconMode(), HgDataProviderModel::HgDataProviderIconHbIcon);
    dph->resizeQPixmapPool(10);
    QPixmap pix;
    QVariant res = dph->testCreateIcon(0, pix);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.canConvert<HbIcon>());
    
    dph->testChangeIconMode(HgDataProviderModel::HgDataProviderIconHbIcon);
    res = dph->testCreateIcon(0, pix);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.canConvert<HbIcon>());
    
    dph->testChangeIconMode(HgDataProviderModel::HgDataProviderIconQIcon);
    res = dph->testCreateIcon(0, pix);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.canConvert<QIcon>());
    
    dph->testChangeIconMode(HgDataProviderModel::HgDataProviderIconQImage);
    res = dph->testCreateIcon(0, pix);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.canConvert<QImage>());
    
    dph->testChangeIconMode(HgDataProviderModel::HgDataProviderIconQPixmap);
    res = dph->testCreateIcon(0, pix);
    QVERIFY(res.isValid() == true);
    QVERIFY(res.canConvert<QPixmap>());
    
    dph->testChangeIconMode( (HgDataProviderModel::HgDataProviderIconMode)10 );
    res = dph->testCreateIcon(0, pix);
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
    
    QList< QPair< QVariant, int > > list;
    QString s = "NEWITEM";
    list.append( QPair< QVariant, int >(s, Qt::DisplayRole) );
    dph->testNewItem(&list, false);
    
    dph->testRemoveItems(0, 1);//should remove item

    QModelIndex idx = cp->index(0,0);
    QCOMPARE(cp->hasChildren(idx), true);
    QSize s1 = cp->span(idx);
    QSize s2 = cp->span(cp->index(1, 0));
    QVERIFY(s1 == s2);
    cp->submit();
    cp->revert();
    Qt::ItemFlags f1 = cp->flags(idx);
    Qt::ItemFlags f2 = cp->flags(cp->index(1, 0));
    QVERIFY(f1 == f2);
    
    QVariant res = cp->headerData(0, Qt::Horizontal, Qt::DecorationRole);
    QCOMPARE(res.isValid(), false);
    //default implementation ignores that
    cp->setHeaderData(0, Qt::Horizontal, QString("headerdata"), Qt::DecorationRole);
    res = cp->headerData(0, Qt::Horizontal, Qt::DecorationRole);
    QCOMPARE(res.isValid(), false);
    
    QStringList li = cp->mimeTypes();
    QCOMPARE(li.count(), 1);
    
    QModelIndexList li2;
    li2<< idx;
    QMimeData* md1 = cp->mimeData(li2);
    QVERIFY(md1 != NULL);
    QCOMPARE(cp->dropMimeData(NULL, Qt::MoveAction, 10, 0, QModelIndex() ), false);    
    QCOMPARE(cp->supportedDragActions(), Qt::CopyAction);

    QCOMPARE(cp->canFetchMore(idx), false);
    cp->fetchMore(idx);
    
    QModelIndex idx2 = cp->buddy(idx);
    QCOMPARE(idx2.row(), idx.row());
    QCOMPARE(idx2.column(), idx.column());
    
    li2 = cp->match(idx, Qt::DisplayRole, QString("ITEM"), 1000, Qt::MatchContains);
    QCOMPARE(li2.count(), dph->getCount());

    
    
    cp->setDataProvider(NULL);
    QVERIFY(cp->DataProvider() == NULL);
    idx = cp->index(100,0);
    res = dph->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), false);
    
    QCOMPARE(cp->insertRows(0, 10), false); //default implementation returns false
    QCOMPARE(cp->removeRows(0, 10), false);//default implementation returns false

    QCOMPARE(cp->insertRow(0), false); //default implementation returns false
    QCOMPARE(cp->removeRow(0), false);//default implementation returns false
    
    QCOMPARE(cp->insertColumns(0, 10), false); //default implementation returns false
    QCOMPARE(cp->removeColumns(0, 10), false);//default implementation returns false

    QCOMPARE(cp->insertColumn(0), false); //default implementation returns false
    QCOMPARE(cp->removeColumn(0), false);//default implementation returns false
    
    
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
    
    cph->getSignalDataChanged();
    dph->testEmitDataChanged(dph->index(0,0), dph->index(10,0));
    resList = cph->getSignalDataChanged();
    QCOMPARE(resList.count(), 1);
    QCOMPARE(resList[0].first, 0);
    QCOMPARE(resList[0].second, 10);

    cph->getSignalColumnsAboutToBeInserted();
    cph->getSignalColumnsInserted();
    dph->testEmitColumnsInsert(dph->index(0,0),0, 10);
    resList = cph->getSignalColumnsAboutToBeInserted();
    QCOMPARE(resList.count(), 1);
    QCOMPARE(resList[0].first, 0);
    QCOMPARE(resList[0].second, 10);
    resList = cph->getSignalColumnsInserted();
    QCOMPARE(resList.count(), 1);
    QCOMPARE(resList[0].first, 0);
    QCOMPARE(resList[0].second, 10);

    cph->getSignalColumnsAboutToBeRemoved();
    cph->getSignalColumnsRemoved();
    dph->testEmitColumnsRemove(dph->index(0,0),0, 10);
    resList = cph->getSignalColumnsAboutToBeRemoved();
    QCOMPARE(resList.count(), 1);
    QCOMPARE(resList[0].first, 0);
    QCOMPARE(resList[0].second, 10);
    resList = cph->getSignalColumnsRemoved();
    QCOMPARE(resList.count(), 1);
    QCOMPARE(resList[0].first, 0);
    QCOMPARE(resList[0].second, 10);
    
//    cph->getSignalColumnsAboutToBeMoved();
//    cph->getSignalColumnsMoved();
//    QCOMPARE(dph->testEmitColumnsAboutToBeMoved(0, 10), false);
//    resList = cph->getSignalColumnsAboutToBeMoved();
//    QCOMPARE(resList.count(), 0);
    
//    dph->testEmitColumnsMoved(0, 10);
//    resList = cph->getSignalColumnsMoved();
//    QCOMPARE(resList.count(), 0);
    
    
//    cph->getSignalRowsAboutToBeMoved();
//    cph->getSignalRowsMoved();
//    QCOMPARE(dph->testEmitRowsAboutToBeMoved(0, 10), false);
//    resList = cph->getSignalRowsAboutToBeMoved();
//    QCOMPARE(resList.count(), 0);
    
//    dph->testEmitRowsMoved(0, 10);
//    resList = cph->getSignalRowsMoved();
//    QCOMPARE(resList.count(), 0);
    
    
    cph->getSignalHeaderDataChanged();
    dph->testEmitHeaderDataChanged(Qt::Horizontal, 0, 0);
    resList = cph->getSignalHeaderDataChanged();
    QCOMPARE(resList.count(), 1);
    QCOMPARE(resList[0].first, 0);
    QCOMPARE(resList[0].second, 0);

    
}

void TestCacheProxy::testCP_ResizeCache()
{
    cp = new HgCacheProxyModel();
    cph = new CacheProxyHelper(cp);
    dph = new DataProviderHelper(200);
    cp->resizeCache(100, 20);
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
   
    dph->getLastRelease();
    dph->getLastRequest();
    cp->release(0, 100);
    QCOMPARE(dph->getLastRelease().count(), 101);
    cp->release(-10, 0);
    QCOMPARE(dph->getLastRelease().count(), 1);
    cp->release(10, -10);
    QCOMPARE(dph->getLastRelease().count(), 11);
    cp->request(0, 100);
    QCOMPARE(dph->getLastRequest().count(), 101);
    cp->request(-10, 0);
    QCOMPARE(dph->getLastRequest().count(), 1);
    cp->request(-10, 10, HgBufferManagerObserver::HgRequestOrderDescending);
    QCOMPARE(dph->getLastRequest().count(), 11);
    
    cp->setDataProvider(NULL, 50, 20);
    res = cp->data(idx, v);
    QCOMPARE(res.isValid(), false);
    cp->request(0, 100, HgBufferManagerObserver::HgRequestOrderAscending);   
    QCOMPARE(dph->getLastRequest().count(), 0);    
    cp->release(0, 100);
    QCOMPARE(dph->getLastRelease().count(), 0);

}

void TestCacheProxy::testCP_Sort()
{
    cp = new HgCacheProxyModel();
    cph = new CacheProxyHelper(cp);
    dph = new DataProviderHelper(200);
    cp->setDataProvider(dph, 50, 20);

    cp->setSortRole(Qt::UserRole+2);
    QCOMPARE(cp->sortRole(), Qt::UserRole+2);
    cp->setSortRole(Qt::UserRole+2);
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
    cp->setSortCaseSensitivity(Qt::CaseSensitive);
    cp->setSortCaseSensitivity(Qt::CaseInsensitive);
    cp->setSortCaseSensitivity(Qt::CaseSensitive);
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
    cp->setSortLocaleAware(true);
    cp->setSortLocaleAware(false);
    QCOMPARE(cp->isSortLocaleAware(), false);
        
    QString s = QString("AAA");
    cp->setDynamicSortFilter(true);
    QCOMPARE(cp->dynamicSortFilter(), true);
    cp->setDynamicSortFilter(true);
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
    cp->setFilterRegExp(regexp);
    cp->setFilterCaseSensitivity(Qt::CaseSensitive);
    QCOMPARE(cp->filterCaseSensitivity(), Qt::CaseSensitive);   
    cp->setFilterCaseSensitivity(Qt::CaseSensitive);
    cp->setFilterCaseSensitivity(Qt::CaseInsensitive);
    cp->setFilterCaseSensitivity(Qt::CaseSensitive);
    
    idx = cp->index(1, 0);
    res = cp->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toString().startsWith("item"), false);
    
    cp->setFilterKeyColumn(1);
    QCOMPARE(cp->filterKeyColumn (), 1);
    cp->setFilterKeyColumn(1);
    cp->setFilterKeyColumn(0);
    
    cp->setFilterRole(Qt::UserRole+1);
    QCOMPARE(cp->filterRole(), Qt::UserRole+1);
    cp->setFilterRole(Qt::UserRole+1);
    cp->setFilterRole(Qt::DisplayRole);     
    
    regexp = QRegExp("ITEM*", Qt::CaseInsensitive, QRegExp::Wildcard);
    cp->setFilterRegExp(regexp);
    QVERIFY(cp->filterRegExp() == regexp);
    cp->setFilterCaseSensitivity(Qt::CaseInsensitive);
    QCOMPARE(cp->filterCaseSensitivity(), Qt::CaseInsensitive);   
    cp->setFilterCaseSensitivity(Qt::CaseInsensitive);

    idx = cp->index(1, 0);
    res = cp->data(idx, Qt::DisplayRole);
    QCOMPARE(res.isValid(), true);
    QCOMPARE(res.toString().startsWith("item"), true);

}

void TestCacheProxy::testCP_IsCached()
{
    cp = new HgCacheProxyModel();
    dph = new DataProviderHelper(1000);
    cp->setDataProvider(dph);
    
    QModelIndex idx = cp->index(0, 0);
    QVariant res = cp->data(idx, Qt::DisplayRole); //this will move buffer to 0-119
    int begin = 0;
    int end = 120;
    int size = dph->getCount();
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }
    
    idx = cp->index(50, 0);
    cp->data(idx, Qt::DisplayRole); //don't move buffer yet
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }

    idx = cp->index(89, 0);//don't move buffer yet
    cp->data(idx, Qt::DisplayRole);
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }
    
    idx = cp->index(100, 0); //now move buffer
    cp->data(idx, Qt::DisplayRole);
    begin = 40;
    end = 160;
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }

    idx = cp->index(500, 0); //move buffer
    cp->data(idx, Qt::DisplayRole);
    begin = 440;
    end = 560;
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }

    idx = cp->index(980, 0); //move buffer
    cp->data(idx, Qt::DisplayRole);
    begin = 880;
    end = 1000;
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }

    idx = cp->index(920, 0); //move buffer
    cp->data(idx, Qt::DisplayRole);
    begin = 880;
    end = 1000;
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }
    
    idx = cp->index(890, 0); //move buffer
    cp->data(idx, Qt::DisplayRole);
    begin = 830;
    end = 950;
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }
    
    idx = cp->index(10, 0); //move buffer
    cp->data(idx, Qt::DisplayRole);
    begin = 0;
    end = 120;
    for (int i=0; i<size; i++){
        QVERIFY2((dph->isCached(i))==((i>=begin)&& (i<end)), QString("begin:%1 end:%2 size:%3 i:%4").arg(begin).arg(end).arg(size).arg(i).toLatin1().data() );
    }
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
    int main (int argc, char* argv[]) 
    {
        for ( int i=0;i<argc; i++){
            if (strcmp(argv[i], "-o")==0 && i+1 <argc ){
                //let's make sure that folder specified after -o exists
                QDir file( QString::fromLatin1( argv[i+1] ));
                QString s = file.absolutePath ();
                s = s.left( s.lastIndexOf(file.dirName()) );
                if ( !file.exists(s) ){
                    file.mkpath(s);
                }
                break;
            }
        }
        
        QApplication app(argc, argv);
        QTEST_DISABLE_KEYPAD_NAVIGATION
        TestCacheProxy tc;
        return QTest::qExec(&tc, argc, argv);
    }
#endif
