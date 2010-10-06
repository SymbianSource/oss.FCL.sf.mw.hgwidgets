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
*  Version     : %version: 14 %
*/
#include <QList>
#include <QAbstractItemModel>

#include <hgwidgets/hgcacheproxymodel.h>
#include <hgwidgets/hgdataprovidermodel.h>
#include "hgbuffermanager.h"
#include "hglogger.h"





HgCacheProxyModel::HgCacheProxyModel(QObject *parent):
    QAbstractItemModel(parent),
    mBufferManager(0),
    mSortFilterProxyModel(new QSortFilterProxyModel(this)),
    mDataProviderModel(0),
    mSupressBM(false),
    mCurrentPos(0)
{
    TX_ENTRY
    connect(mSortFilterProxyModel, SIGNAL(columnsAboutToBeInserted(const QModelIndex&, int, int)),
            this, SLOT(sourceColumnsAboutToBeInserted(const QModelIndex&, int, int)));
    connect(mSortFilterProxyModel, SIGNAL(columnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            this, SLOT(sourceColumnsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
    connect(mSortFilterProxyModel, SIGNAL(columnsAboutToBeRemoved(const QModelIndex&, int, int)),
            this, SLOT(sourceColumnsAboutToBeRemoved(const QModelIndex&, int, int)));
    
    connect(mSortFilterProxyModel, SIGNAL(columnsInserted(const QModelIndex&, int, int)),
            this, SLOT(sourceColumnsInserted(const QModelIndex&, int, int)));
    connect(mSortFilterProxyModel, SIGNAL(columnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            this, SLOT(sourceColumnsMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
    connect(mSortFilterProxyModel, SIGNAL(columnsRemoved(const QModelIndex&, int, int )),
            this, SLOT(sourceColumnsRemoved(const QModelIndex&, int, int )));
    connect(mSortFilterProxyModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(sourceDataChanged(const QModelIndex&, const QModelIndex&)));
    connect(mSortFilterProxyModel, SIGNAL(headerDataChanged(Qt::Orientation, int, int)),
            this, SLOT(sourceHeaderDataChanged(Qt::Orientation, int, int)));
    connect(mSortFilterProxyModel, SIGNAL(layoutAboutToBeChanged()),
            this, SLOT(sourceLayoutAboutToBeChanged()));
    connect(mSortFilterProxyModel, SIGNAL(layoutChanged()),
            this, SLOT(sourceLayoutChanged()));
    connect(mSortFilterProxyModel, SIGNAL(modelAboutToBeReset()),
            this, SLOT(sourceModelAboutToBeReset()));
    connect(mSortFilterProxyModel, SIGNAL(modelReset()),
            this, SLOT(sourceModelReset()));
    connect(mSortFilterProxyModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
            this, SLOT(sourceRowsAboutToBeInserted(const QModelIndex&, int, int)));
    connect(mSortFilterProxyModel, SIGNAL(rowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            this, SLOT(sourceRowsAboutToBeMoved(const QModelIndex&, int, int, const QModelIndex&, int)));
    connect(mSortFilterProxyModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
            this, SLOT(sourceRowsAboutToBeRemoved(const QModelIndex&, int, int)));
    connect(mSortFilterProxyModel, SIGNAL(rowsInserted(const QModelIndex &, int , int)),
            this, SLOT(sourceRowsInserted(const QModelIndex &, int , int)));
    connect(mSortFilterProxyModel, SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex&, int)),
            this, SLOT(sourceRowsMoved(const QModelIndex &, int, int, const QModelIndex&, int)));
    connect(mSortFilterProxyModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            this, SLOT(sourceRowsRemoved(const QModelIndex&, int, int)));

    TX_EXIT
}

HgCacheProxyModel::~HgCacheProxyModel()
{
    TX_ENTRY
    delete mBufferManager;
    TX_EXIT    
}

void HgCacheProxyModel::setDataProvider(HgDataProviderModel *dataProvider, int cacheSize, int cacheTreshold)
{
    TX_ENTRY
    mDataProviderModel = dataProvider;
    mSortFilterProxyModel->setSourceModel(mDataProviderModel);
    if (mDataProviderModel) {
        mDataProviderModel->registerObserver(this);

        mSupressBM = true;
        resizeCache(cacheSize, cacheTreshold);
        mSupressBM = false;

        if (mBufferManager == NULL) {
            mBufferManager = new HgBufferManager(this, cacheSize, cacheTreshold, 0, count() );
        } else {
            mBufferManager->resetBuffer(0, count());
        }
        

    }
    TX_EXIT    
}

HgDataProviderModel* HgCacheProxyModel::DataProvider()
{
    return mDataProviderModel;
}

void HgCacheProxyModel::resizeCache(int newSize, int newTreshold)
{
    TX_ENTRY
    if (mDataProviderModel)
        mDataProviderModel->resizeQPixmapPool(newSize);
    if (mBufferManager)
        mBufferManager->resizeCache( newSize, newTreshold );
    TX_EXIT    
}

QModelIndex HgCacheProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row >= rowCount()) {
        row = -1;
    }
    if (column >= columnCount()) {
        column = -1;
    }
    
    return QAbstractItemModel::createIndex(row, column); 
}

QModelIndex HgCacheProxyModel::parent(const QModelIndex &child) const
{
    return mSortFilterProxyModel->parent(mapToSource(child));
}

int HgCacheProxyModel::rowCount(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->rowCount(mapToSource(parent));
}

int HgCacheProxyModel::columnCount(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->columnCount(mapToSource(parent));
}

bool HgCacheProxyModel::hasChildren(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->hasChildren(mapToSource(parent));
}

QVariant HgCacheProxyModel::data(const QModelIndex &index, int role) const
{    
    setBufferPosition(index.row());
    QVariant res = mSortFilterProxyModel->data(mapToSource(index), role);
    return res;
}

bool HgCacheProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return mSortFilterProxyModel->setData(mapToSource(index), value, role);
}

QVariant HgCacheProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return mSortFilterProxyModel->headerData(section, orientation, role);
}

bool HgCacheProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return mSortFilterProxyModel->setHeaderData(section, orientation, value, role);
}

