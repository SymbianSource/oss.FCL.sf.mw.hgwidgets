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

#ifndef HGWIDGETITEM_H
#define HGWIDGETITEM_H

#include <QString>
#include <QPixmap>
#include <QModelIndex>
#include <QObject>

class HgQuadRenderer;
class HgImage;

class HgWidgetItem
{
public:

    HgWidgetItem(HgQuadRenderer *renderer);
    HgWidgetItem(HgQuadRenderer *renderer, QImage image, QString title, QString description );
    virtual ~HgWidgetItem();

    void setImage(const QImage &image);
    void setPixmap(const QPixmap& pixmap);

    void setTitle(QString title);
    QString title() const;

    const HgImage* image() const;

    void setDescription( QString description );
    QString description() const;

    void setModelIndex( const QModelIndex& index);
    QModelIndex modelIndex() const;

    bool validData() const;
    bool updateItemData();
    void releaseItemData();

    bool visibility() const;
    void setVisibility(bool visible);

    void enableReflection(bool enabled);
    
private:

    QString     mTitle;
    QString     mDescription;
    QModelIndex mModelIndex;
    bool        mValidData;
    HgImage*    mHgImage;
    HgQuadRenderer* mRenderer;
    bool mVisibility;
    bool mReflectionEnabled; 
    
    Q_DISABLE_COPY(HgWidgetItem)
};

#endif  //HGWIDGETITEM_H

