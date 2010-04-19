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

#include <QBrush>
#include <QColor>
#include <QtCore>

#include <hbicon.h>
#include <hbnamespace.h>
#include <hgwidgets/hgwidgets.h>

#include "hgwidgettestdatamodel.h"
#include "hgwidgettestalbumartmanager.h"
#include "trace.h"

typedef QPair<int, int> Range;
typedef QList<Range > RangeList;

/*!
    \class HgWidgetTestDataModel
    \brief Music Player collection data model.

    Collection data model implements the interface specified by HbAbstractDataModel,
    which defines the standard interface that item models must use to be able to
    interoperate with other components in the model/view architecture.

    Every item of data that can be accessed via a model has an associated model
    index.

    Each item has a number of data elements associated with it and they can be
    retrieved by specifying a role (see Qt::ItemDataRole) to the model's data
    returned by itemData() function.

    \sa HbAbstractDataModel
*/

/*!
 Constructs the collection data model.
 */
HgWidgetTestDataModel::HgWidgetTestDataModel(QObject *parent)
    : QAbstractListModel(parent),
      mCachingInProgress(false),
      mImageType(TypeQImage),	  
      mDefaultIcon((":/images/default.svg")),
      mUseLowResImages(false)
{
    FUNC_LOG;

    mAlbumArtManager = new HgWidgetTestAlbumArtManager;
    connect( mAlbumArtManager, SIGNAL(albumArtReady(int)), this, SLOT(updateAlbumArt(int)) );
    connect( mAlbumArtManager, SIGNAL(albumCacheReady()), this, SLOT(albumCacheReady()) );
    init();
}

/*!
 Destructs the collection data model.
 */
HgWidgetTestDataModel::~HgWidgetTestDataModel()
{
    FUNC_LOG;

    disconnect( mAlbumArtManager, SIGNAL(albumArtReady(int)), this, SLOT(updateAlbumArt(int)) );
    disconnect( mAlbumArtManager, SIGNAL(albumCacheReady()), this, SLOT(albumCacheReady()) );
    delete mAlbumArtManager;
}

void HgWidgetTestDataModel::setThumbnailSize(ThumbnailManager::ThumbnailSize size)
{
    mAlbumArtManager->setThumbnailSize(size);
}

void HgWidgetTestDataModel::init()
{
    FUNC_LOG;

    // Read all .jpg image paths from the c:/data/images folder
    QDir dir;
    dir.setFilter(QDir::Files | QDir:: Dirs);
#ifdef __WINS__
    dir.setPath(QString("c:/data/images"));
#else
    dir.setPath(QString("f:/data/images"));
#endif

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i){
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isFile()){
            QString s = fileInfo.filePath();
            if (s.indexOf(QString(".jpg"),0,Qt::CaseInsensitive)>0){
                mFiles.append(s);
                mVisibility.append(true);
            }
        }
    }

    QPixmap pixmap(":/images/default.svg");
    if (!pixmap.isNull()){
        mQIcon = QIcon(pixmap);
        if (!mQIcon.isNull()){
            mHbIcon = HbIcon(mQIcon);    
        }
    }
}

/*!
 Returns the number of rows under the given \a parent.

 View will request for the row count immediately after a model is set.
 To prevent it from reading data while caching the album art for the first
 screen, return row count as zero.

 \reimp
 */
int HgWidgetTestDataModel::rowCount( const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return mFiles.count();
}

/*!
 Returns the data stored for the item referred to by the \a index.

 \reimp
 */
QVariant HgWidgetTestDataModel::data(const QModelIndex &index, int role) const
{
    QVariant returnValue = QVariant();
    if ( !index.isValid() ) {
        return returnValue;
    }

    int row = index.row();

    if( row >= mFiles.count() ){
        return returnValue;
    }

    switch ( role )
        {
    case HgWidget::HgVisibilityRole:
    {
        returnValue = mVisibility[index.row()];
    } break;
        case Qt::DisplayRole:
            {
            QStringList texts;
            QString text( "Primary " );
            text.append(QString::number(row));
            texts << text;
            text = "Secondary ";
            text.append(QString::number(row));
            texts << text;
            returnValue = texts;
            break;
            }
        case Qt::DecorationRole:
            {
            // INFO("Requesting model item" << row << ", " << mFiles.at(row));
            if (mFiles.at(row).isEmpty()) {
                returnValue = mDefaultIcon;
            }
            else {
                QImage icon = mAlbumArtManager->albumArt(mFiles.at(row), row);
                if ( !icon.isNull() )
                    {
                    if (mUseLowResImages) {                        
                        QSize size = icon.size();
                        icon = icon.scaled(QSize(size.width()/4, size.height()/4));
                    }

                    switch(mImageType)
                        {
                        case TypeHbIcon:
                            {
                            returnValue = mHbIcon;
                            break;
                            }
                        case TypeQImage:
                            {
                            returnValue = icon;
                            break;
                            }
                        case TypeQIcon:
                            {
                            returnValue = mQIcon;
                            break;
                            }
                        default:
                            break;                    
                        }
                
                    }
                else
                    {
                    returnValue = mDefaultIcon;
                    }
                }
            break;
            }
        case Hb::IndexFeedbackRole:
            {
            returnValue = QString::number(row);
            break;
            }
        case Qt::BackgroundRole:
            {
            if ( (index.row() % 2) == 0 ) {
                QColor color(211,211,211,127);
                QBrush brush(color);
                returnValue = brush;
            }
            else {
                QColor color(255,250,250,127);
                QBrush brush(color);
                returnValue = brush;
            }
            break;
            }
            
        case (Qt::UserRole+2):
            {
                QImage icon = mAlbumArtManager->albumArt(mFiles.at(row), row);
                if (!icon.isNull())
                {
                    returnValue = icon;
                }
            } break;
        }

    return returnValue;
}

