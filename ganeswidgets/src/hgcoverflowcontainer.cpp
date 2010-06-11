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

#include <QGesture>
#include <QGraphicsSceneResizeEvent>
#include <QPainter>
#include <hblabel.h>
#include "hgcoverflowcontainer.h"
#include "hgmediawallrenderer.h"
#include "hgwidgetitem.h"
#include "hgcenteritemarea.h"
#include "trace.h"


static const qreal KCameraMaxYAngle(20);
static const qreal KSpringVelocityToCameraYAngleFactor(2);

HgCoverflowContainer::HgCoverflowContainer(
    QGraphicsItem* parent) : HgContainer(parent),
    mPrevPos(-1),
    mAnimationAboutToEndReacted(false),
    mCenterItemArea(0)
{
    mUserItemSize = QSize(250,250);
    mUserItemSpacing = QSize(1,1);
}

HgCoverflowContainer::~HgCoverflowContainer()
{
}

// events
void HgCoverflowContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    HgContainer::paint(painter, option, widget);
    painter->setRenderHint(QPainter::Antialiasing, false);
}

// from HgContainer
HgMediaWallRenderer* HgCoverflowContainer::createRenderer(Qt::Orientation scrollDirection)
{
    HgMediaWallRenderer* renderer = new HgMediaWallRenderer(this, scrollDirection, scrollDirection, true);
    renderer->setImageSize(mUserItemSize);
    renderer->enableCoverflowMode(true);
    renderer->setRowCount(1, renderer->getImageSize(), false);
    renderer->enableReflections(true);
    renderer->setSpacing(mUserItemSpacing);
    renderer->setFrontCoverElevationFactor(0.5);
    return renderer;
}

qreal HgCoverflowContainer::getCameraDistance(qreal springVelocity)
{
    return qAbs(springVelocity * 0.01f);
}

qreal HgCoverflowContainer::getCameraRotationY(qreal springVelocity)
{
    return qBound(-KCameraMaxYAngle, springVelocity * KSpringVelocityToCameraYAngleFactor, KCameraMaxYAngle);
}

void HgCoverflowContainer::handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(pos)

    if (qAbs(qreal(hitItemIndex) - mSpring.pos().x()) < 0.01f)
    {
        emit activated(hitItem->modelIndex());
    }
    else
    {
        mSpring.animateToPos(QPointF(hitItemIndex, 0));
    }
}

void HgCoverflowContainer::handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(hitItemIndex)

    emit longPressed(hitItem->modelIndex(), pos);
}

void HgCoverflowContainer::onScrollPositionChanged(qreal pos)
{
    HgContainer::onScrollPositionChanged(pos);

    if(!mAnimationAboutToEndReacted) {
        qreal endPos = mSpring.endPos().x();
        qreal abs = qAbs(endPos - mSpring.pos().x());

        if( abs <= 0.5f ) {
            HgWidgetItem* item = itemByIndex((int)endPos);
            if (item) {
                emit animationAboutToEnd( item->modelIndex() );
                mAnimationAboutToEndReacted = true;
            }
        }
    }

    qreal ipos = floorf(pos);
    qreal frac = pos - ipos;
    qreal p = frac > 0.5 ? ipos + 1.0f : ipos;

    if (mPrevPos != (int)p) {
        mPrevPos = (int)p;
        HgWidgetItem* item = itemByIndex((int)p);
        if (item && item->modelIndex() != mSelectionModel->currentIndex()) {
            mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
        }
    }
}

void HgCoverflowContainer::onScrollingStarted()
{
    mAnimationAboutToEndReacted = false;    //reset
}

void HgCoverflowContainer::onScrollingEnded()
{
    mAnimationAboutToEndReacted = false;    //reset
}

void HgCoverflowContainer::calculateItemSize()
{
    FUNC_LOG;

    QRectF containerRect = contentsRect();
    INFO("Container rect:" << containerRect);

    if (!mCenterItemArea) return;

    QRectF centerItemRect = mCenterItemArea->mapRectToParent(mCenterItemArea->contentsRect());
    INFO("Center item rect:" << centerItemRect);
    if (centerItemRect.isEmpty()) return;

    QSizeF imageSize(1, 1); // Aspect ratio 1:1
    if (!mUserItemSize.isEmpty()) {
        imageSize = mUserItemSize; // Get aspect ration from user item size
    }
    imageSize.scale(centerItemRect.size(), Qt::KeepAspectRatio);

    QPointF delta = centerItemRect.center() - containerRect.center();
    INFO("Setting image size to:" << imageSize << "delta:" << delta);

    mRenderer->setImageSize(imageSize);
    mAutoSize = imageSize;
    setFrontItemPositionDelta(delta);
    mRenderer->setSpacing(QSizeF(1,1));
}

void HgCoverflowContainer::scrollToPosition(const QPointF& pos, bool animate)
{
    QPointF p = pos;
    p.setX((int)pos.x());
    HgContainer::scrollToPosition(p,animate);
}

QSizeF HgCoverflowContainer::getAutoItemSize() const
{
    return mAutoSize;
}

QSizeF HgCoverflowContainer::getAutoItemSpacing() const
{
    return QSizeF(1,1);
}

void HgCoverflowContainer::updateItemSizeAndSpacing()
{
    HgContainer::updateItemSizeAndSpacing();

    updateItemSize();
}


void HgCoverflowContainer::updateItemSize()
{
    if (mItemSizePolicy == HgWidget::ItemSizeAutomatic) {
        calculateItemSize();
    }
}

void HgCoverflowContainer::setFrontItemPositionDelta(const QPointF& position)
{
    if (!mRenderer)
        return;

    mRenderer->setFrontItemPosition(position);
}

QPointF HgCoverflowContainer::frontItemPositionDelta() const
{
    return mRenderer ? mRenderer->frontItemPosition() : QPointF();
}

void HgCoverflowContainer::setFrontItemElevationFactor(qreal factor)
{
    mRenderer->setFrontCoverElevationFactor(factor);
}

void HgCoverflowContainer::enableReflections(bool enabled)
{
    if (mRenderer)
        mRenderer->enableReflections(enabled);
}

bool HgCoverflowContainer::reflectionsEnabled() const
{
    return mRenderer ? mRenderer->reflectionsEnabled() : false;
}

void HgCoverflowContainer::setCenterItemArea(HgCenterItemArea *centerItemArea)
{
    FUNC_LOG;

    if (!mCenterItemArea) {
        mCenterItemArea = centerItemArea;
        connect(mCenterItemArea, SIGNAL(geometryChanged()), SLOT(updateItemSize()));
    }
}

void HgCoverflowContainer::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HgContainer::resizeEvent(event);
    updateItemSize();
}

// EOF
