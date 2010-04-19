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
#include <hbicon>
#include <thumbnailmanager_qt.h>

class HgWidgetTestAlbumArtManager;


class HgWidgetTestDataModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit HgWidgetTestDataModel(QObject *parent=0);
    virtual ~HgWidgetTestDataModel();

    void setThumbnailSize(ThumbnailManager::ThumbnailSize size);

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

    void refreshModel();

    void remove(const QItemSelection &selection);
    void move(const QItemSelection &selection, const QModelIndex &target);
    void add(const QModelIndex &target, int count);

    enum ImageType{TypeQIcon, TypeHbIcon, TypeQImage};
    
    void setImageDataType(ImageType type);
    
    void enableLowResImages(bool enabled);
    bool lowResImagesEnabled() const;
    
private:

    void init();

public slots:

    void updateAlbumArt( int index );
    void albumCacheReady();

private:

	HgWidgetTestAlbumArtManager     *mAlbumArtManager;  // Own
    bool                            mCachingInProgress;
    QStringList                     mFiles;
    ImageType                       mImageType;
    HbIcon                          mHbIcon;
    QIcon                           mQIcon;
    QImage                          mDefaultIcon;
    QList<bool>                     mVisibility;
    bool                            mUseLowResImages;
};

#endif // HgWidgetTestDataModel_H

