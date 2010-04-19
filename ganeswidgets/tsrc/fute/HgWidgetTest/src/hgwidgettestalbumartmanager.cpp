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

#include <thumbnailmanager_qt.h>
#include <QTimer>

#include "hgwidgettestalbumartmanager.h"

const int KMaxThumbnailReq = 1;

/*!
    \class HgWidgetTestAlbumArtManager
    \brief Music Player collection album art manager.

    Collection album art manager provides access to album art needed for
    display in certain collection views. It hides interface to the thumbnail
    manager and also implements a caching mechanism for performance reasons.
*/

/*!
    \fn void albumArtReady( int index )

    This signal is emitted when album art for \a index is ready.

    \sa getAlbumArt()
 */

/*!
    \fn void albumCacheReady()

    This signal is emitted when album art cache is ready.

    \sa cacheAlbumArt()
 */

/*!
 Constructs the album art manager.
 */
HgWidgetTestAlbumArtManager::HgWidgetTestAlbumArtManager(QObject *parent)
    : QObject(parent),
      mCachingInProgress(false),
      mRequestCount(0),
      mMask()
{
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForQuality);
//    mThumbnailManager->setMode(ThumbnailManager::CreateQImages);
    QPixmap qpixmap(":/icons/teardrop_mask.png");
    mMask = QBitmap(qpixmap);

//    connect( mThumbnailManager, SIGNAL(thumbnailReady(QImage, void *, int, int)),
//             this, SLOT(thumbnailReady(QImage, void *, int, int)) );
    connect( mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(thumbnailReady(QPixmap, void *, int, int)) );
}

/*!
 Destructs the album art manager.
 */
HgWidgetTestAlbumArtManager::~HgWidgetTestAlbumArtManager()
{
    cancel();
    mImageCache.clear();
}

void HgWidgetTestAlbumArtManager::setThumbnailSize(ThumbnailManager::ThumbnailSize size)
{
    cancel();
    mImageCache.clear();
    mThumbnailManager->setThumbnailSize(size);
}

/*!
 Returns the album art for the given \a albumArtUri. If the album art is not
 available in its cache, an asynchronous request is made to the thumbnail manager
 and a null icon is returned.

 \sa signal albumArtReady
 */
QImage HgWidgetTestAlbumArtManager::albumArt( const QString& albumArtUri, int index )
{
    QImage icon;
    if ( mImageCache.contains(albumArtUri) ) {
        icon = mImageCache.value(albumArtUri);
        // If you want to cache all images disable this line. However, with large 
        // amount of items OOM will occure.
//        mImageCache.remove(albumArtUri);
    }
    else {
        if ( mRequestCount < KMaxThumbnailReq ) {
            // Using negative index as priority will ensure that thumbnail requests
            // are processed in the order they were requested.
            int *clientData = new int(index);
            int reqId = mThumbnailManager->getThumbnail( albumArtUri, clientData, -1 );
            if ( reqId != -1 ) {
                mTnmReqMap.insert( reqId, albumArtUri );
                mRequestCount++;
            }
            else {
            }
        }
        else {
            mRequestQueue.enqueue( qMakePair(albumArtUri, index) );
        }
    }
    return icon;
}

/*!
 Request to cache the album art for the items specified in \a albumArtList.
 Returns 'true' if caching is started. If all items already exist in cache,
 'false' is returned.

 \sa signal albumCacheReady
 */
bool HgWidgetTestAlbumArtManager::cacheAlbumArt( const QStringList albumArtList )
{
    int allAvailable = true;
    if ( !albumArtList.empty() ) {
        QString albumArtUri;
        int reqId;
        QStringListIterator iter(albumArtList);
        while ( iter.hasNext() ) {
            albumArtUri = iter.next();
            if ( !mImageCache.contains(albumArtUri) ) {
                reqId = mThumbnailManager->getThumbnail( albumArtUri );
                if ( reqId != -1 ) {
                    mTnmReqMap.insert( reqId, albumArtUri );
                    mRequestCount++;
                    allAvailable = false;
                }
                else {
                }
            }
        }
    }

    if ( allAvailable ) {
        return false;
    }
    else {
        mCachingInProgress = true;
        return true;
    }
}

