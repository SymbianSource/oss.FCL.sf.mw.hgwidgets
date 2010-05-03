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
#include <QList>
#include <QAbstractItemModel>

#include <hgwidgets/hgcacheproxymodel.h>
#include <hgwidgets/hgdataprovidermodel.h>
#include "hgbuffermanager.h"
#include "hglogger.h"





Q_DECL_EXPORT HgCacheProxyModel::HgCacheProxyModel(QObject *parent):
QAbstractItemModel(parent),
mBufferManager(0),
mSortFilterProxyModel(new QSortFilterProxyModel(this)),
mDataProviderModel(0),
mResetNeeded(false),
mSupressBM(false),
mSortParameterChanged(true),
mFilterParameterChanged(true)
{
    connect(mSortFilterProxyModel, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
            this, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)),
            this, SIGNAL(columnsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));    
    
    connect(mSortFilterProxyModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
            this, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)));    
    
    connect(mSortFilterProxyModel, SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SIGNAL(columnsInserted(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)),
            this, SIGNAL(columnsMoved(QModelIndex, int, int, QModelIndex, int)));
    
    connect(mSortFilterProxyModel, SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SIGNAL(columnsRemoved(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(dataUpdated(QModelIndex,QModelIndex)));
    
    connect(mSortFilterProxyModel, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SIGNAL(headerDataChanged(Qt::Orientation,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(layoutAboutToBeChanged()),
            this, SIGNAL(layoutAboutToBeChanged()));
    
    connect(mSortFilterProxyModel, SIGNAL(layoutChanged()), 
            this, SIGNAL(layoutChanged()));
    
    connect(mSortFilterProxyModel, SIGNAL(modelAboutToBeReset()), 
            this, SLOT(sourceAboutToBeReset()));
    
    connect(mSortFilterProxyModel, SIGNAL(modelReset()), 
            this, SLOT(sourceReset()));    
    
    connect(mSortFilterProxyModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)),
            this, SIGNAL(rowsAboutToBeMoved(QModelIndex, int, int, QModelIndex, int)));
    
    connect(mSortFilterProxyModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SIGNAL(rowsInserted(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceRowsInserted(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
            this, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)));
    
    connect(mSortFilterProxyModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SIGNAL(rowsRemoved(QModelIndex,int,int)));
    
    connect(mSortFilterProxyModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(sourceRowsRemoved(QModelIndex,int,int)));
}

Q_DECL_EXPORT HgCacheProxyModel::~HgCacheProxyModel()
{
    delete mBufferManager;
}

Q_DECL_EXPORT void HgCacheProxyModel::setDataProvider(HgDataProviderModel *dataProvider, int cacheSize, int cacheTreshold)
{
    TX_ENTRY
    mDataProviderModel = dataProvider;
    mSortFilterProxyModel->setSourceModel(mDataProviderModel);
    if (mDataProviderModel){
        mDataProviderModel->registerObserver(this);
        mDataProviderModel->resizeQPixmapPool(cacheSize);

        delete mBufferManager;
        mBufferManager = NULL;
        mBufferManager = new HgBufferManager(this, cacheSize, cacheTreshold, 0, count() );
    }
    TX_EXIT    
}

Q_DECL_EXPORT HgDataProviderModel* HgCacheProxyModel::DataProvider()
{
    return mDataProviderModel;
}

Q_DECL_EXPORT void HgCacheProxyModel::resizeCache(int newSize, int newTreshold)
{
    TX_ENTRY
    if (mDataProviderModel)
        mDataProviderModel->resizeQPixmapPool(newSize);
    if (mBufferManager)
        mBufferManager->resizeCache( newSize, newTreshold );
    TX_EXIT    
}

Q_DECL_EXPORT QModelIndex HgCacheProxyModel::index(int row, int column, const QModelIndex &parent) const
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

Q_DECL_EXPORT QModelIndex HgCacheProxyModel::parent(const QModelIndex &child) const
{
    return mSortFilterProxyModel->parent(mapToSource(child));
}

Q_DECL_EXPORT int HgCacheProxyModel::rowCount(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->rowCount(mapToSource(parent));
}

Q_DECL_EXPORT int HgCacheProxyModel::columnCount(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->columnCount(mapToSource(parent));
}

Q_DECL_EXPORT bool HgCacheProxyModel::hasChildren(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->hasChildren(mapToSource(parent));
}

Q_DECL_EXPORT QVariant HgCacheProxyModel::data(const QModelIndex &index, int role) const
{    
    setBufferPosition(index.row());
    QVariant res = mSortFilterProxyModel->data(mapToSource(index), role);
    return res;
}

Q_DECL_EXPORT bool HgCacheProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return mSortFilterProxyModel->setData(mapToSource(index), value, role);
}

Q_DECL_EXPORT QVariant HgCacheProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return mSortFilterProxyModel->headerData(section, orientation, role);
}