QMap<int, QVariant> HgCacheProxyModel::itemData(const QModelIndex &index) const
{
    setBufferPosition(index.row());
    return mSortFilterProxyModel->itemData(mapToSource(index));
}

bool HgCacheProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    return mSortFilterProxyModel->setItemData(mapToSource(index), roles);
}

QStringList HgCacheProxyModel::mimeTypes() const
{
    return mSortFilterProxyModel->mimeTypes();
}

QMimeData *HgCacheProxyModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndexList list;
    for (int i=0; i < indexes.count(); i++) {
        list.append(mapToSource(indexes[i]));
    }
    return mSortFilterProxyModel->mimeData(list);
}

bool HgCacheProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    return mSortFilterProxyModel->dropMimeData(data, action, row, column, mapToSource(parent));
}

Qt::DropActions HgCacheProxyModel::supportedDropActions() const
{
    return mSortFilterProxyModel->supportedDropActions();
}

bool HgCacheProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->insertRows(row, count, mapToSource(parent));
}

bool HgCacheProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->insertColumns(column, count, mapToSource(parent));
}

bool HgCacheProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->removeRows(row, count, mapToSource(parent));
}

bool HgCacheProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->removeColumns(column, count, mapToSource(parent));
}

void HgCacheProxyModel::fetchMore(const QModelIndex &parent)
{
    mSortFilterProxyModel->fetchMore(mapToSource(parent));
}

bool HgCacheProxyModel::canFetchMore(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->canFetchMore(mapToSource(parent));
}        

Qt::ItemFlags HgCacheProxyModel::flags(const QModelIndex &index) const
{
    return mSortFilterProxyModel->flags(mapToSource(index));
}

void HgCacheProxyModel::sort(int column, Qt::SortOrder order)
{
    mSupressBM = true;
    sourceModelAboutToBeReset();
    mSortFilterProxyModel->sort(column, order);
    sourceModelReset();  
}

QModelIndex HgCacheProxyModel::buddy(const QModelIndex &index) const
{
    return mSortFilterProxyModel->buddy(mapToSource(index));
}

QModelIndexList HgCacheProxyModel::match(const QModelIndex &start, int role,
                              const QVariant &value, int hits,
                              Qt::MatchFlags flags) const
{
    return mSortFilterProxyModel->match(mapToSource(start), role, value, hits, flags);
}

