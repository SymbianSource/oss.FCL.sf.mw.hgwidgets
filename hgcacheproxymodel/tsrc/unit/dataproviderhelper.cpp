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
#include "dataproviderhelper.h"
#include <QtDebug>

const int KRole1 = Qt::UserRole+1;
const int KRole2 = Qt::UserRole+2;
const int KRole3 = Qt::UserRole+3;

DataProviderHelper::DataProviderHelper(int size, QObject *parent):
HgDataProviderModel(parent)
{
    mDefaultIcon = QString("DefaultIcon");
    
    for ( int i(0);i<size;i++){
        QList< QPair< QVariant, int > > list;
        int a = i;
        int b = -i;
        int c = a/4 + b/10 + i%30 + i/2;
        QString s = QString("ITEM%1 %2 %3").arg(c).arg(a).arg(b);
        if (i%2){
            s = s.toLower();
        }
        list.append( QPair< QVariant, int >(s, Qt::DisplayRole) );
        list.append( QPair< QVariant, int >(QVariant(a), KRole1) );
        list.append( QPair< QVariant, int >(QVariant(b), KRole2) );
        list.append( QPair< QVariant, int >(QVariant(c), KRole3) );
        newItem(&list, true);
    }
    
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotDataUpdated(QModelIndex,QModelIndex)));
    
    connect(this, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(slotRowsAboutToBeInserted(QModelIndex,int,int)));

    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotRowsInserted(QModelIndex,int,int)));    
    
    connect(this, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(slotRowsAboutToBeRemoved(QModelIndex,int,int)));

    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(slotRowsRemoved(QModelIndex,int,int)));    
    
    connect(this, SIGNAL(modelAboutToBeReset()), 
            this, SLOT(slotModelAboutToBeReset()));
    
    connect(this, SIGNAL(modelReset()), 
            this, SLOT(slotModelReset()));
}

DataProviderHelper::~DataProviderHelper()
{
    
}

void DataProviderHelper::doReleaseData(QList<int> list, bool silent)
{
    Q_UNUSED(silent);    
    mLastRelease = QList<int>(list);
}

void DataProviderHelper::doRequestData(QList<int> list, bool silent)
{
    Q_UNUSED(silent);
    mLastRequest = QList<int>(list);
    for ( int i=0; i<list.size(); i++){
        updateIcon(list[i], QString("MyNiceIcon %1").arg(list[i]) );
    }
}

QVariant DataProviderHelper::defaultIcon() const
{
    return mDefaultIcon;
}

void DataProviderHelper::testClearCache()
{
    clearCache();
}

void DataProviderHelper::testNewItem(QList< QPair< QVariant, int > >* list, bool silent)
{
    newItem(list, silent);
}

void DataProviderHelper::testNewItem(QPair< QVariant, int > item, bool silent)
{
    newItem(item, silent);
}

bool DataProviderHelper::testUpdate(int pos, QList< QPair< QVariant, int > >* list, bool silent)
{
    return update(pos, list, silent);
}

bool DataProviderHelper::testUpdate(int pos, QVariant obj, int role, bool silent )
{
    return update(pos, obj, role, silent);
}

void DataProviderHelper::testRemoveItems(int pos, int size)
{
    removeItems(pos, size);
}

void DataProviderHelper::testRemoveItem(int pos)
{
    removeItem(pos);
}

void DataProviderHelper::testInsertItem(int pos, QPair< QVariant, int > item, bool silent)
{
    insertItem(pos, item, silent);
}

void DataProviderHelper::doResetModel()
{
    clearCache();
    for ( int i(0);i<1000;i++){
        QList< QPair< QVariant, int > > list;
        int a = i;
        int b = -i;
        int c = a/4 + b/10 + i%30 + i/2;
        QString s = QString("NEW ITEM %1 %2 %3").arg(c).arg(a).arg(b);
        if (i%2){
            s = s.toLower();
        }
        list.append( QPair< QVariant, int >(s, Qt::DisplayRole) );
        list.append( QPair< QVariant, int >(QVariant(a), KRole1) );
        list.append( QPair< QVariant, int >(QVariant(b), KRole2) );
        list.append( QPair< QVariant, int >(QVariant(c), KRole3) );
        newItem(&list, true);
    }
}

bool DataProviderHelper::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool res = false;
    if ( index.row()>=0 && index.row() < count() ){
//        qWarning()<<index.row();
        res = update(index.row(), value, role, false);
    }
    return res;
}
bool DataProviderHelper::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    bool res = false;
    if ( index.row()>=0 && index.row() < count() ){
        QMapIterator<int, QVariant> i(roles);
        while (i.hasNext()){
            i.next();
            res = update(index.row(), i.value(), i.key(), true) | res;
        }
    }
    return res;
}

QList< QPair< int, int > > DataProviderHelper::getObserverDataUpdated()
{
    QList< QPair< int, int > > res = mObserverDataUpdated;
    mObserverDataUpdated.clear();
    return res;
}

