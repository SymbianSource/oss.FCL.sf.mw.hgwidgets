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

#ifndef HGWIDGETTESTDATAMODEL_H
#define HGWIDGETTESTDATAMODEL_H

#include <QAbstractListModel>
#include <QItemSelection>
#include <QStringList>
#include <QFileInfoList>
#include <hbicon>
#include <thumbnailmanager_qt.h>
#include <QImage>
#include <QList>
#include "buffermanager.h"
#include "hgtestdefs.h"

class HgWidgetTestAlbumArtManager;
class ThumbnailManager;


class HgWidgetTestDataModel : public QAbstractListModel, public AbstractDataProvider
{
    Q_OBJECT

public:

    explicit HgWidgetTestDataModel(QObject *parent=0);
    virtual ~HgWidgetTestDataModel();

    void setThumbnailSize(ThumbnailManager::ThumbnailSize size);
    void setBuffer(int buffer, int treshhold);
    
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    QVariant silentData(const QModelIndex &index, int role=Qt::DisplayRole);
    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

    void refreshModel();

    void remove(const QItemSelection &selection);
    void move(const QItemSelection &selection, const QModelIndex &target);
    void add(const QModelIndex &target, int count);
    void reset();
    
    HgTestImageType imageDataType() const;
    void setImageDataType(HgTestImageType type);

    void enableLowResImages(bool enabled);
    bool lowResImagesEnabled() const;

public slots:
    void thumbnailReady( QPixmap , void* , int, int ); 
    
private:

    void init();
    void getNextThumbnail();

public slots:

    void updateAlbumArt( int index );
    void albumCacheReady();
    void timeOut();
    
private:
    
    void release(int start, int end);
    void request(int start, int end, requestsOrder order);
    
private:

    bool                            mCachingInProgress;
    QStringList                     mFiles;
    HgTestImageType                 mImageType;
    HbIcon                          mHbIcon;
    QIcon                           mQIcon;
    QImage                          mDefaultIcon;
    QList<bool>                     mVisibility;
    bool                            mUseLowResImages;
    QList<QImage>                   mImages;
    BufferManager                   *mBufferManager;
    QList<QPixmap>                  mPixmaps;
    QPixmap                         mDefaultPixmap;
    bool                            mSilentDataFetch;

    ThumbnailManager* mWrapper;
    QStringList mWaitingThumbnails;
    bool mThumbnailRequestPending;
    int mThumbnailRequestIndex;
    int mThumbnailRequestID;

    QFileInfoList mFileInfoList;
};

#endif // HgWidgetTestDataModel_H