QSize HgCacheProxyModel::span(const QModelIndex &index) const
{
    return mSortFilterProxyModel->span(mapToSource(index));
}

Qt::CaseSensitivity HgCacheProxyModel::sortCaseSensitivity() const
{
    return mSortFilterProxyModel->sortCaseSensitivity();
}

void HgCacheProxyModel::setSortCaseSensitivity(Qt::CaseSensitivity cs)
{
    if (sortCaseSensitivity() != cs) {
        mSupressBM = true;
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setSortCaseSensitivity(cs);
        sourceModelReset();  
    }
}

bool HgCacheProxyModel::isSortLocaleAware() const
{
    return mSortFilterProxyModel->isSortLocaleAware();
}

void HgCacheProxyModel::setSortLocaleAware(bool on)
{
    if (isSortLocaleAware() != on) {
        mSupressBM = true;    
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setSortLocaleAware(on);
        sourceModelReset();  
    }
}

int HgCacheProxyModel::sortColumn() const
{
    return mSortFilterProxyModel->sortColumn();
}

Qt::SortOrder HgCacheProxyModel::sortOrder() const
{
    return mSortFilterProxyModel->sortOrder();
}

bool HgCacheProxyModel::dynamicSortFilter() const
{
    return mSortFilterProxyModel->dynamicSortFilter();
}

void HgCacheProxyModel::setDynamicSortFilter(bool enable)
{
    if (dynamicSortFilter() != enable) {
        mSupressBM = true;   
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setDynamicSortFilter(enable);
        sourceModelReset();  
    }
}

int HgCacheProxyModel::sortRole() const
{
    return mSortFilterProxyModel->sortRole();
}

void HgCacheProxyModel::setSortRole(int role)
{
    if (sortRole() != role) {
        mSupressBM = true;
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setSortRole(role);
        sourceModelReset();    
    }
}

QRegExp HgCacheProxyModel::filterRegExp() const
{
    return mSortFilterProxyModel->filterRegExp();
}

void HgCacheProxyModel::setFilterRegExp(const QRegExp &regExp)
{
    if (filterRegExp() != regExp) {
        mSupressBM = true;
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setFilterRegExp(regExp);
        sourceModelReset();    
    }
}

int HgCacheProxyModel::filterKeyColumn() const
{
    return mSortFilterProxyModel->filterKeyColumn();
}

void HgCacheProxyModel::setFilterKeyColumn(int column)
{
    if (filterKeyColumn() != column) {
        mSupressBM = true;
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setFilterKeyColumn(column);
        sourceModelReset();    
    }
}

Qt::CaseSensitivity HgCacheProxyModel::filterCaseSensitivity() const
{
    return mSortFilterProxyModel->filterCaseSensitivity();
}

void HgCacheProxyModel::setFilterCaseSensitivity(Qt::CaseSensitivity cs)
{
    if (filterCaseSensitivity() != cs) {   
        mSupressBM = true;
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setFilterCaseSensitivity(cs);
        sourceModelReset();    
    }
}

int HgCacheProxyModel::filterRole() const
{
    return mSortFilterProxyModel->filterRole();
}

void HgCacheProxyModel::setFilterRole(int role)
{
    if (filterRole() != role) {
        mSupressBM = true;
        sourceModelAboutToBeReset();
        mSortFilterProxyModel->setFilterRole(role);
        sourceModelReset();    
    }
}

bool HgCacheProxyModel::submit()
{
    return mSortFilterProxyModel->submit();
}

void HgCacheProxyModel::revert()
{
    mSortFilterProxyModel->revert();
}

void HgCacheProxyModel::release(int start, int end)
{
    TX_ENTRY_ARGS( QString("%0-%1").arg(start).arg(end));   
    QList<int> list;
    int idx = 0;
    if (start > end) {
        idx = end;
        end = start;
        start = idx;
        idx = 0;
    }
    for (int i=start; i <=end; i++) {
        idx = mapToDataProviderIndex(i);
        if (idx >=0)
            list.append(idx);
    }
    if (mDataProviderModel) {
        mSupressBM = true;
        mDataProviderModel->release(list, false);
        mSupressBM = false;
    }
    TX_EXIT
}

