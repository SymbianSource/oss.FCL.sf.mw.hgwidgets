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
#include "trace.h"


static const qreal KCameraMaxYAngle(20);
static const qreal KSpringVelocityToCameraYAngleFactor(2);
static const int   KLabelMargin(4);

HgCoverflowContainer::HgCoverflowContainer(
    QGraphicsItem* parent) : HgContainer(parent),
    mTitleLabel(0),
    mDescriptionLabel(0),
    mTitlePosition(HgMediawall::PositionAboveImage),
    mDescriptionPosition(HgMediawall::PositionNone),
    mPrevPos(-1),
    mAspectRatio(1),
    mAnimationAboutToEndReacted(false)
{
    mTitleLabel = new HbLabel(this);
    mTitleLabel->setZValue(zValue()+1);
    mTitleLabel->setAlignment(Qt::AlignCenter);
    mTitleLabel->setVisible(false);

    mDescriptionLabel = new HbLabel(this);
    mDescriptionLabel->setZValue(zValue()+1);
    mDescriptionLabel->setAlignment(Qt::AlignCenter);
    mDescriptionLabel->setVisible(false);

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

void HgCoverflowContainer::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    HbWidget::resizeEvent(event);

    updatePositions();
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

void HgCoverflowContainer::handleCurrentChanged(const QModelIndex &current)
{
    FUNC_LOG;

    if (current.isValid()) {
        updateLabels(current.row());
    }
}

void HgCoverflowContainer::itemDataChanged(const int &firstIndex, const int &lastIndex)
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mSelectionModel); // If model has been set, also is selection model

    HgContainer::itemDataChanged(firstIndex, lastIndex);

    if (mSelectionModel->currentIndex().isValid()) {
        int current = mSelectionModel->currentIndex().row();
        if (firstIndex <= current && current <= lastIndex) {
            updateLabels(current);
        }
    }

    if (firstIndex == 0) {
        // Take preferred aspect ratio from the first image
        const HgImage *firstImage = image(0);
        if (firstImage && firstImage->height() != 0) {
            mAspectRatio = qMax((qreal)0.1, (qreal)firstImage->width()/firstImage->height()); // Don't let aspect ratio go to 0
            updatePositions();
        }
    }
}

void HgCoverflowContainer::setTitlePosition(HgMediawall::LabelPosition position)
{
    FUNC_LOG;

    if (mTitlePosition != position) {
        mTitlePosition = position;
        updatePositions();
    }
}

HgMediawall::LabelPosition HgCoverflowContainer::titlePosition() const
{
    FUNC_LOG;

    return mTitlePosition;
}

void HgCoverflowContainer::setDescriptionPosition(HgMediawall::LabelPosition position)
{
    FUNC_LOG;

    if (mDescriptionPosition != position) {
        mDescriptionPosition = position;
        updatePositions();
    }
}

HgMediawall::LabelPosition HgCoverflowContainer::descriptionPosition() const
{
    FUNC_LOG;

    return mDescriptionPosition;
}

void HgCoverflowContainer::setTitleFontSpec(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    if (!mTitleLabel) return;
    if (mTitleLabel->fontSpec() != fontSpec) {
        mTitleLabel->setFontSpec(fontSpec);
        updatePositions();
    }
}

HbFontSpec HgCoverflowContainer::titleFontSpec() const
{
    FUNC_LOG;

    if (!mTitleLabel) return HbFontSpec();
    return mTitleLabel->fontSpec();
}

void HgCoverflowContainer::setDescriptionFontSpec(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    if (!mDescriptionLabel) return;
    if (mDescriptionLabel->fontSpec() != fontSpec) {
        mDescriptionLabel->setFontSpec(fontSpec);
        updatePositions();
    }
}

HbFontSpec HgCoverflowContainer::descriptionFontSpec() const
{
    FUNC_LOG;

    if (!mDescriptionLabel) return HbFontSpec();
    return mDescriptionLabel->fontSpec();
}

