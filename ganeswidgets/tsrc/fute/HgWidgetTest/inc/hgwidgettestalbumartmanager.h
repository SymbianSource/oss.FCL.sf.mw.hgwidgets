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


#ifndef HGWIDGETTESTALBUMARTMANAGER_H
#define HGWIDGETTESTALBUMARTMANAGER_H

#include <QObject>
#include <QHash>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QBitmap>
#include <HbIcon>
#include <thumbnailmanager_qt.h>

class HgWidgetTestAlbumArtManager : public QObject
{
    Q_OBJECT

public:

    explicit HgWidgetTestAlbumArtManager(QObject *parent=0);
    virtual ~HgWidgetTestAlbumArtManager();

    void setThumbnailSize(ThumbnailManager::ThumbnailSize size);

    QImage albumArt( const QString &albumArtUri, int index );
    bool cacheAlbumArt( const QStringList albumArtList );
    void cancel();

signals:

    void albumArtReady( int index );
    void albumCacheReady();

public slots:

    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    void thumbnailReady( const QImage& image, void* data, int id, int error );
    void executeNext();

private:

    void applyMask( QPixmap &pixmap );

private:

    ThumbnailManager                *mThumbnailManager;
    QHash<QString, QImage>          mImageCache;
    QMap<int, QString>              mTnmReqMap;
    bool                            mCachingInProgress;

    QQueue< QPair<QString, int> >   mRequestQueue;
    int                             mRequestCount;
    QBitmap                         mMask;

};

#endif // HGWIDGETTESTALBUMARTMANAGER_H