void HgCacheProxyModel::request(int start, int end, HgRequestOrder order)
{
    TX_ENTRY_ARGS( QString("%0-%1").arg(start).arg(end));   
    QList<int> list;
    int idx;
    if (order == HgRequestOrderAscending) {
        for (int i=start; i <=end; i++) {
            idx = mapToDataProviderIndex(i);
            if (idx >=0)
                list.append(idx);
        }
    } else {
        for (int i=end; i >=start; i--) {
            idx = mapToDataProviderIndex(i);
            if (idx >=0)
                list.append(idx);
        }
    }
    if (mDataProviderModel && list.count()>0)
        mDataProviderModel->request(list, mSupressBM);
    TX_EXIT    
}

int HgCacheProxyModel::count()const
{
    return rowCount(index(0, 0, QModelIndex()));
}

QModelIndex HgCacheProxyModel::mapToSource(const QModelIndex &index) const
{
    return mapToSource(index.row(), index.column());
}

QModelIndex HgCacheProxyModel::mapToSource(int row, int col) const
{
    return mSortFilterProxyModel->index(row, col);
}

int HgCacheProxyModel::mapToDataProviderIndex(int myIndex) const
{
    return mSortFilterProxyModel->mapToSource( mapToSource(myIndex, 0)).row();
}

int HgCacheProxyModel::mapFromDataProviderIndex(int myIndex) const
{
    return mSortFilterProxyModel->mapFromSource( mDataProviderModel->index(myIndex, 0) ).row();
}

void HgCacheProxyModel::releaseAll()
{
    if (mDataProviderModel) {
        QList<int> list;
        for (int i=0; i<mDataProviderModel->rowCount(); i++) {
            list.append(i);
        }
        mDataProviderModel->release(list, true);
    }
}

void HgCacheProxyModel::setBufferPosition(int pos) const
{
    if (mBufferManager && !mSupressBM) {
        if (mCurrentPos!=pos) {
//            TX_LOG_ARGS(QString("pos:%1 ").arg(pos) );
            mCurrentPos = pos;
            mBufferManager->setPosition(mCurrentPos);
        }
    }
}

void HgCacheProxyModel::sourceColumnsAboutToBeInserted( const QModelIndex & parent, int start, int end)
{
    TX_ENTRY
    beginInsertColumns(parent, start, end);
    TX_EXIT
}

void HgCacheProxyModel::sourceColumnsAboutToBeMoved( const QModelIndex & sourceParent, int sourceStart, 
    int sourceEnd, const QModelIndex & destinationParent, int destinationColumn )
{
    TX_ENTRY
    beginMoveColumns(sourceParent, sourceStart, sourceEnd, destinationParent, destinationColumn);
    TX_EXIT
}

void HgCacheProxyModel::sourceColumnsAboutToBeRemoved( const QModelIndex & parent, int start, int end)
{
    TX_ENTRY
    beginRemoveColumns(parent, start, end);
    TX_EXIT
}

void HgCacheProxyModel::sourceColumnsInserted( const QModelIndex & parent, int start, int end )
{
    TX_ENTRY
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    endInsertColumns();
    TX_EXIT
}

void HgCacheProxyModel::sourceColumnsMoved( const QModelIndex & sourceParent, int sourceStart, 
        int sourceEnd, const QModelIndex & destinationParent, int destinationColumn )
{
    TX_ENTRY
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationColumn);
    endMoveColumns();
    TX_EXIT
}

void HgCacheProxyModel::sourceColumnsRemoved( const QModelIndex & parent, int start, int end )
{
    TX_ENTRY
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    endRemoveColumns();
    TX_EXIT
}

void HgCacheProxyModel::sourceDataChanged( const QModelIndex & topLeft, const QModelIndex & bottomRight )
{
    TX_ENTRY_ARGS(QString("from:%1 to:%2").arg( topLeft.row() ).arg( bottomRight.row() ) );
    QModelIndex begin = index(topLeft.row(), topLeft.column());
    QModelIndex end = index(bottomRight.row(), bottomRight.column());
    
    if (begin.isValid() && end.isValid() && !mSupressBM) {
        emit dataChanged(begin, end);
    }
    TX_EXIT
}