void HgCoverflowContainer::calculatePositions()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mTitleLabel);
    HANDLE_ERROR_NULL(mDescriptionLabel);

    int height = size().height();
    int width = size().width();
    int titleHeight = QFontMetrics(mTitleLabel->effectiveFontSpec().font()).height();
    int descriptionHeight = QFontMetrics(mDescriptionLabel->effectiveFontSpec().font()).height();
    qreal usableHeight = height-KLabelMargin;
    if (mTitlePosition != HgMediawall::PositionNone) {
        usableHeight -= (titleHeight+KLabelMargin);
    }
    if (mDescriptionPosition != HgMediawall::PositionNone) {
        usableHeight -= (descriptionHeight+KLabelMargin);
    }

    if (usableHeight <= 0) return;

    qreal usableWidth = width/1.4;
    if (usableWidth <= 0) return;

    QRectF imageRect = rect();
    if (usableWidth/usableHeight > mAspectRatio) {
        imageRect.setHeight(usableHeight);
        imageRect.setWidth(mAspectRatio*usableHeight);
        imageRect.moveLeft((width-imageRect.width())/2);
    }
    else {
        imageRect.setWidth(usableWidth);
        imageRect.setHeight(usableWidth/mAspectRatio);
        imageRect.moveTop((usableHeight-imageRect.height())/2);
        imageRect.moveLeft((width-imageRect.width())/2);
    }

    QRectF titleGeometry(0, imageRect.top()+KLabelMargin, width, titleHeight);
    QRectF descriptionGeometry(0, imageRect.top()+KLabelMargin, width, descriptionHeight);

    if (mTitlePosition == HgMediawall::PositionAboveImage &&
        mDescriptionPosition == HgMediawall::PositionAboveImage) {
        // titleGeometry default is ok
        descriptionGeometry.moveTop(titleGeometry.bottom()+KLabelMargin);
        imageRect.moveTop(descriptionGeometry.bottom()+KLabelMargin);
    }
    else if (mTitlePosition == HgMediawall::PositionBelowImage &&
             mDescriptionPosition == HgMediawall::PositionBelowImage) {
        titleGeometry.moveTop(imageRect.bottom()+KLabelMargin);
        descriptionGeometry.moveTop(titleGeometry.bottom()+KLabelMargin);
    }
    else {
        if (mTitlePosition == HgMediawall::PositionAboveImage) {
            // titleGeometry default is ok
            imageRect.moveTop(titleGeometry.bottom()+KLabelMargin);
        }
        else if (mDescriptionPosition == HgMediawall::PositionAboveImage) {
            // descriptionGeometry default is ok
            imageRect.moveTop(descriptionGeometry.bottom()+KLabelMargin);
        }

        if (mTitlePosition == HgMediawall::PositionBelowImage) {
            titleGeometry.moveTop(imageRect.bottom()+KLabelMargin);
        }
        else if (mDescriptionPosition == HgMediawall::PositionBelowImage) {
            descriptionGeometry.moveTop(imageRect.bottom()+KLabelMargin);
        }
    }

    INFO("Setting image rect to:" << imageRect << "(total size:" << QSize(width, height)
        << "usable size:" << QSizeF(usableWidth, usableHeight) << ", aspect ratio is:" << mAspectRatio << ")");

    mRenderer->setImageSize(imageRect.size());
    qreal diff = rect().center().y()-imageRect.center().y();

    INFO("Setting front item position to:" << QPointF(0, -diff) << "(rect:" << rect()
        << "imageRect:" << imageRect << ")");
    mRenderer->setFrontItemPosition(QPointF(0, -diff));

    mAutoSize = imageRect.size();

    if (mTitlePosition != HgMediawall::PositionNone) {
        INFO("Title geometry:" << titleGeometry);
        mTitleLabel->setGeometry(titleGeometry);
        mTitleLabel->setVisible(true);
    }
    else {
        mTitleLabel->setVisible(false);
    }
    if (mDescriptionPosition != HgMediawall::PositionNone) {
        INFO("Description geometry:" << descriptionGeometry);
        mDescriptionLabel->setGeometry(descriptionGeometry);
        mDescriptionLabel->setVisible(true);
    }
    else {
        mDescriptionLabel->setVisible(false);
    }

    // This may be called before selection model is set.
    if (mSelectionModel && mSelectionModel->currentIndex().isValid()) {
        updateLabels(mSelectionModel->currentIndex().row());
    }

    mRenderer->setSpacing(QSizeF(1,1));

}