Q_DECL_EXPORT bool HgCacheProxyModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    return mSortFilterProxyModel->setHeaderData(section, orientation, value, role);
}

Q_DECL_EXPORT QMap<int, QVariant> HgCacheProxyModel::itemData(const QModelIndex &index) const
{
    setBufferPosition(index.row());
    return mSortFilterProxyModel->itemData(mapToSource(index));
}

Q_DECL_EXPORT bool HgCacheProxyModel::setItemData(const QModelIndex &index, const QMap<int, QVariant> &roles)
{
    return mSortFilterProxyModel->setItemData(mapToSource(index), roles);
}

Q_DECL_EXPORT QStringList HgCacheProxyModel::mimeTypes() const
{
    return mSortFilterProxyModel->mimeTypes();
}

Q_DECL_EXPORT QMimeData *HgCacheProxyModel::mimeData(const QModelIndexList &indexes) const
{
    QModelIndexList list;
    for ( int i=0; i < indexes.count(); i++){
        list.append(mapToSource(indexes[i]));
    }
    return mSortFilterProxyModel->mimeData(list);
}

Q_DECL_EXPORT bool HgCacheProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    return mSortFilterProxyModel->dropMimeData(data, action, row, column, mapToSource(parent));
}

Q_DECL_EXPORT Qt::DropActions HgCacheProxyModel::supportedDropActions() const
{
    return mSortFilterProxyModel->supportedDropActions();
}

Q_DECL_EXPORT bool HgCacheProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->insertRows(row, count, mapToSource(parent));
}

Q_DECL_EXPORT bool HgCacheProxyModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->insertColumns(column, count, mapToSource(parent));
}

Q_DECL_EXPORT bool HgCacheProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->removeRows(row, count, mapToSource(parent));
}

Q_DECL_EXPORT bool HgCacheProxyModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    return mSortFilterProxyModel->removeColumns(column, count, mapToSource(parent));
}

Q_DECL_EXPORT void HgCacheProxyModel::fetchMore(const QModelIndex &parent)
{
    mSortFilterProxyModel->fetchMore(mapToSource(parent));
}

Q_DECL_EXPORT bool HgCacheProxyModel::canFetchMore(const QModelIndex &parent) const
{
    return mSortFilterProxyModel->canFetchMore(mapToSource(parent));
}        

Q_DECL_EXPORT Qt::ItemFlags HgCacheProxyModel::flags(const QModelIndex &index) const
{
    return mSortFilterProxyModel->flags(mapToSource(index));
}

Q_DECL_EXPORT void HgCacheProxyModel::sort(int column, Qt::SortOrder order)
{
    if ( mSortParameterChanged || (sortColumn() != column) || (sortOrder() != order) ){
        mSortParameterChanged = false;
        mResetNeeded = true;
        cleanupForBMResetIfNeeded();
        mSortFilterProxyModel->sort(column, order);
        resetBMIfNeeded();
    }
}

Q_DECL_EXPORT QModelIndex HgCacheProxyModel::buddy(const QModelIndex &index) const
{
    return mSortFilterProxyModel->buddy(mapToSource(index));
}

Q_DECL_EXPORT QModelIndexList HgCacheProxyModel::match(const QModelIndex &start, int role,
                              const QVariant &value, int hits,
                              Qt::MatchFlags flags) const
{
    return mSortFilterProxyModel->match(mapToSource(start), role, value, hits, flags);
}

Q_DECL_EXPORT QSize HgCacheProxyModel::span(const QModelIndex &index) const
{
    return mSortFilterProxyModel->span(mapToSource(index));
}

Q_DECL_EXPORT Qt::CaseSensitivity HgCacheProxyModel::sortCaseSensitivity() const
{
    return mSortFilterProxyModel->sortCaseSensitivity();
}

