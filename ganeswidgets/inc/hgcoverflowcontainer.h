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
#include "HgContainer.h"

class HbLabel;

class HgCoverflowContainer: public HgContainer
{
    Q_OBJECT
    Q_DISABLE_COPY(HgCoverflowContainer)

public:
    explicit HgCoverflowContainer(QGraphicsItem* parent = 0);
    virtual ~HgCoverflowContainer();

    // events
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void resizeEvent(QGraphicsSceneResizeEvent *event);

    // from HgContainer
    virtual HgMediaWallRenderer* createRenderer(Qt::Orientation scrollDirection);
    virtual qreal getCameraDistance(qreal springVelocity);
    virtual qreal getCameraRotationY(qreal springVelocity);
    virtual void handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void onScrollPositionChanged(qreal pos);
    virtual void handleCurrentChanged(const QModelIndex & current);
    virtual void itemDataChanged(const int &firstIndex, const int &lastIndex);
    virtual void scrollToPosition(const QPointF& pos, bool animate);

    virtual QSizeF getAutoItemSize() const;
    virtual QSizeF getAutoItemSpacing() const;
    virtual void updateItemSizeAndSpacing();
    
    void setTitlePosition(HgMediawall::LabelPosition position);
    HgMediawall::LabelPosition titlePosition() const;
    void setDescriptionPosition(HgMediawall::LabelPosition position);
    HgMediawall::LabelPosition descriptionPosition() const;
    void setTitleFontSpec(const HbFontSpec &fontSpec);
    HbFontSpec titleFontSpec() const;
    void setDescriptionFontSpec(const HbFontSpec &fontSpec);
    HbFontSpec descriptionFontSpec() const;

    void setFrontItemPositionDelta(const QPointF& position);
    QPointF frontItemPositionDelta() const;

    void enableReflections(bool enabled);
    bool reflectionsEnabled() const;

signals:

    void animationAboutToEnd(const QModelIndex& targetIndex);

private: // From HgContainer
    void setDefaultImage(QImage defaultImage);

private:

    void updateLabels(int itemIndex);

    void updatePositions();
    
    void calculatePositions();    
    void positionLabels();
    void onScrollingStarted();
    void onScrollingEnded();

private:
    HbLabel                     *mTitleLabel;
    HbLabel                     *mDescriptionLabel;
    HgMediawall::LabelPosition  mTitlePosition;
    HgMediawall::LabelPosition  mDescriptionPosition;
    int                         mPrevPos;
    qreal                       mAspectRatio;
    QSizeF                      mAutoSize;
    bool                        mAnimationAboutToEndReacted;
};

#endif
