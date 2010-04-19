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
#include "hgwidgetitem.h"
#include "hgquadrenderer.h"
#include <hgwidgets/hgwidgets.h>
#include <QStringList>
#include <HbIcon>

HgWidgetItem::HgWidgetItem(HgQuadRenderer* renderer):
mTitle(""),
mDescription(""),
mValidData(false),
mHgImage(NULL),
mRenderer(renderer)
{
}

HgWidgetItem::HgWidgetItem(HgQuadRenderer* renderer, QImage image, QString title, QString description ) :
mTitle(""),
mDescription(""),
mValidData(false),
mHgImage(NULL),
mRenderer(renderer),
mVisibility(true)
{
    Q_UNUSED(image)
    setTitle(title);
    setDescription(description);    
}

HgWidgetItem::~HgWidgetItem()
{
    releaseItemData();
    delete mHgImage;
}

void HgWidgetItem::setImage( QImage image )
{
    if (!mHgImage)
    {
        mHgImage = mRenderer->createNativeImage();
    }
    mHgImage->setImage(image);
    if (!mVisibility)
        mHgImage->setAlpha(0);
    
}

void HgWidgetItem::setTitle( QString title )
{
    mTitle = title;
}

QString HgWidgetItem::title() const
{
    return mTitle;
}

void HgWidgetItem::setDescription( QString description )
{
    mDescription = description;
}

QString HgWidgetItem::description() const
{
    return mDescription;
}

void HgWidgetItem::setModelIndex(const QModelIndex& index)
{
    mModelIndex = index;
}

QModelIndex HgWidgetItem::modelIndex() const
{
    return mModelIndex;
}

bool HgWidgetItem::updateItemData()
{
    mValidData = false;
    if( mModelIndex.isValid() ){
        QVariant image = mModelIndex.data(Qt::DecorationRole);
        QVariant texts = mModelIndex.data(Qt::DisplayRole);

        QVariant vis = mModelIndex.data(HgWidget::HgVisibilityRole);
        if (vis.canConvert<bool>())
        {
            setVisibility(vis.toBool());
        }
        
        // Convert data to correct format if possible.
        if(image.canConvert<QImage>()){
            setImage(image.value<QImage>());
            mValidData = true;
        }
        else if(image.canConvert<HbIcon>()){
            // This is heavy operation but we need to support
            // HbIcon too.
            HbIcon hbIcon = image.value<HbIcon>();
            if (!hbIcon.isNull()){
                QIcon &qicon = hbIcon.qicon();
                QList<QSize> sizes = qicon.availableSizes();
                QSize size;
                foreach(size, sizes){
                    if (size.width() != 0 && size.height() != 0 ){
                        QPixmap pixmap = qicon.pixmap(size);
                        if (!pixmap.isNull()){
                            setImage(pixmap.toImage());
                            mValidData = true;        
                        }
                    break;
                    }
                }
            }
        }
        else if (image.canConvert<QIcon>()){
            // This is heavy operation but we need to support
            // QIcon too.
            QIcon tempIcon = image.value<QIcon>();
            QList<QSize> sizes = tempIcon.availableSizes();
            QSize size;
            foreach(size, sizes){
                if (size.width() != 0 && size.height() != 0 ){
                    QPixmap pixmap = tempIcon.pixmap(size);
                    if (!pixmap.isNull()){
                        setImage(pixmap.toImage());
                        mValidData = true;        
                    }
                break;
                }
            }
        }
        if( texts.canConvert<QStringList>() ){
            QStringList list(texts.toStringList() );
            if( list.count() >= 2 ){
                setTitle(list.at(0));
                setDescription(list.at(1));
                mValidData = true;
            }
        }
        
    }
    return mValidData;
}

void HgWidgetItem::releaseItemData()
{
    mTitle = "";
    mDescription = "";
    mValidData = false;
    if (mHgImage)
        mHgImage->releaseImage();
}

bool HgWidgetItem::validData() const
{
    return mValidData;
}

const HgImage* HgWidgetItem::image() const
{
    return mHgImage;
}

void HgWidgetItem::setVisibility(bool visibility)
{
    mVisibility = visibility;
    if (mHgImage)
    {
        if (!mVisibility)
            mHgImage->setAlpha(0);
        else
            mHgImage->setAlpha(1);
    }
}
