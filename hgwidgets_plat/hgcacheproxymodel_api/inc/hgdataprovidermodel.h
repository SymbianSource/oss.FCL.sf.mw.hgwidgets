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
#ifndef HGDATAPROVIDERMODEL_H_
#define HGDATAPROVIDERMODEL_H_

#include <QObject>
#include <QList>
#include <QPair>
#include <QPixmap.h>
#include <QMap>
#include <QMutex>
#include <QAbstractItemModel>

class HbIcon;
class QVariant;
class HgCacheProxyModel;

#ifdef BUILD_CACHEPROXYMODEL
#define CACHEPROXYMODEL_EXPORT Q_DECL_EXPORT
#else
#define CACHEPROXYMODEL_EXPORT Q_DECL_IMPORT
#endif

class HgDataProviderModelObserver
{
public:
    virtual void dataUpdated(int from, int to) = 0;    
};

class CACHEPROXYMODEL_EXPORT HgDataProviderModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    HgDataProviderModel(QObject *parent = 0);
    ~HgDataProviderModel();   
//to cooperate with buffer manager    
    void release(QList<int> list, bool silent = true);
    void request(QList<int> list, bool silent = false);  
    virtual void doReleaseData(QList<int> list, bool silent) = 0;
    virtual void doRequestData(QList<int> list, bool silent) = 0;  
    void registerObserver(HgDataProviderModelObserver* obs);

public:
//from QAbstractItemModel
    QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
//cache management    
protected:
    QVariant data(int idx, int role) const;
    void clearCache();
    int count() const;
    bool update(int pos, QVariant obj, int role, bool silent = false);
    bool update(int pos, QList< QPair< QVariant, int > >* list, bool silent = false);    
    bool updateIcon(int pos, QVariant obj, bool silent = false);
    void resetIcon(int idx);
    void newItem(QList< QPair< QVariant, int > >* list = NULL, bool silent = true);
    void newItem(QPair< QVariant, int > item, bool silent = true);
    void insertItem(int pos, QList< QPair< QVariant, int > >* list = NULL, bool silent = true);
    void insertItem(int pos, QPair< QVariant, int > item, bool silent = true);

private:
    void doInsertItem(int pos, QList< QPair< QVariant, int > >* list, bool silent);

protected:
    void removeItem(int pos);
    void removeItems(int pos, int size);
    
    virtual QVariant defaultIcon() const = 0;
    inline bool containsRole(int idx, int role) const;
    inline bool isIndexValid(int idx) const;

public:    
    void resetModel();

protected:    
    virtual void doResetModel() {};
    
// helpers fot emits
protected:  
    void emitDataChanged(int start, int end, bool silent);
    
//QPixmap pool
public:
    void resizeQPixmapPool(int size);
    
protected:  
    void releasePixmap(int idx);
    QVariant createIcon(int index, QPixmap aPixmap);

private:
    QPixmap* getPixmap(int idx);

    
private:
    QList<QMap<int, QVariant>*>* mCache;   
    int mCacheSize; //Number of cached Icons. Not same as mCache->count()
    QList< QPixmap* > mFreePixmaps;
    QMap< int, QPixmap* > mUsedPixmaps;    
    int mUnallocatedPixmaps;
//    QMutex mQPixmapsLock;
    HgDataProviderModelObserver *mObserver;
};

inline bool HgDataProviderModel::isIndexValid(int idx) const
{
    return ((idx >= 0) && 
            (idx < mCache->size()) && 
            (mCache->at(idx))); 
}

inline bool HgDataProviderModel::containsRole(int idx, int role) const
{
    return ( isIndexValid(idx) && 
             mCache->at(idx)->contains(role) ); 
}




#endif // HGDATAPROVIDERMODEL_H_
