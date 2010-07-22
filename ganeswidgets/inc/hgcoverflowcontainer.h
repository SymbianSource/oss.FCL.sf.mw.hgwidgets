/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef HGCOVERFLOWCONTAINER_H
#define HGCOVERFLOWCONTAINER_H

#include <hgwidgets/hgmediawall.h>
#include "hgcontainer.h"

class HgCenterItemArea;

class HgCoverflowContainer: public HgContainer
{
    Q_OBJECT
    Q_DISABLE_COPY(HgCoverflowContainer)

public:
    explicit HgCoverflowContainer(QGraphicsItem* parent = 0);
    virtual ~HgCoverflowContainer();

    // events
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // from HgContainer
    virtual HgMediaWallRenderer* createRenderer(Qt::Orientation scrollDirection);
    virtual qreal getCameraDistance(qreal springVelocity);
    virtual qreal getCameraRotationY(qreal springVelocity);
    virtual void handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void onScrollPositionChanged(qreal pos);
    virtual void scrollToPosition(const QPointF& pos, bool animate);

    virtual QSizeF getAutoItemSize() const;
    virtual QSizeF getAutoItemSpacing() const;
    virtual void updateItemSizeAndSpacing();

    void setFrontItemPositionDelta(const QPointF& position);
    QPointF frontItemPositionDelta() const;

    void setFrontItemElevationFactor(qreal factor);
    
    void enableReflections(bool enabled);
    bool reflectionsEnabled() const;

    void setCenterItemArea(HgCenterItemArea *centerItemArea);

public slots:

    void updateItemSize();

signals:

    void animationAboutToEnd(const QModelIndex& targetIndex);

private:

    void calculateItemSize();
    void onScrollingStarted();
    void onScrollingEnded();

    void resizeEvent(QGraphicsSceneResizeEvent *event);
    
private:
    int              mPrevPos;
    QSizeF           mAutoSize;
    bool             mAnimationAboutToEndReacted;
    HgCenterItemArea *mCenterItemArea;
};

#endif