Q_DECL_EXPORT void HgCacheProxyModel::setSortCaseSensitivity(Qt::CaseSensitivity cs)
{
    if (sortCaseSensitivity() != cs){
        mSortParameterChanged = true;
        cleanupForBMResetIfNeeded();
        mSortFilterProxyModel->setSortCaseSensitivity(cs);
        resetBMIfNeeded();    
    }
}

Q_DECL_EXPORT bool HgCacheProxyModel::isSortLocaleAware() const
{
    return mSortFilterProxyModel->isSortLocaleAware();
}

Q_DECL_EXPORT void HgCacheProxyModel::setSortLocaleAware(bool on)
{
    if (isSortLocaleAware() != on){
        mSortParameterChanged = true;    
        cleanupForBMResetIfNeeded();
        mSortFilterProxyModel->setSortLocaleAware(on);
        resetBMIfNeeded();    
    }
}

Q_DECL_EXPORT int HgCacheProxyModel::sortColumn() const
{
    return mSortFilterProxyModel->sortColumn();
}

Q_DECL_EXPORT Qt::SortOrder HgCacheProxyModel::sortOrder() const
{
    return mSortFilterProxyModel->sortOrder();
}

Q_DECL_EXPORT bool HgCacheProxyModel::dynamicSortFilter() const
{
    return mSortFilterProxyModel->dynamicSortFilter();
}

Q_DECL_EXPORT void HgCacheProxyModel::setDynamicSortFilter(bool enable)
{
    if (dynamicSortFilter() != enable){
        mSortParameterChanged = true;   
        mFilterParameterChanged = true;
        cleanupForBMResetIfNeeded();
        mSortFilterProxyModel->setDynamicSortFilter(enable);
        resetBMIfNeeded();
    }
}

Q_DECL_EXPORT int HgCacheProxyModel::sortRole() const
{
    return mSortFilterProxyModel->sortRole();
}

Q_DECL_EXPORT void HgCacheProxyModel::setSortRole(int role)
{
    if (sortRole() != role){
        mSortParameterChanged = true;   
        cleanupForBMResetIfNeeded();
        mSortFilterProxyModel->setSortRole(role);
        resetBMIfNeeded();    
    }
}

//Q_DECL_EXPORT QRegExp HgCacheProxyModel::filterRegExp() const
//{
//    return mSortFilterProxyModel->filterRegExp();
//}
//
//Q_DECL_EXPORT void HgCacheProxyModel::setFilterRegExp(const QRegExp &regExp)
//{
//    if (filterRegExp() != regExp){   
//        mFilterParameterChanged = true;
//        cleanupForBMResetIfNeeded();
//        mSortFilterProxyModel->setFilterRegExp(regExp);
//        resetBMIfNeeded();    
//    }
//}
//
//Q_DECL_EXPORT int HgCacheProxyModel::filterKeyColumn() const
//{
//    return mSortFilterProxyModel->filterKeyColumn();
//}
//
//Q_DECL_EXPORT void HgCacheProxyModel::setFilterKeyColumn(int column)
//{
//    if (filterKeyColumn() != column){   
//        mFilterParameterChanged = true;
//        cleanupForBMResetIfNeeded();
//        mSortFilterProxyModel->setFilterKeyColumn(column);
//        resetBMIfNeeded();    
//    }
//}
//
//Q_DECL_EXPORT Qt::CaseSensitivity HgCacheProxyModel::filterCaseSensitivity() const
//{
//    return mSortFilterProxyModel->filterCaseSensitivity();
//}
//
//Q_DECL_EXPORT void HgCacheProxyModel::setFilterCaseSensitivity(Qt::CaseSensitivity cs)
//{
//    if ( filterCaseSensitivity() != cs){   
//        mFilterParameterChanged = true;
//        cleanupForBMResetIfNeeded();
//        mSortFilterProxyModel->setFilterCaseSensitivity(cs);
//        resetBMIfNeeded();    
//    }
//}
//
//Q_DECL_EXPORT int HgCacheProxyModel::filterRole() const
//{
//    return mSortFilterProxyModel->filterRole();
//}
//
//Q_DECL_EXPORT void HgCacheProxyModel::setFilterRole(int role)
//{
//    if ( mFilterParameterChanged || (filterRole() != role)){   
//        mFilterParameterChanged = false;
//        mResetNeeded = true;
//        cleanupForBMResetIfNeeded();
//        mSortFilterProxyModel->setFilterRole(role);
//        resetBMIfNeeded();
//    }
//}

