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
*  Version     : %version: 3 %
*/
#include <e32debug.h>
#include <QVariant>
#include <HbIcon.h>
#include <qpixmapdata_p.h>
#include <hgwidgets/hgdataprovidermodel.h>
#include <hgwidgets/hgcacheproxymodel.h>
#include "hglogger.h"


const int KQPixmapCacheEmergencyBuffer = 5;

HgDataProviderModel::HgDataProviderModel(QObject *parent) : 
    QAbstractItemModel(parent),
    mCache(new QList<QMap<int, QVariant>*>()),
    mCacheSize(0),
    mUnallocatedPixmaps(0),
    mObserver(0)
{
    TX_ENTRY
    TX_EXIT
}

HgDataProviderModel::~HgDataProviderModel()
{
    TX_ENTRY    
    clearCache();
    delete mCache;
    qDeleteAll( mFreePixmaps.begin(), mFreePixmaps.end() );
    mFreePixmaps.clear();
    qDeleteAll( mUsedPixmaps.begin(), mUsedPixmaps.end() );
    mUsedPixmaps.clear();
    TX_EXIT    
}

void HgDataProviderModel::release(QList<int> list, bool silent)
{
    TX_ENTRY    
    int i=0;
    int min = count();
    int max = 0;
    
    for ( int idx = 0; idx < list.count(); idx++){
        i = list[idx];
        if ( i >=0 && i<count()){
            if ( i < min)
                min = i;
            if ( i > max)
                max = i;
            resetIcon(i);
        }
    }
    
    doReleaseData(list, silent);
    
    if (min<max){ //min<max is true if at least one item is in range <0,count())
        emitDataChanged(min, max, silent);
    }
    TX_EXIT    
}

void HgDataProviderModel::request(QList<int> list, bool silent)
{
    doRequestData(list, silent); 
}

void HgDataProviderModel::registerObserver(HgDataProviderModelObserver* obs)
{
    TX_ENTRY
    mObserver = obs;
    TX_EXIT    
}

QModelIndex HgDataProviderModel::index(int row, int column,
                                     const QModelIndex &parent) const
{   
    Q_UNUSED(parent);
    if (  row >= rowCount() ){
        row = -1;
    }
    if (  column >= columnCount() ){
        column = -1;
    }
    
    return QAbstractItemModel::createIndex(row, column); 
}

QModelIndex HgDataProviderModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();   //returns always invalid model index
}

int HgDataProviderModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return count();
}

int HgDataProviderModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant HgDataProviderModel::data(const QModelIndex &index, int role) const
{
    return data(index.row(), role);
}

QVariant HgDataProviderModel::data(int idx, int role) const
{
    QVariant res;
    if ( containsRole(idx, role)){
        res = mCache->at(idx)->value(role);
    } else if (role == Qt::DecorationRole ){
        res = defaultIcon();
    }
    return res;
}

QMap<int, QVariant> HgDataProviderModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> res;
    if ( index.row()>=0 && index.row()<count() ){
        res = QMap<int, QVariant>(*mCache->at(index.row()));
    }   
    return res;
}

void HgDataProviderModel::clearCache()
{
    qDeleteAll( mCache->begin(), mCache->end() );
    mCache->clear();
}

int HgDataProviderModel::count() const
{
    return mCache->count();
}

bool HgDataProviderModel::update(int pos, QList< QPair< QVariant, int > >* list, bool silent)
{
    bool change(false);
    if (list && list->count() && pos >=0 && pos<count() && mCache->at(pos)) {
        while(list->count()){
            QPair< QVariant, int > pair = list->takeFirst();
            change = update(pos, pair.first, pair.second, true)|change;
        }
        if ( !silent && change ){
            emitDataChanged(pos, pos, false);
        }
    }
    return change;
}

bool HgDataProviderModel::update(int pos, QVariant obj, int role, bool silent)
{
    bool change(false);
    
    if ( pos >=0 && pos<count() && mCache->at(pos)){
        mCache->at(pos)->insert(role, obj); //this will remove old one if needed
        change = true;
    }
    
    if ( !silent && change ){
        emitDataChanged(pos, pos, false);
    }
    return change;
}

bool HgDataProviderModel::updateIcon(int pos, QVariant obj, bool silent)
{
    if (obj.isValid()==false)
        return false;
    
    bool change(false);
    if ( pos >=0 && pos<count() && mCache->at(pos)){
        mCache->at(pos)->insert(Qt::DecorationRole, obj); //will remove old if needed
        change = true;
    }
    
    if (!silent && change){
        TX_LOG
        if ( mObserver){
            mObserver->dataUpdated(pos,pos);
        } else {
            QModelIndex topLeft = index(pos, 0);
            QModelIndex bottomRight = index(pos, 0);
            emit dataChanged(topLeft,bottomRight);
        }
    }
    return change;
}

void HgDataProviderModel::resetIcon(int pos)
{
    if ( containsRole(pos, Qt::DecorationRole)){
        mCache->at(pos)->remove(Qt::DecorationRole);
    }
}

