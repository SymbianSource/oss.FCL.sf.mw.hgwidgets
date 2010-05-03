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

#ifndef HGGRIDCONTAINER_H
#define HGGRIDCONTAINER_H

#include "HgContainer.h"

class HbLabel;
class HgWidgetItem;
class HgMediaWallRenderer;

class HgGridContainer : public HgContainer
{
    Q_OBJECT
    Q_DISABLE_COPY(HgGridContainer)

public:
    explicit HgGridContainer(QGraphicsItem *parent = 0);
    
    virtual ~HgGridContainer();

protected:

    // events
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
    // from HgContainer
    virtual HgMediaWallRenderer* createRenderer(Qt::Orientation scrollDirection);
    virtual qreal getCameraDistance(qreal springVelocity);
    virtual qreal getCameraRotationY(qreal springVelocity);
    virtual void handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemindex);
    virtual void onScrollPositionChanged(qreal pos);
   
};

#endif
