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

#include <HbIcon>
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
      mImageType(ImageTypeNone),
//      mDefaultIcon((":/images/default.svg")),
      mUseLowResImages(false),
      mBufferManager(0),
      mSilentDataFetch(false),
      mWrapper( new ThumbnailManager() ),
      mThumbnailRequestPending(false),
      mThumbnailRequestIndex(-1),
      mThumbnailRequestID(-1)
{
    FUNC_LOG;
    mWrapper->setQualityPreference( ThumbnailManager::OptimizeForPerformance );

    mWrapper->setProperty("qimageSupport","true");

    QObject::connect( mWrapper, SIGNAL(thumbnailReady( QPixmap , void* , int, int ) ),
                      this, SLOT( thumbnailReady( QPixmap , void* , int , int )));

    init();
}

/*!
 Destructs the collection data model.
 */
HgWidgetTestDataModel::~HgWidgetTestDataModel()
{
    FUNC_LOG;

    disconnect( mWrapper, SIGNAL(thumbnailReady( QPixmap , void* , int , int )), this, SLOT(thumbnailReady( QPixmap , void* , int , int )) );
    delete mWrapper;
    delete mBufferManager;
}

void HgWidgetTestDataModel::setThumbnailSize(ThumbnailManager::ThumbnailSize size)
{
    mWrapper->setThumbnailSize(size);
}

