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
#include "cacheproxyhelper.h"

CacheProxyHelper::CacheProxyHelper(HgCacheProxyModel *model, QObject *parent):
QObject(parent),
mModel(model)
{
    ASSERT(mModel!=0);
    
    connect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotDataChanged(QModelIndex,QModelIndex)));
    
    connect(mModel, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SLOT(slotHeaderDataChanged(Qt::Orientation,int,int)));

    connect(mModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(slotRowsAboutToBeInserted(QModelIndex,int,int)));

    connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotRowsInserted(QModelIndex,int,int)));

    connect(mModel, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(slotColumnsAboutToBeInserted(QModelIndex,int,int)));

    connect(mModel, SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SLOT(slotColumnsInserted(QModelIndex,int,int)));

    connect(mModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(slotRowsAboutToBeRemoved(QModelIndex,int,int)));

    connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(slotRowsRemoved(QModelIndex,int,int)));

    connect(mModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(slotColumnsAboutToBeRemoved(QModelIndex,int,int)));

    connect(mModel, SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SLOT(slotColumnsRemoved(QModelIndex,int,int)));

    connect(mModel, SIGNAL(layoutAboutToBeChanged()),
            this, SLOT(slotLayoutAboutToBeChanged()));

    connect(mModel, SIGNAL(layoutChanged()), 
            this, SLOT(slotLayoutChanged()));

    connect(mModel, SIGNAL(modelAboutToBeReset()), 
            this, SLOT(slotModelAboutToBeReset()));
    
    connect(mModel, SIGNAL(modelReset()), 
            this, SLOT(slotModelReset()));	
    
}

CacheProxyHelper::~CacheProxyHelper()
{
    disconnect(mModel);
}

void CacheProxyHelper::slotDataChanged(QModelIndex from,QModelIndex to)
{
    QPair< int, int > p;
    p.first = from.row();
    p.second = to.row();    
    mSignalDataChanged.append(p);
}

void CacheProxyHelper::slotHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    Q_UNUSED(orientation);    
    QPair< int, int > p;
    p.first = first;
    p.second = last;    
    mSignalHeaderDataChanged.append(p);
}

void CacheProxyHelper::slotRowsAboutToBeInserted(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;    
    mSignalRowsAboutToBeInserted.append(p);
}

void CacheProxyHelper::slotRowsInserted(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;     
    mSignalRowsInserted.append(p);
}

void CacheProxyHelper::slotColumnsAboutToBeInserted(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;    
    mSignalColumnsAboutToBeInserted.append(p);
}

void CacheProxyHelper::slotColumnsInserted(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;      
    mSignalColumnsInserted.append(p);
}

void CacheProxyHelper::slotRowsAboutToBeRemoved(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;      
    mSignalRowsAboutToBeRemoved.append(p);
}

void CacheProxyHelper::slotRowsRemoved(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;       
    mSignalRowsRemoved.append(p);
}

void CacheProxyHelper::slotColumnsAboutToBeRemoved(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;       
    mSignalColumnsAboutToBeRemoved.append(p);
}

void CacheProxyHelper::slotColumnsRemoved(QModelIndex parent,int from,int to)
{
    Q_UNUSED(parent);    
    QPair< int, int > p;
    p.first = from;
    p.second = to;       
    mSignalColumnsRemoved.append(p);
}

void CacheProxyHelper::slotLayoutAboutToBeChanged()
{
    mSignalLayoutAboutToBeChanged = true;
}

void CacheProxyHelper::slotLayoutChanged()
{
    mSignalLayoutChanged = true;
}

void CacheProxyHelper::slotModelAboutToBeReset()
{
    mSignalModelAboutToBeReset = true;
}

void CacheProxyHelper::slotModelReset()
{
    mSignalModelReset = true;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalDataChanged()
{
    QList< QPair< int, int > > res = mSignalDataChanged;
    mSignalDataChanged.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalHeaderDataChanged()
{
    QList< QPair< int, int > > res = mSignalHeaderDataChanged;
    mSignalHeaderDataChanged.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalRowsAboutToBeInserted()
{
    QList< QPair< int, int > > res = mSignalRowsAboutToBeInserted;
    mSignalRowsAboutToBeInserted.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalRowsInserted()
{
    QList< QPair< int, int > > res = mSignalRowsInserted;
    mSignalRowsInserted.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalColumnsAboutToBeInserted()
{
    QList< QPair< int, int > > res = mSignalColumnsAboutToBeInserted;
    mSignalColumnsAboutToBeInserted.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalColumnsInserted()
{
    QList< QPair< int, int > > res = mSignalColumnsInserted;
    mSignalColumnsInserted.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalRowsAboutToBeRemoved()
{
    QList< QPair< int, int > > res = mSignalRowsAboutToBeRemoved;
    mSignalRowsAboutToBeRemoved.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalRowsRemoved()
{
    QList< QPair< int, int > > res = mSignalRowsRemoved;
    mSignalRowsRemoved.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalColumnsAboutToBeRemoved()
{
    QList< QPair< int, int > > res = mSignalColumnsAboutToBeRemoved;
    mSignalColumnsAboutToBeRemoved.clear();
    return res;
}

QList< QPair< int, int > > CacheProxyHelper::getSignalColumnsRemoved()
{
    QList< QPair< int, int > > res = mSignalColumnsRemoved;
    mSignalColumnsRemoved.clear();
    return res;
}

bool CacheProxyHelper::getSignalLayoutAboutToBeChanged()
{
    bool res = mSignalLayoutAboutToBeChanged;
    mSignalLayoutAboutToBeChanged = false;
    return res;
}

bool CacheProxyHelper::getSignalLayoutChanged()
{
    bool res = mSignalLayoutChanged;
    mSignalLayoutChanged = false;
    return res;
}

bool CacheProxyHelper::getSignalModelAboutToBeReset()
{
    bool res = mSignalModelAboutToBeReset;
    mSignalModelAboutToBeReset = false;
    return res;
}

bool CacheProxyHelper::getSignalModelReset()
{
    bool res = mSignalModelReset;
    mSignalModelReset = false;
    return res;
}