/*!
 Cancels all outstanding album art requests.

 \sa getAlbumArt, cacheAlbumArt
 */
void HgWidgetTestAlbumArtManager::cancel()
{
    if ( !mTnmReqMap.empty() ) {
        QMapIterator<int, QString> iter(mTnmReqMap);
        while ( iter.hasNext() ) {
            iter.next();
            bool result = mThumbnailManager->cancelRequest(iter.key());
        }
    }
    mTnmReqMap.clear();
    mRequestQueue.clear();
    mRequestCount = 0;
    mCachingInProgress = false;
}


/*!
 Slot to be called when thumbnail bitmap generation or loading is complete.
 */
void HgWidgetTestAlbumArtManager::thumbnailReady( const QPixmap& pixmap, void *data, int id, int error )
{
    // Find the index
    if ( mTnmReqMap.contains(id) ) {
        // Remove the request whether it completed successfully or with error.
        QString albumArtUri = mTnmReqMap[id];
        mTnmReqMap.remove( id );
        mRequestCount--;

        if ( mCachingInProgress ) {
            if ( error == 0 ) {
//                QPixmap art(pixmap);
//                applyMask(art);
//                QIcon qicon(art);
//                mImageCache.insert(albumArtUri, pixmap);
            }
            else {
            }
            if ( mTnmReqMap.empty() ) {
                mCachingInProgress = false;
                emit albumCacheReady();
                return;
            }
        }
        else {
            if ( error == 0 ) {
                int *clientData = (int *)data;
                int index = *clientData;
                delete clientData;
                QImage image = pixmap.toImage().convertToFormat(QImage::Format_RGB16);
                mImageCache.insert(albumArtUri, image);
                emit albumArtReady(index);
                QTimer::singleShot(0, this, SLOT(executeNext()));
            }
            else {
            }
        }
    }
}

/*!
 Slot to be called when thumbnail bitmap generation or loading is complete.
 */
void HgWidgetTestAlbumArtManager::thumbnailReady( const QImage& image, void *data, int id, int error )
{
    // Find the index
    if ( mTnmReqMap.contains(id) ) {
        // Remove the request whether it completed successfully or with error.
        QString albumArtUri = mTnmReqMap[id];
        mTnmReqMap.remove( id );
        mRequestCount--;

        if ( mCachingInProgress ) {
            if ( error == 0 ) {
//                QPixmap art(pixmap);
//                applyMask(art);
//                QIcon qicon(art);
                mImageCache.insert(albumArtUri, image);
            }
            else {
            }
            if ( mTnmReqMap.empty() ) {
                mCachingInProgress = false;
                emit albumCacheReady();
                return;
            }
        }
        else {
            if ( error == 0 ) {
                int *clientData = (int *)data;
                int index = *clientData;
                delete clientData;
                QSize size = image.size();
                QImage newImage;
                newImage = image.copy();
                mImageCache.insert(albumArtUri, newImage);
                emit albumArtReady(index);
                QTimer::singleShot(0, this, SLOT(executeNext()));
            }
            else {
            }
        }
    }
}


/*!
 Applies mask on the thumbnail.
 */
void HgWidgetTestAlbumArtManager::applyMask( QPixmap& pixmap )
{
    pixmap.setMask(mMask);
}

void HgWidgetTestAlbumArtManager::executeNext()
{
    // Check to see if any request is pending in the queue
    while ( !mRequestQueue.isEmpty()
            && (mRequestCount < KMaxThumbnailReq) ) {
        QPair<QString, int> req = mRequestQueue.dequeue();
        QString albumArtUri = req.first;
        int index = req.second;

        // Using negative index as priority will ensure that thumbnail requests
        // are processed in the order they were requested.
        int *clientData = new int(index);
        int reqId = mThumbnailManager->getThumbnail( albumArtUri, clientData, -1 );
        if ( reqId != -1 ) {
            mTnmReqMap.insert( reqId, albumArtUri );
            mRequestCount++;
        }
        else {
        }
    }

}