void HgWidgetTestDataModel::timeOut()
{
    for (int i = 0; i < mFileInfoList.size(); ++i){
        QFileInfo fileInfo = mFileInfoList.at(i);
        if (fileInfo.isFile()){
            QString s = fileInfo.filePath();
            if (s.indexOf(QString(".jpg"),0,Qt::CaseInsensitive)>0){
                mFiles.append(s);
                mImages.append(QImage());
                mPixmaps.append(QPixmap());
                mVisibility.append(true);
            }
        }
    }

    if (mBufferManager) {
        mBufferManager->resetBuffer(0,mFiles.count());
    }            
    
    if (mFiles.count()>0) {
        // photo's behaves this way so for testing purposes lets follow their practice.
        beginInsertRows(QModelIndex(), 0, mFiles.count()-1);
        endInsertRows();
    }
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

    mFileInfoList = dir.entryInfoList();
    QPixmap pixmap(":/images/default.svg");
    mDefaultPixmap = pixmap;
    if (!pixmap.isNull()){
        mQIcon = QIcon(pixmap);
        if (!mQIcon.isNull()){
            mHbIcon = HbIcon(mQIcon);
        }
    }
    
    QTimer::singleShot(0, this, SLOT(timeOut()));
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


QVariant HgWidgetTestDataModel::silentData(const QModelIndex &index, int role)
{
    mSilentDataFetch = true;
    QVariant variant = data(index,role);
    mSilentDataFetch = false;
    return variant;
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

    if( row < 0 && row >= mFiles.count() ){
        return returnValue;
    }

    if( mBufferManager && !mSilentDataFetch)
        mBufferManager->setPosition( row );

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
                if (mFiles.at(row).isEmpty())
                {
                    
                    switch (mImageType)
                    {
                    case ImageTypeHbIcon:
                        returnValue = mHbIcon;
                        break;
                    case ImageTypeQImage:
                        returnValue = mDefaultIcon;
                        break;
                    case ImageTypeQIcon:
                        returnValue = mQIcon;
                        break;
                    case ImageTypeQPixmap:
                        returnValue = mDefaultPixmap;
                        break;
                    }
                
                }
                else
                {
                    switch (mImageType)
                    {
                    case ImageTypeQImage:
                    {
                        QImage image = mImages.at(row);
                        if (image.isNull())
                        {
                            returnValue = mDefaultIcon;
                        }
                        else
                        {
                            if (mUseLowResImages)
                            {
                                QSize size = image.size();
                                image = image.scaled(QSize(size.width()/4, size.height()/4));
                                returnValue = image;
                            }
                            else
                            {
                                returnValue = image;
                            }
                        }
                    }
                    break;
                    case ImageTypeQPixmap:
                    {
                        QPixmap pixmap = mPixmaps.at(row);
                        if (pixmap.isNull())
                        {
                            returnValue = mDefaultPixmap;
                        }
                        else
                        {
                            returnValue = pixmap;
                        }
                    }
                    break;
                    case ImageTypeHbIcon:
                    {
                    QPixmap pixmap = mPixmaps.at(row);
                    if (!pixmap.isNull()) {
                        QIcon qicon(mPixmaps.at(row));
                        if (!qicon.isNull()){
                            returnValue = HbIcon(qicon);
                        }else {
                            returnValue = mHbIcon;
                        }
                    }
                    }
                    break;
                    case ImageTypeQIcon:
                    {
                        returnValue = mQIcon;
                    }
                    break;
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
                QImage icon = mImages.at(row);
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

    // TODO FIX

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

void HgWidgetTestDataModel::reset()
{
    emit beginResetModel();
    mFiles.removeAt(0);
    if (mImageType == ImageTypeQPixmap)
    {
        mPixmaps.removeAt(0);
    }
    else
        mImages.removeAt(0);
    emit endResetModel();
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

HgTestImageType HgWidgetTestDataModel::imageDataType() const
{
    return mImageType;
}

void HgWidgetTestDataModel::setImageDataType(HgTestImageType type)
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

void HgWidgetTestDataModel::getNextThumbnail()
{
    if ( !mThumbnailRequestPending && mWaitingThumbnails.count()){
        QString image = mWaitingThumbnails.takeFirst();
        int index = mFiles.indexOf(image);
        int *clientData = new int(index);
        mThumbnailRequestID = mWrapper->getThumbnail(image, clientData, -1);
        mThumbnailRequestIndex = index;
        mThumbnailRequestPending = true;
    }
}

void HgWidgetTestDataModel::setBuffer(int buffer, int treshhold)
{
    mWaitingThumbnails.clear();

    if (mThumbnailRequestID!=-1){
        mWrapper->cancelRequest(mThumbnailRequestID);
    }

    mThumbnailRequestID = -1;
    mThumbnailRequestIndex = -1;
    mThumbnailRequestPending = false;

    delete mBufferManager;
    mBufferManager = 0;
    mBufferManager = new BufferManager(this, buffer, treshhold, 0, mFiles.count());
    if (mImageType == ImageTypeQPixmap || mImageType == ImageTypeHbIcon)
    {
        for (int i = 0; i<mPixmaps.count();i++) {
            mPixmaps.replace(i, QPixmap());
        }        

    }
    else
    {
        for (int i = 0; i<mImages.count();i++) {
            mImages.replace(i, QImage());
        }

    }
}

void HgWidgetTestDataModel::release(int start, int end)
{
    bool requestNew = false;
    int first = (start<=end)?start:end;
    if (first<0)
        first =0;

    int last = end>=start?end:start;
    if (last>=mFiles.count())
        last=mFiles.count()-1;


    for (int i(first); i<=last; i++){
        mWaitingThumbnails.removeOne(mFiles.at(i));
        if (mThumbnailRequestIndex==i && mThumbnailRequestID!=-1){
            mWrapper->cancelRequest(mThumbnailRequestID);
            requestNew = true;
        }
        if (mImageType == ImageTypeQPixmap)
        {
            mPixmaps.replace(i, QPixmap());
        }
        else
        {
            mImages.replace(i,QImage());
        }
    }

    if (requestNew){
        mThumbnailRequestIndex = -1;
        mThumbnailRequestID = -1;
        mThumbnailRequestPending = false;
        getNextThumbnail();
    }
}

void HgWidgetTestDataModel::request(int start, int end, requestsOrder order)
{
    int first = start;
    int last = end;

    if (first<0)
        first =0;

    if (last>=mFiles.count())
        last=mFiles.count()-1;

    if (order == ascending){
        for (int i(first); i<=last; i++){
            mWaitingThumbnails.append(mFiles.at(i));
        }
    } else if (order ==descending){
        for (int i(last); i>=first; i--){
            mWaitingThumbnails.append(mFiles.at(i));
        }
    }

    getNextThumbnail();
}

void HgWidgetTestDataModel::thumbnailReady( QPixmap pixmap, void* data, int /*id*/, int error )
{
    Q_UNUSED(data);
    
    if (!error && !pixmap.isNull() ){
//        int idx = reinterpret_cast<int>(data);
        if (mImageType == ImageTypeQPixmap || mImageType == ImageTypeHbIcon)
        {
            mPixmaps.replace(mThumbnailRequestIndex, pixmap);
        }
        else {
            mImages.replace(mThumbnailRequestIndex,pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied));//.convertToFormat(QImage::Format_RGB16));            
        }
        QModelIndex modelIndex = QAbstractItemModel::createIndex(mThumbnailRequestIndex, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
    mThumbnailRequestIndex = -1;
    mThumbnailRequestID = -1;
    mThumbnailRequestPending = false;
    getNextThumbnail();
}