/*!
 Must be called when data has changed and model needs to be refreshed
 to reflect the new data.
 */
void HgWidgetTestDataModel::refreshModel()
{
    // Cancel all outstanding album art request first, then reset the model.
    mAlbumArtManager->cancel();

    // Before providing the new data to the view (list, grid, etc.), we want
    // to make sure that we have enough album arts for the first screen.
/*    mFiles.count() = mCollectionData->count();
    if ( mFiles.count() > 0 ) {
        int initCount = ( mFiles.count() > KInitCacheSize ) ? KInitCacheSize : mFiles.count();
        QStringList albumArtList;
        QString albumArtUri;
        for ( int i = 0; i < initCount; i++ ) {
            albumArtUri = mCollectionData->itemData(i, MpMpxCollectionData::AlbumArtUri);
            if ( !albumArtUri.isEmpty() ) {
                 albumArtList << albumArtUri;
            }
        }
        mCachingInProgress = mAlbumArtManager->cacheAlbumArt(albumArtList);
        if ( !mCachingInProgress ) {
            reset();
        }
    }
    else {
        reset();
    }
    */
}

/*!
 Remove items from model (do not actually delete them).
 */
void HgWidgetTestDataModel::remove(const QItemSelection &selection)
{
    FUNC_LOG;

    QModelIndexList modelIndexes = selection.indexes();
    int removeCount = modelIndexes.count();
    int originalItemCount = mFiles.count();
    if (originalItemCount-removeCount > 0) {
        RangeList removeRanges;
        qSort(modelIndexes);
        while (!modelIndexes.isEmpty()) {
            QModelIndexList::iterator i = modelIndexes.begin();
            QModelIndexList::iterator start = i;
            int lastRow = i->row();
            while (++i != modelIndexes.end() && i->row() == lastRow+1) {
                lastRow++;
            }
            removeRanges.append(Range(start->row(), lastRow));
            modelIndexes.erase(start, i);
        }

        // Work backwards to keep the indexes consistent
        for (int i = removeRanges.count()-1; i >= 0; i--) {
            Range range = removeRanges.at(i);
            beginRemoveRows(QModelIndex(), range.first, range.second);
            for (int j = range.second; j >= range.first; j--) {
                INFO("Removing model item" << j);
                mFiles.removeAt(j);
            }
            endRemoveRows();
        }
    }
    else if (originalItemCount-removeCount == 0) {
        beginRemoveRows(QModelIndex(), 0, originalItemCount-1);
        mFiles.clear();
        endRemoveRows();
    }
}

/*!
 Move items to the target index in the model. The selection should be contiguous.
 */
void HgWidgetTestDataModel::move(const QItemSelection &selection, const QModelIndex &target)
{
    FUNC_LOG;

    QModelIndexList modelIndexes = selection.indexes();

    if (modelIndexes.count() > 0 && target.isValid()) {
        int first = modelIndexes.front().row();
        int last = modelIndexes.back().row();
        int targetRow = target.row();
        INFO("Move indexes" << first << "-" << last << "to" << targetRow);
        if (targetRow < first) {
            beginMoveRows(QModelIndex(), first, last, QModelIndex(), targetRow);
            for (int i = 0; i <= last-first; i++) {
                mFiles.move(first+i, targetRow+i);
            }
            endMoveRows();
        }
        else if (targetRow > last) {
            beginMoveRows(QModelIndex(), first, last, QModelIndex(), targetRow);
            for (int i = 0; i <= last-first; i++) {
                mFiles.move(last-i, targetRow);
            }
            endMoveRows();
        }
    }
}

/*!
 Add count dummy items at the target index in the model.
 */
void HgWidgetTestDataModel::add(const QModelIndex &target, int count)
{
    FUNC_LOG;

    if (target.isValid()) {
        beginInsertRows(QModelIndex(), target.row(), target.row()+count-1);
        for (int i = 0; i < count; i++) {
            mFiles.insert(target.row(), QString());
            mVisibility.insert(target.row(), true);
        }
        endInsertRows();
    }
}

/*!
 Slot to be called when album art for the \a index needs to be updated.
 */
void HgWidgetTestDataModel::updateAlbumArt( int index )
{
    if ( index >= 0 && index < mFiles.count() ) {
        QModelIndex modelIndex = QAbstractItemModel::createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}

/*!
 Slot to be called when album art cache is ready.
 */
void HgWidgetTestDataModel::albumCacheReady()
{
    if ( mCachingInProgress ) {
        mCachingInProgress = false;
        reset();
    }
}

void HgWidgetTestDataModel::setImageDataType(ImageType type)
{
    mImageType = type;
}

bool HgWidgetTestDataModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role == HgWidget::HgVisibilityRole)
    {
        mVisibility[index.row()] = value.toBool();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void HgWidgetTestDataModel::enableLowResImages(bool enabled) {

    mUseLowResImages = enabled;
}

bool HgWidgetTestDataModel::lowResImagesEnabled() const {

    return mUseLowResImages;
}

