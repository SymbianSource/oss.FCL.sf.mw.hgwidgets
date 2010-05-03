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
#ifndef CACHEPROXYHELPER_H
#define CACHEPROXYHELPER_H

#include <hgwidgets/hgcacheproxymodel.h>


class CacheProxyHelper: public QObject
{
    Q_OBJECT
public:
    CacheProxyHelper(HgCacheProxyModel *model, QObject *parent = 0);
    ~CacheProxyHelper();
    
private slots:
    void slotDataChanged(QModelIndex from,QModelIndex to);
    void slotHeaderDataChanged(Qt::Orientation orientation, int first, int last);
    void slotRowsAboutToBeInserted(QModelIndex parent,int from,int to);
    void slotRowsInserted(QModelIndex parent,int from,int to);
    void slotColumnsAboutToBeInserted(QModelIndex parent,int from,int to);
    void slotColumnsInserted(QModelIndex parent,int from,int to);
    void slotRowsAboutToBeRemoved(QModelIndex parent,int from,int to);
    void slotRowsRemoved(QModelIndex parent,int from,int to);
    void slotColumnsAboutToBeRemoved(QModelIndex parent,int from,int to);
    void slotColumnsRemoved(QModelIndex parent,int from,int to);
    void slotLayoutAboutToBeChanged();
    void slotLayoutChanged();
    void slotModelAboutToBeReset();
    void slotModelReset();
    
public:
    QList< QPair< int, int > > getSignalDataChanged();
    QList< QPair< int, int > > getSignalHeaderDataChanged();
    QList< QPair< int, int > > getSignalRowsAboutToBeInserted();
    QList< QPair< int, int > > getSignalRowsInserted();
    QList< QPair< int, int > > getSignalColumnsAboutToBeInserted();
    QList< QPair< int, int > > getSignalColumnsInserted();
    QList< QPair< int, int > > getSignalRowsAboutToBeRemoved();
    QList< QPair< int, int > > getSignalRowsRemoved();
    QList< QPair< int, int > > getSignalColumnsAboutToBeRemoved();
    QList< QPair< int, int > > getSignalColumnsRemoved();
    bool getSignalLayoutAboutToBeChanged();
    bool getSignalLayoutChanged();
    bool getSignalModelAboutToBeReset();
    bool getSignalModelReset();
    
private:
	HgCacheProxyModel* mModel;
	
    QList< QPair< int, int > > mSignalDataChanged;
    QList< QPair< int, int > > mSignalHeaderDataChanged;
    QList< QPair< int, int > > mSignalRowsAboutToBeInserted;
    QList< QPair< int, int > > mSignalRowsInserted;
    QList< QPair< int, int > > mSignalColumnsAboutToBeInserted;
    QList< QPair< int, int > > mSignalColumnsInserted;
    QList< QPair< int, int > > mSignalRowsAboutToBeRemoved;
    QList< QPair< int, int > > mSignalRowsRemoved;
    QList< QPair< int, int > > mSignalColumnsAboutToBeRemoved;
    QList< QPair< int, int > > mSignalColumnsRemoved;
    bool mSignalLayoutAboutToBeChanged;
    bool mSignalLayoutChanged;
    bool mSignalModelAboutToBeReset;
    bool mSignalModelReset;
    
};

#endif // CACHEPROXYHELPER_H