Q_DECL_EXPORT bool HgCacheProxyModel::submit()
{
    return mSortFilterProxyModel->submit();
}

Q_DECL_EXPORT void HgCacheProxyModel::revert()
{
    mSortFilterProxyModel->revert();
}

Q_DECL_EXPORT void HgCacheProxyModel::release(int start, int end)
{
    TX_ENTRY_ARGS( QString("%0-%1").arg(start).arg(end));   
    QList<int> list;
    int idx = 0;
    if ( start > end){
        idx = end;
        end = start;
        start = idx;
        idx = 0;
    }
    for ( int i=start; i <=end; i++){
        idx = mapToDataProviderIndex(i);
        if ( idx >=0)
            list.append(idx);
    }
    if (mDataProviderModel)
        mDataProviderModel->release(list, true);
    TX_EXIT    
}

Q_DECL_EXPORT void HgCacheProxyModel::request(int start, int end, HgRequestOrder order)
{
    TX_ENTRY_ARGS( QString("%0-%1").arg(start).arg(end));   
    QList<int> list;
    int idx;
    if (order == HgRequestOrderAscending){
        for ( int i=start; i <=end; i++){
            idx = mapToDataProviderIndex(i);
            if ( idx >=0)
                list.append(idx);
        }
    } else {
        for ( int i=end; i >=start; i--){
            idx = mapToDataProviderIndex(i);
            if ( idx >=0)
                list.append(idx);
        }
    }
    if (mDataProviderModel)
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

void HgCacheProxyModel::cleanupForBMResetIfNeeded()
{
    if (mResetNeeded || mSortFilterProxyModel->dynamicSortFilter()==true){
        TX_LOG
        mSupressBM = true;
        mResetNeeded = true;
        beginResetModel();
        releaseAll();
    }
}

void HgCacheProxyModel::resetBMIfNeeded()
{
    if ( mResetNeeded ){
        if (mBufferManager)
            mBufferManager->resetBuffer( mCurrentPos, count() );
        mSupressBM = false;
        mResetNeeded = false;
        endResetModel();
    }
    mResetNeeded = false;
}

void HgCacheProxyModel::releaseAll()
{
    if ( mDataProviderModel ){
        QList<int> list;
        for ( int i=0; i<mDataProviderModel->rowCount(); i++){
            list.append(i);
        }
        mDataProviderModel->release(list, true); //will quietly replace all icons with default ones,
    }
}

void HgCacheProxyModel::setBufferPosition(int pos) const
{
    if (!mSupressBM && mBufferManager){
        TX_LOG_ARGS(QString("idx:%1 ").arg(pos) );
        if (mCurrentPos!=pos){
            mCurrentPos = pos;
            mBufferManager->setPosition(mCurrentPos);
        }
    }
}

void HgCacheProxyModel::sourceReset()
{
    mSupressBM = true;
    mResetNeeded = true;
    releaseAll();
    if (mBufferManager)
        mBufferManager->resetBuffer( 0, count() );
    mSupressBM = false;
    mResetNeeded = false;
    endResetModel();
}

void HgCacheProxyModel::sourceAboutToBeReset()
{
    mSupressBM = true;
    mResetNeeded = true;
    beginResetModel();
}

void HgCacheProxyModel::sourceRowsInserted(const QModelIndex &source_parent, int start, int end)
{
    Q_UNUSED(source_parent);
    Q_UNUSED(end);
    if (mBufferManager)
        mBufferManager->itemCountChanged(start, false, count());
}

void HgCacheProxyModel::sourceRowsRemoved(const QModelIndex &source_parent, int start, int end)
{
    Q_UNUSED(source_parent);
    Q_UNUSED(end);    
    if (mBufferManager)
        mBufferManager->itemCountChanged(start, true, count());   
}

void HgCacheProxyModel::dataUpdated(QModelIndex from, QModelIndex to)
{
    dataUpdated(from.row(), to.row());
}

Q_DECL_EXPORT void HgCacheProxyModel::dataUpdated(int from, int to)
{
    TX_LOG_ARGS(QString("from:%1 to:%2").arg(from).arg(to));
    emit dataChanged(index(mapFromDataProviderIndex(from),0), index ( mapFromDataProviderIndex(to),0) );
}
//eof
