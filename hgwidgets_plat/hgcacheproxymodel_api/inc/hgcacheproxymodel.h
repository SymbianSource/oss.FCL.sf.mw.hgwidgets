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

class CACHEPROXYMODEL_EXPORT HgCacheProxyModel : public QAbstractItemModel, public HgBufferManagerObserver, public HgDataProviderModelObserver
{
    Q_OBJECT
public:
    HgCacheProxyModel(QObject *parent = 0);
    ~HgCacheProxyModel();   
    
    void setDataProvider(HgDataProviderModel *dataProvider, int cacheSize = KDefaultCacheSize, int cacheTreshold = KDefaultCacheTreshold);
    HgDataProviderModel* DataProvider();
    void resizeCache(int newSize, int newTreshold);
    
//from QAbstractItemModel
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    virtual QMap<int, QVariant> itemData(const QModelIndex &index) const;
    virtual bool setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles);
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    virtual Qt::DropActions supportedDropActions() const;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    virtual void fetchMore(const QModelIndex &parent);
    virtual bool canFetchMore(const QModelIndex &parent) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    virtual QModelIndex buddy(const QModelIndex &index) const;
    virtual QModelIndexList match(const QModelIndex &start, int role,
                                  const QVariant &value, int hits = 1,
                                  Qt::MatchFlags flags =
                                  Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const;
    virtual QSize span(const QModelIndex &index) const;

// for sorting
    Qt::CaseSensitivity sortCaseSensitivity() const;
    void setSortCaseSensitivity(Qt::CaseSensitivity cs);
    bool isSortLocaleAware() const;
    void setSortLocaleAware(bool on);
    int sortColumn() const;
    Qt::SortOrder sortOrder() const;
    bool dynamicSortFilter() const;
    void setDynamicSortFilter(bool enable);
    int sortRole() const;
    void setSortRole(int role);

//for filtering
    QRegExp filterRegExp() const;
    void setFilterRegExp(const QRegExp &regExp);
    int filterKeyColumn() const;
    void setFilterKeyColumn(int column);
    Qt::CaseSensitivity filterCaseSensitivity() const;
    void setFilterCaseSensitivity(Qt::CaseSensitivity cs);
    int filterRole() const;
    void setFilterRole(int role);
    
public slots:
    virtual bool submit();
    virtual void revert();
    
public:
//from HgBufferManagerObserver
    virtual void release(int start, int end);
    virtual void request(int start, int end, HgRequestOrder order = HgRequestOrderAscending);  
    
private:
    int count()const;
    QModelIndex mapToSource(const QModelIndex &index)const;
    QModelIndex mapToSource(int row, int col) const;
    int mapToDataProviderIndex(int myIndex) const;
    int mapFromDataProviderIndex(int myIndex) const;
    void cleanupForBMReset();
    void resetBM();
    void releaseAll();
    void setBufferPosition(int pos) const;
    
private slots:
    void sourceColumnsAboutToBeInserted( const QModelIndex & parent, int start, int end);
    void sourceColumnsAboutToBeMoved( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn );
    void sourceColumnsAboutToBeRemoved( const QModelIndex & parent, int start, int end);
    void sourceColumnsInserted( const QModelIndex & parent, int start, int end );
    void sourceColumnsMoved( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn );
    void sourceColumnsRemoved( const QModelIndex & parent, int start, int end );
    void sourceDataChanged( const QModelIndex & topLeft, const QModelIndex & bottomRight );
    void sourceHeaderDataChanged( Qt::Orientation orientation, int first, int last );
    void sourceLayoutAboutToBeChanged();
    void sourceLayoutChanged();
    void sourceModelAboutToBeReset();
    void sourceModelReset();
    void sourceRowsAboutToBeInserted( const QModelIndex & parent, int start, int end );
    void sourceRowsAboutToBeMoved( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow );
    void sourceRowsAboutToBeRemoved( const QModelIndex & parent, int start, int end );
    void sourceRowsInserted( const QModelIndex & parent, int start, int end );
    void sourceRowsMoved( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow );
    void sourceRowsRemoved( const QModelIndex & parent, int start, int end );
    
//from HgDataProviderModelObserver    
public:
    virtual void dataUpdated(int from, int to);
    
private:
    HgBufferManager* mBufferManager;
    QSortFilterProxyModel* mSortFilterProxyModel;
    HgDataProviderModel* mDataProviderModel;
    mutable bool mSupressBM;
    mutable int mCurrentPos;
};

#endif // HGCACHEPROXYMODEL_H_