QList< QPair< int, int > > DataProviderHelper::getSignalDataUpdated()
{
    QList< QPair< int, int > > res = mSignalDataUpdated;
    mSignalDataUpdated.clear();
    return res;
}

QList< QPair< int, int > > DataProviderHelper::getSignalRowsAboutToBeInserted()
{
    QList< QPair< int, int > > res = mSignalRowsAboutToBeInserted;
    mSignalRowsAboutToBeInserted.clear();
    return res;
}

QList< QPair< int, int > > DataProviderHelper::getSignalRowsInserted()
{
    QList< QPair< int, int > > res = mSignalRowsInserted;
    mSignalRowsInserted.clear();
    return res;
}

QList< QPair< int, int > > DataProviderHelper::getSignalRowsAboutToBeRemoved()
{
    QList< QPair< int, int > > res = mSignalRowsAboutToBeRemoved;
    mSignalRowsAboutToBeRemoved.clear();
    return res;
}

QList< QPair< int, int > > DataProviderHelper::getSignalRowsRemoved()
{
    QList< QPair< int, int > > res = mSignalRowsRemoved;
    mSignalRowsRemoved.clear();
    return res;
}

bool DataProviderHelper::getSignalAboutToBeReset()
{
    bool res = mSignalAboutToBeReset;
    mSignalAboutToBeReset = false;
    return res;
}

bool DataProviderHelper::getSignalReset()
{
    bool res = mSignalReset;
    mSignalReset = false;
    return res;
}

QList<int> DataProviderHelper::getLastRelease()
{
    QList<int> res = mLastRelease;
    mLastRelease.clear();
    return res;
}

QList<int> DataProviderHelper::getLastRequest()
{
    QList<int> res = mLastRequest;
    mLastRequest.clear();
    return res;
}

void DataProviderHelper::testReleasePixmap(int idx)
{
    releasePixmap(idx);
}

QVariant DataProviderHelper::testCreateIcon(int index, QPixmap aPixmap)
{
    return createIcon(index, aPixmap);
}

void DataProviderHelper::testUpdateIcon(int pos, QVariant icon, bool silent)
{
    updateIcon(pos, icon, silent);
}

void DataProviderHelper::testEmitDataChanged(QModelIndex from,QModelIndex to)
{
    emit dataChanged(from,to);
}

void DataProviderHelper::testEmitHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    emit headerDataChanged(orientation, first, last);
}

void DataProviderHelper::testEmitRowsInsert(QModelIndex parent,int from,int to)
{
    beginInsertRows(parent, from, to);
    endInsertRows();
}


void DataProviderHelper::testEmitColumnsInsert(QModelIndex parent,int from,int to)
{
    beginInsertColumns(parent, from, to);
    endInsertColumns();
}


void DataProviderHelper::testEmitRowsRemove(QModelIndex parent,int from,int to)
{
    beginRemoveRows(parent, from, to);
    endRemoveRows();
}

void DataProviderHelper::testEmitColumnsRemove(QModelIndex parent,int from,int to)
{
    beginRemoveColumns(parent, from, to);
    endRemoveColumns();
}

void DataProviderHelper::testEmitLayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void DataProviderHelper::testEmitLayoutChanged()
{
    emit layoutChanged();
}

void DataProviderHelper::testEmitModelAboutToBeReset()
{
    beginResetModel();
}

void DataProviderHelper::testEmitModelReset()
{
    endResetModel();
}

void DataProviderHelper::dataUpdated(int from, int to)
{
    QPair< int, int > p;
    p.first = from;
    p.second = to;    
    mObserverDataUpdated.append(p);
}

void DataProviderHelper::slotDataUpdated(QModelIndex from,QModelIndex to)
{
    QPair< int, int > p;
    p.first = from.row();
    p.second = to.row();    
    mSignalDataUpdated.append(p);
}

void DataProviderHelper::slotRowsAboutToBeInserted(QModelIndex parent, int from, int to)
{
    Q_UNUSED(parent);
    QPair< int, int > p;
    p.first = from;
    p.second = to;    
    mSignalRowsAboutToBeInserted.append(p);
}

void DataProviderHelper::slotRowsInserted(QModelIndex parent, int from, int to)
{
    Q_UNUSED(parent);
    QPair< int, int > p;
    p.first = from;
    p.second = to;    
    mSignalRowsInserted.append(p);
}

void DataProviderHelper::slotRowsAboutToBeRemoved(QModelIndex parent, int from, int to)
{
    Q_UNUSED(parent);
    QPair< int, int > p;
    p.first = from;
    p.second = to;    
    mSignalRowsAboutToBeRemoved.append(p);
}

void DataProviderHelper::slotRowsRemoved(QModelIndex parent, int from, int to)
{
    Q_UNUSED(parent);    
    Q_UNUSED(parent);
    QPair< int, int > p;
    p.first = from;
    p.second = to;    
    mSignalRowsRemoved.append(p);
}

void DataProviderHelper::slotModelAboutToBeReset()
{
    mSignalAboutToBeReset = true;
}

void DataProviderHelper::slotModelReset()
{
    mSignalReset = true;
}