void HgCoverflowContainer::positionLabels()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mTitleLabel);
    HANDLE_ERROR_NULL(mDescriptionLabel);

    int centerIconTop = (size().height() - mRenderer->getImageSize().height()) / 2;

    int height = size().height();
    int width = size().width();
    int titleHeight = QFontMetrics(mTitleLabel->effectiveFontSpec().font()).height();
    int descriptionHeight = QFontMetrics(mDescriptionLabel->effectiveFontSpec().font()).height();

    if (mTitlePosition == HgMediawall::PositionAboveImage &&
        mDescriptionPosition == HgMediawall::PositionAboveImage) {
        mTitleLabel->setGeometry(QRectF(
            0,
            qMax(KLabelMargin, centerIconTop-2*KLabelMargin-titleHeight-descriptionHeight),
            width, titleHeight));
        mDescriptionLabel->setGeometry(QRectF(
            0,
            mTitleLabel->geometry().bottom()+KLabelMargin,
            width, descriptionHeight));
    }
    else if (mTitlePosition == HgMediawall::PositionBelowImage &&
             mDescriptionPosition == HgMediawall::PositionBelowImage) {
        mDescriptionLabel->setGeometry(QRectF(
            0,
            height-descriptionHeight-KLabelMargin,
            width, descriptionHeight));
        mTitleLabel->setGeometry(QRectF(
            0,
            mDescriptionLabel->geometry().top()-titleHeight-KLabelMargin,
            width, titleHeight));
    }
    else {
        if (mTitlePosition == HgMediawall::PositionAboveImage) {
            mTitleLabel->setGeometry(QRectF(
                0,
                qMax(KLabelMargin, centerIconTop-KLabelMargin-titleHeight),
                width, titleHeight));
        }
        else if (mTitlePosition == HgMediawall::PositionBelowImage) {
            mTitleLabel->setGeometry(QRectF(
                0,
                height-titleHeight-KLabelMargin,
                width, titleHeight));
        }

        if (mDescriptionPosition == HgMediawall::PositionAboveImage) {
            mDescriptionLabel->setGeometry(QRectF(
                0,
                qMax(KLabelMargin, centerIconTop-KLabelMargin-descriptionHeight),
                width, descriptionHeight));
        }
        else if (mDescriptionPosition == HgMediawall::PositionBelowImage) {
            mDescriptionLabel->setGeometry(QRectF(
                0,
                height-descriptionHeight-KLabelMargin,
                width, descriptionHeight));
        }
    }

    mTitleLabel->setVisible(mTitlePosition != HgMediawall::PositionNone);
    mDescriptionLabel->setVisible(mDescriptionPosition != HgMediawall::PositionNone);

    INFO("Title geometry:" << mTitleLabel->geometry() << "visible:" << mTitleLabel->isVisible());
    INFO("Description geometry:" << mDescriptionLabel->geometry() << "visible:" << mDescriptionLabel->isVisible());

    if ( mSelectionModel &&  mSelectionModel->currentIndex().isValid()) {
        updateLabels(mSelectionModel->currentIndex().row());
    }
}


void HgCoverflowContainer::updateLabels(int itemIndex)
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mTitleLabel);
    HANDLE_ERROR_NULL(mDescriptionLabel);

    if (itemIndex >= 0 && itemIndex < mItems.count()) {
        mTitleLabel->setPlainText(mItems.at(itemIndex)->title());
        mDescriptionLabel->setPlainText(mItems.at(itemIndex)->description());
    }
}

void HgCoverflowContainer::scrollToPosition(const QPointF& pos, bool animate)
{
    QPointF p = pos;
    p.setX((int)pos.x());
    HgContainer::scrollToPosition(p,animate);
}

void HgCoverflowContainer::setDefaultImage(QImage defaultImage)
{
    HgContainer::setDefaultImage(defaultImage);

    if (!defaultImage.isNull()) {
        mAspectRatio = qMax((qreal)0.1, (qreal)defaultImage.width()/defaultImage.height()); // Don't let aspect ratio go to 0
        updatePositions();
    }
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

    updatePositions();
}


void HgCoverflowContainer::updatePositions()
{
    if (mItemSizePolicy == HgWidget::ItemSizeAutomatic) {
        calculatePositions();
    }
    else {
        positionLabels();
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

void HgCoverflowContainer::enableReflections(bool enabled)
{
    if (mRenderer)
        mRenderer->enableReflections(enabled);
}

bool HgCoverflowContainer::reflectionsEnabled() const
{
    return mRenderer ? mRenderer->reflectionsEnabled() : false;
}