void HgDataProviderModel::newItem(QList< QPair< QVariant, int > >* list, bool silent)
{
    insertItem(mCache->count(), list, silent);
}

void HgDataProviderModel::newItem(QPair< QVariant, int > item, bool silent)
{
    insertItem(mCache->count(), item, silent);
}

void HgDataProviderModel::insertItem(int pos, QList< QPair< QVariant, int > >* list, bool silent)
{
    doInsertItem(pos, list, silent);
}

void HgDataProviderModel::insertItem(int pos, QPair< QVariant, int > item, bool silent)
{
    QList< QPair< QVariant, int > > list;
    list.append(item);
    doInsertItem(pos, &list, silent);
}

void HgDataProviderModel::doInsertItem(int pos, QList< QPair< QVariant, int > >* list, bool silent)
{
    if (pos >mCache->count()){
        pos = mCache->count();
    } else if (pos <0){
        pos = 0;
    }
    
    if ( !silent){
        beginInsertRows(QModelIndex(), pos, pos);
    }
    
    mCache->insert(pos, new QMap<int, QVariant>());
    if (list && list->count()){
        update(pos, list, true);
    }
    
    if ( !silent){
        endInsertRows();
    } 
}


void HgDataProviderModel::removeItem(int pos)
{
    removeItems(pos, 1);
}

void HgDataProviderModel::removeItems(int pos, int size)
{
    if (pos >mCache->count())
        return;
    else if (pos <0){
        size = size + pos; //pos <0
        pos = 0;
    }
    
    if (size >mCache->count()){
        size = mCache->count();
    } else if (size <0){
        return;
    }
    
    beginRemoveRows(QModelIndex(),pos, pos+size-1);
    for (int i=0; i<size && pos<mCache->count(); i++){
        mCache->removeAt(pos);
    }
    endRemoveRows();
}
        
void HgDataProviderModel::resetModel() 
{
    beginResetModel();
    doResetModel();
    endResetModel();
}

void HgDataProviderModel::emitDataChanged(int from, int to, bool silent)
{
    if ( !silent ){
        TX_LOG    
        QModelIndex topLeft = index(from, 0);
        QModelIndex bottomRight = index(to, 0);
        emit dataChanged(topLeft,bottomRight);
    }
}

void HgDataProviderModel::resizeQPixmapPool(int newSize)
{
    mQPixmapsLock.lock();
    int currentSize = mFreePixmaps.count() + mUsedPixmaps.count();
    int diff = currentSize - newSize - KQPixmapCacheEmergencyBuffer;
    mUnallocatedPixmaps = 0;
    while (diff != 0){
        if (diff < 0){
            mUnallocatedPixmaps++;
            diff++;
        }else{
            if (mUnallocatedPixmaps>0){
                mUnallocatedPixmaps--;
            } else if (mFreePixmaps.count()){
                mFreePixmaps.removeLast();
            } //else will be deleted with releasePixmap;
            diff--;
        }
    }
    mQPixmapsLock.unlock();
    mCacheSize = newSize;
}

void HgDataProviderModel::releasePixmap(int idx)
{
    mQPixmapsLock.lock();
    if (mUsedPixmaps.contains(idx)){
        QPixmap* pix = mUsedPixmaps.take(idx);
        if ( mFreePixmaps.count() + mUsedPixmaps.count() + mUnallocatedPixmaps> mCacheSize + KQPixmapCacheEmergencyBuffer){
            delete pix; //we have too many pixmaps
        }else{
            mFreePixmaps.append(pix);
        }
    }else{
        TX_LOG_ARGS( QString("can't release pixmap for idx=%0").arg(idx));
    }
    mQPixmapsLock.unlock();    
}

QVariant HgDataProviderModel::createIcon(int index, QPixmap aPixmap)
{
	QPixmap* pix = getPixmap(index);
	if (pix){
		if ( pix->pixmapData() ){
			pix->pixmapData()->fromImage(aPixmap.toImage(), Qt::AutoColor );  
		} else {
			*pix = aPixmap;
		}
		mQPixmapsLock.lock();
		mUsedPixmaps.insert(index, pix);
		mQPixmapsLock.unlock();
		return HbIcon(QIcon(*pix));	
	}
	TX_EXIT_ARGS( QString("No pixmap avilable"));
	return QVariant();
}

QPixmap* HgDataProviderModel::getPixmap(int idx)
{
    TX_ENTRY
    QPixmap* res = NULL;
    mQPixmapsLock.lock();
    if ( mUsedPixmaps.contains(idx)){
        res = mUsedPixmaps.take(idx);//let's just replace pixmapdata for that pixmap
    } else {
        if (!mFreePixmaps.isEmpty()){
            res = mFreePixmaps.takeFirst();
        }else if (mUnallocatedPixmaps){
            mUnallocatedPixmaps--;
            res = new QPixmap();
        } else {
            TX_LOG_ARGS(QString("no free pixmaps"));
        }        
    }
    mQPixmapsLock.unlock();
    TX_EXIT    
    return res;
}

//eof
