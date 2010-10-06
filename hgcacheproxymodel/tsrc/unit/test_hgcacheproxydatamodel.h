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
*  Version     : %version: 6 %
*/
#ifndef TestCacheProxy_H
#define TestCacheProxy_H

#include <QObject>

class QAbstractItemModel;

class BMHelper;
class HgBufferManager;
class DataProviderHelper;
class HgCacheProxyModel;
class CacheProxyHelper;

class TestCacheProxy: public QObject
{
    Q_OBJECT

public:
    TestCacheProxy(): QObject() {};
    
private:
    void testQAbstractItemModel(QAbstractItemModel* model, int rows, int columns);
    
private slots:
    void initTestCase();//before first
    void init();//before each
    void cleanup();//after each
    void cleanupTestCase(); //after last
    
    //HgBufferManager
    void testBM_SetPosition();
    void testBM_ResetBuffer();
    void testBM_ItemCountChanged();
    void testBM_ResizeCache();
    
    //HgDataProviderModel
    void testDP_QAbstractItemModel();
    void testDP_RequestReleaseAndData();
    void testDP_CacheManagment();
    void testDP_QPixmapPool();
    void testDP_IconMode();
    
    //HgCacheProxyModel
    void testCP_QAbstractItemModel();
    void testCP_SignalsForward();
    void testCP_ResizeCache();
    void testCP_Data();
    void testCP_Sort();
    void testCP_Filter();
    void testCP_IsCached();
    
private:
    BMHelper* bmh;
    HgBufferManager* bm;
    DataProviderHelper* dph;
    HgCacheProxyModel* cp;
    CacheProxyHelper* cph;
    
};
#endif // TestCacheProxy_H
