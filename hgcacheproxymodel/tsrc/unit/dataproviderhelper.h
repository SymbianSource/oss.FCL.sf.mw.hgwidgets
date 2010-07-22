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
*/
#ifndef DATAPROVIDERHELPER_H
#define DATAPROVIDERHELPER_H

#include <hgwidgets/hgdataprovidermodel.h>
#include <QList>
#include <QPair>

class DataProviderHelper : public HgDataProviderModel, public HgDataProviderModelObserver
{
    Q_OBJECT

public:
    DataProviderHelper(int size =1200, QObject *parent = 0);
    ~DataProviderHelper();

//from HgDataProviderModel
    virtual void doReleaseData(QList<int> list, bool silent);
    virtual void doRequestData(QList<int> list, bool silent);  
    virtual QVariant defaultIcon() const;
    virtual void doResetModel();
        
//from QAbstractItemModel
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);    
    virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);
    
//for testing
    void testClearCache();
    void testNewItem(QList< QPair< QVariant, int > >* list, bool silent);
    void testNewItem(QPair< QVariant, int > item, bool silent);
    bool testUpdate(int pos, QList< QPair< QVariant, int > >* list, bool silent);
    bool testUpdate(int pos, QVariant obj, int role, bool silent );
    void testRemoveItems(int pos, int size = 1);
    void testRemoveItem(int pos);
    void testInsertItem(int pos, QPair< QVariant, int > item, bool silent);
    void testUpdateIcon(int pos, QVariant icon, bool silent = false);
    
    QList< QPair< int, int > > getObserverDataUpdated();
    
    QList< QPair< int, int > > getSignalDataUpdated();
    QList< QPair< int, int > > getSignalRowsAboutToBeInserted();    
    QList< QPair< int, int > > getSignalRowsInserted();
    QList< QPair< int, int > > getSignalRowsAboutToBeRemoved();    
    QList< QPair< int, int > > getSignalRowsRemoved();
    bool getSignalAboutToBeReset();
    bool getSignalReset();    
    
    QList<int> getLastRelease();
    QList<int> getLastRequest();    
    
    void testReleasePixmap(int idx);
    QVariant testCreateIcon(int index, QPixmap aPixmap);    
    
    void testEmitDataChanged(QModelIndex from,QModelIndex to);
    void testEmitHeaderDataChanged(Qt::Orientation orientation, int first, int last);
    void testEmitRowsInsert(QModelIndex parent,int from,int to);
    void testEmitColumnsInsert(QModelIndex parent,int from,int to);
    void testEmitRowsRemove(QModelIndex parent,int from,int to);
    void testEmitColumnsRemove(QModelIndex parent,int from,int to);
    void testEmitLayoutAboutToBeChanged();
    void testEmitLayoutChanged();
    void testEmitModelAboutToBeReset();
    void testEmitModelReset();

    bool testEmitColumnsAboutToBeMoved(int from, int to);
    void testEmitColumnsMoved(int from, int to);
    bool testEmitRowsAboutToBeMoved(int from, int to);
    void testEmitRowsMoved(int from, int to);
    
    
    void testChangeIconMode(HgDataProviderIconMode mode);
    HgDataProviderIconMode testIconMode();
    
    void testClearItem(int pos, bool silent);
    QModelIndex testCreateIndex(int row, int col);
    int getCount();
    
//from HgDataProviderModelObserver
public:
    virtual void dataUpdated(int from, int to);

private slots:
    void slotDataUpdated(QModelIndex,QModelIndex);
    void slotRowsAboutToBeInserted(QModelIndex,int,int);
    void slotRowsInserted(QModelIndex,int,int);
    void slotRowsAboutToBeRemoved(QModelIndex,int,int);
    void slotRowsRemoved(QModelIndex,int,int);
    void slotModelAboutToBeReset();
    void slotModelReset();

private:
    QVariant mDefaultIcon;
    QList< QPair< int, int > > mObserverDataUpdated;
    
    QList< QPair< int, int > > mSignalDataUpdated;
    QList< QPair< int, int > > mSignalRowsAboutToBeInserted;    
    QList< QPair< int, int > > mSignalRowsInserted;
    QList< QPair< int, int > > mSignalRowsAboutToBeRemoved;    
    QList< QPair< int, int > > mSignalRowsRemoved;
    bool mSignalAboutToBeReset;
    bool mSignalReset;    
    QList<int> mLastRelease;
    QList<int> mLastRequest;
    
};

#endif // DATAPROVIDERHELPER_H