void HgCacheProxyModel::sourceHeaderDataChanged( Qt::Orientation orientation, int first, int last )
{
    TX_ENTRY
    emit headerDataChanged(orientation, first, last);
    TX_EXIT
}

void HgCacheProxyModel::sourceLayoutAboutToBeChanged()
{
    TX_ENTRY
    emit layoutAboutToBeChanged();
    TX_EXIT
}

void HgCacheProxyModel::sourceLayoutChanged()
{
    TX_ENTRY
    emit layoutChanged();
    TX_EXIT    
}

void HgCacheProxyModel::sourceModelAboutToBeReset()
{
    TX_ENTRY
    beginResetModel();
    mSupressBM = true;
    releaseAll();
    TX_EXIT
}

void HgCacheProxyModel::sourceModelReset()
{
    TX_ENTRY
    mSupressBM = true;
    if (mBufferManager) {
        mCurrentPos = 0;
        mBufferManager->resetBuffer(mCurrentPos, count());
    }
    mSupressBM = false;
    endResetModel();
    TX_EXIT
}

void HgCacheProxyModel::sourceRowsAboutToBeInserted( const QModelIndex & parent, int start, int end )
{
    TX_ENTRY
    if (mBufferManager && !mSupressBM) {
        beginInsertRows(parent, start, end);
        for (int i=start; i <=end; i++) {
            mBufferManager->aboutToInsertItem(i);
        }        
    }
    TX_EXIT
}

void HgCacheProxyModel::sourceRowsAboutToBeMoved( const QModelIndex & sourceParent, int sourceStart, 
        int sourceEnd, const QModelIndex & destinationParent, int destinationRow )
{
    TX_ENTRY
    beginMoveRows(sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
    TX_EXIT
}

void HgCacheProxyModel::sourceRowsAboutToBeRemoved( const QModelIndex & parent, int start, int end )
{
    TX_ENTRY
    if (mBufferManager && !mSupressBM) {
        beginRemoveRows(parent, start, end);
        for (int i=start; i <=end; i++) {
            mBufferManager->aboutToRemoveItem(i);
        }
    }
    TX_EXIT
}

void HgCacheProxyModel::sourceRowsInserted( const QModelIndex & parent, int start, int end )
{
    TX_ENTRY
    Q_UNUSED(parent);
    Q_UNUSED(end);
    if (mBufferManager && !mSupressBM) {
        for (int i=start; i <=end; i++) {
            mBufferManager->insertedItem(i);
        }
        endInsertRows();
    }
    TX_EXIT    
}

void HgCacheProxyModel::sourceRowsMoved( const QModelIndex & sourceParent, int sourceStart, 
        int sourceEnd, const QModelIndex & destinationParent, int destinationRow )
{
    TX_ENTRY
    Q_UNUSED(sourceParent);
    Q_UNUSED(sourceStart);
    Q_UNUSED(sourceEnd);
    Q_UNUSED(destinationParent);
    Q_UNUSED(destinationRow);
    endMoveRows();
    TX_EXIT
}

void HgCacheProxyModel::sourceRowsRemoved( const QModelIndex & parent, int start, int end )
{
    TX_ENTRY
    Q_UNUSED(parent);
    Q_UNUSED(end);
    if (mBufferManager && !mSupressBM) {
        for (int i=start; i <=end; i++) {
            mBufferManager->removedItem(i);
        }
        endRemoveRows();
    }
    TX_EXIT
}

void HgCacheProxyModel::dataUpdated(int from, int to)
{
    TX_LOG_ARGS(QString("from:%1 to:%2").arg(from).arg(to));
    QModelIndex begin;
    QModelIndex end;
    begin = index(mapFromDataProviderIndex(from),0);
    
    if (from == to) {
        end = begin;
    } else {
        end = index(mapFromDataProviderIndex(to),0);
    }
    
    if (begin.isValid() && end.isValid() && !mSupressBM) {
        emit dataChanged(begin, end );
    }
}

bool HgCacheProxyModel::isCached(int idx) const
{
    bool res = false;
    if (mBufferManager) {
        res = mBufferManager->isCached(mapFromDataProviderIndex(idx));
    }
    return res;
}

//eof
