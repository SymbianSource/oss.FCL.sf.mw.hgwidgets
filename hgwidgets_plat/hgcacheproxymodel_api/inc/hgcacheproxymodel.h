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
*  Version     : %version: 2 %
*/

#ifndef HGCACHEPROXYMODEL_H_
#define HGCACHEPROXYMODEL_H_

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

#include <hgwidgets/hgdataprovidermodel.h>

#ifdef BUILD_CACHEPROXYMODEL
#define CACHEPROXYMODEL_EXPORT Q_DECL_EXPORT
#else
#define CACHEPROXYMODEL_EXPORT Q_DECL_IMPORT
#endif

class HgBufferManager;

const int KDefaultCacheSize = 120;
const int KDefaultCacheTreshold = 30;

class HgBufferManagerObserver
{
public:
    enum HgRequestOrder {
        HgRequestOrderAscending,
        HgRequestOrderDescending
    };   

    virtual void release(int start, int end) = 0;
    virtual void request(int start, int end, HgRequestOrder order = HgRequestOrderAscending) = 0;  
};

class HgCacheProxyModel : public QAbstractItemModel, public HgBufferManagerObserver, public HgDataProviderModelObserver
{
    Q_OBJECT
public:
    CACHEPROXYMODEL_EXPORT HgCacheProxyModel(QObject *parent = 0);
    CACHEPROXYMODEL_EXPORT ~HgCacheProxyModel();   
    
    CACHEPROXYMODEL_EXPORT void setDataProvider(HgDataProviderModel *dataProvider, int cacheSize = KDefaultCacheSize, int cacheTreshold = KDefaultCacheTreshold);
    CACHEPROXYMODEL_EXPORT HgDataProviderModel* DataProvider();
    CACHEPROXYMODEL_EXPORT void resizeCache(int newSize, int newTreshold);
    
//from QAbstractItemModel
    CACHEPROXYMODEL_EXPORT virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    CACHEPROXYMODEL_EXPORT virtual QModelIndex parent(const QModelIndex &child) const;
    CACHEPROXYMODEL_EXPORT virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    CACHEPROXYMODEL_EXPORT virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    CACHEPROXYMODEL_EXPORT virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    CACHEPROXYMODEL_EXPORT virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    CACHEPROXYMODEL_EXPORT virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    CACHEPROXYMODEL_EXPORT virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    CACHEPROXYMODEL_EXPORT virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    CACHEPROXYMODEL_EXPORT virtual QMap<int, QVariant> itemData(const QModelIndex &index) const;
    CACHEPROXYMODEL_EXPORT virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);
    CACHEPROXYMODEL_EXPORT virtual QStringList mimeTypes() const;
    CACHEPROXYMODEL_EXPORT virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    CACHEPROXYMODEL_EXPORT virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    CACHEPROXYMODEL_EXPORT virtual Qt::DropActions supportedDropActions() const;
    CACHEPROXYMODEL_EXPORT virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    CACHEPROXYMODEL_EXPORT virtual bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    CACHEPROXYMODEL_EXPORT virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    CACHEPROXYMODEL_EXPORT virtual bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    CACHEPROXYMODEL_EXPORT virtual void fetchMore(const QModelIndex &parent);
    CACHEPROXYMODEL_EXPORT virtual bool canFetchMore(const QModelIndex &parent) const;
    CACHEPROXYMODEL_EXPORT virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    CACHEPROXYMODEL_EXPORT virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    CACHEPROXYMODEL_EXPORT virtual QModelIndex buddy(const QModelIndex &index) const;
    CACHEPROXYMODEL_EXPORT virtual QModelIndexList match(const QModelIndex &start, int role,
                                  const QVariant &value, int hits = 1,
                                  Qt::MatchFlags flags =
                                  Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;
    CACHEPROXYMODEL_EXPORT virtual QSize span(const QModelIndex &index) const;

// for sorting
    CACHEPROXYMODEL_EXPORT Qt::CaseSensitivity sortCaseSensitivity() const;
    CACHEPROXYMODEL_EXPORT void setSortCaseSensitivity(Qt::CaseSensitivity cs);
    CACHEPROXYMODEL_EXPORT bool isSortLocaleAware() const;
    CACHEPROXYMODEL_EXPORT void setSortLocaleAware(bool on);
    CACHEPROXYMODEL_EXPORT int sortColumn() const;
    CACHEPROXYMODEL_EXPORT Qt::SortOrder sortOrder() const;
    CACHEPROXYMODEL_EXPORT bool dynamicSortFilter() const;
    CACHEPROXYMODEL_EXPORT void setDynamicSortFilter(bool enable);
    CACHEPROXYMODEL_EXPORT int sortRole() const;
    CACHEPROXYMODEL_EXPORT void setSortRole(int role);

//for filtering
//    CACHEPROXYMODEL_EXPORT QRegExp filterRegExp() const;
//    CACHEPROXYMODEL_EXPORT void setFilterRegExp(const QRegExp &regExp);
//    CACHEPROXYMODEL_EXPORT int filterKeyColumn() const;
//    CACHEPROXYMODEL_EXPORT void setFilterKeyColumn(int column);
//    CACHEPROXYMODEL_EXPORT Qt::CaseSensitivity filterCaseSensitivity() const;
//    CACHEPROXYMODEL_EXPORT void setFilterCaseSensitivity(Qt::CaseSensitivity cs);
//    CACHEPROXYMODEL_EXPORT int filterRole() const;
//    CACHEPROXYMODEL_EXPORT void setFilterRole(int role);
    
public slots:
    virtual bool submit();
    virtual void revert();
    
public:
//from HgBufferManagerObserver
    CACHEPROXYMODEL_EXPORT virtual void release(int start, int end);
    CACHEPROXYMODEL_EXPORT virtual void request(int start, int end, HgRequestOrder order = HgRequestOrderAscending);  
    
private:
    int count()const;
    QModelIndex mapToSource(const QModelIndex &index)const;
    QModelIndex mapToSource(int row, int col) const;
    int mapToDataProviderIndex(int myIndex) const;
    int mapFromDataProviderIndex(int myIndex) const;
    void cleanupForBMResetIfNeeded();
    void resetBMIfNeeded();
    void releaseAll();
    void setBufferPosition(int pos) const;
    
private slots:
    void sourceReset();
    void sourceAboutToBeReset();
    void sourceRowsInserted(const QModelIndex &source_parent, int start, int end);
    void sourceRowsRemoved(const QModelIndex &source_parent, int start, int end);
    void dataUpdated(QModelIndex from, QModelIndex to);
    
//from HgDataProviderModelObserver    
public:
    CACHEPROXYMODEL_EXPORT virtual void dataUpdated(int from, int to);
    
private:
    HgBufferManager* mBufferManager;
    QSortFilterProxyModel* mSortFilterProxyModel;
    HgDataProviderModel* mDataProviderModel;
    mutable bool mResetNeeded;
    mutable bool mSupressBM;
    mutable int mCurrentPos;
    bool mSortParameterChanged;
    bool mFilterParameterChanged;
    
};

#endif // HGCACHEPROXYMODEL_H_
