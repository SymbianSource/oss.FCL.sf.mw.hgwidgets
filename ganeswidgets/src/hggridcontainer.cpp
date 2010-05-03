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
#include <QTimer>
#include <hblabel.h>
#include <hbgridviewitem>
#include <hbmainwindow>
#include "hggridcontainer.h"
#include "hgmediawallrenderer.h"
#include "hgquad.h"
#include "hgvgquadrenderer.h"
#include "hgvgimage.h"
#include "hgwidgetitem.h"
#include "trace.h"

#include <hbgridviewitem>
#include <hbgridview>
#include <hbiconitem>
#include <qabstractitemmodel>
#include "hglongpressvisualizer.h"


static const qreal KCameraMaxYAngle(20);
static const qreal KSpringVelocityToCameraYAngleFactor(2);

HgGridContainer::HgGridContainer(QGraphicsItem *parent) : HgContainer(parent)
{

    mUserItemSize = QSize(120,120);
    mUserItemSpacing = QSize(1,1);

}

HgGridContainer::~HgGridContainer()
{
    
}

void HgGridContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    HgContainer::paint(painter, option, widget);
    
    updateSelectedItem();
}

HgMediaWallRenderer* HgGridContainer::createRenderer(Qt::Orientation scrollDirection)
{

    HgMediaWallRenderer* renderer = new HgMediaWallRenderer(this, scrollDirection, false);
    renderer->enableCoverflowMode(false);
    renderer->setImageSize(mUserItemSize);
    renderer->setRowCount(3, renderer->getImageSize(), false);    
    renderer->enableReflections(false);
    renderer->setSpacing(mUserItemSpacing);
    renderer->setFrontCoverElevationFactor(0.5);    

    return renderer;
}

qreal HgGridContainer::getCameraDistance(qreal springVelocity)
{
    if (mRenderer->getOrientation() == Qt::Vertical)
        return 0;
    
    return qAbs(springVelocity * 0.01f);
}

qreal HgGridContainer::getCameraRotationY(qreal springVelocity)
{
    if (mRenderer->getOrientation() == Qt::Vertical)
        return 0;

    return qBound(-KCameraMaxYAngle, springVelocity * KSpringVelocityToCameraYAngleFactor, KCameraMaxYAngle);
}

void HgGridContainer::handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(pos)
    Q_UNUSED(hitItemIndex)
        
    selectItem();                
    emit activated(hitItem->modelIndex());                        
}

void HgGridContainer::handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(hitItemIndex)
    
    selectItem();
    emit longPressed(hitItem->modelIndex(), pos);    
}

void HgGridContainer::onScrollPositionChanged(qreal pos)
{
    HgContainer::onScrollPositionChanged(pos);

    if (pos < 0) return;    
    const int index = ((int)pos)*rowCount();
    if (index > itemCount()) return;
    
    HgWidgetItem* item = itemByIndex(index);
    if (item && item->modelIndex() != mSelectionModel->currentIndex()) {
        qDebug() << "CURRENT CHANGE" << QString::number(item->modelIndex().row());
        mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
    }    
}
