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
    mCenterIconTop(0),
    mPrevPos(-1)
{
    mTitleLabel = new HbLabel(this);
    mTitleLabel->setZValue(zValue()+1);
    mTitleLabel->setAlignment(Qt::AlignCenter);
    mTitleLabel->setVisible(false);

    mDescriptionLabel = new HbLabel(this);
    mDescriptionLabel->setZValue(zValue()+1);
    mDescriptionLabel->setAlignment(Qt::AlignCenter);
    mDescriptionLabel->setVisible(false);
}

HgCoverflowContainer::~HgCoverflowContainer()
{
}

// events
void HgCoverflowContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    HgContainer::paint(painter, option, widget);
}

void HgCoverflowContainer::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    HbWidget::resizeEvent(event);

    QSizeF s(size());
    qreal side = qMin(s.height()/1.8, s.width()/1.8);
    INFO("Setting image size to:" << side << "," << side);
    mRenderer->setImageSize(QSizeF(side, side));
    mCenterIconTop = (s.height()-side)/2;

    positionLabels();
}

// from HgContainer
HgMediaWallRenderer* HgCoverflowContainer::createRenderer()
{
    HgMediaWallRenderer* renderer = new HgMediaWallRenderer(this);
    renderer->setImageSize(QSizeF(200, 200));
    renderer->enableCoverflowMode(true);
    renderer->setRowCount(1, renderer->getImageSize(), false);
    renderer->enableReflections(true);
    renderer->setSpacing(QSizeF(1,1));
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

    if (mPrevPos != (int)pos) {
        mPrevPos = (int)pos;
        HgWidgetItem* item = itemByIndex((int)pos);
        if (item && item->modelIndex() != mSelectionModel->currentIndex()) {
            mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
        }            
    }
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
}

void HgCoverflowContainer::setTitlePosition(HgMediawall::LabelPosition position)
{
    FUNC_LOG;

    if (mTitlePosition != position) {
        mTitlePosition = position;
        positionLabels();
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
        positionLabels();
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
        positionLabels();
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
        positionLabels();
    }
}

HbFontSpec HgCoverflowContainer::descriptionFontSpec() const
{
    FUNC_LOG;

    if (!mDescriptionLabel) return HbFontSpec();
    return mDescriptionLabel->fontSpec();
}

void HgCoverflowContainer::positionLabels()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mTitleLabel);
    HANDLE_ERROR_NULL(mDescriptionLabel);
    HANDLE_ERROR_NULL(mSelectionModel);

    int height = size().height();
    int width = size().width();
    int titleHeight = QFontMetrics(mTitleLabel->effectiveFontSpec().font()).height();
    int descriptionHeight = QFontMetrics(mDescriptionLabel->effectiveFontSpec().font()).height();

    if (mTitlePosition == HgMediawall::PositionAboveImage &&
        mDescriptionPosition == HgMediawall::PositionAboveImage) {
        mTitleLabel->setGeometry(QRectF(
            0,
            qMax(KLabelMargin, mCenterIconTop-2*KLabelMargin-titleHeight-descriptionHeight),
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
                qMax(KLabelMargin, mCenterIconTop-KLabelMargin-titleHeight),
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
                qMax(KLabelMargin, mCenterIconTop-KLabelMargin-descriptionHeight),
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

    if (mSelectionModel->currentIndex().isValid()) {
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
