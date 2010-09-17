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
#include <HbGridViewItem>
#include <HbMainWindow>
#include "hglongpressvisualizer.h"

HgLongPressVisualizer::HgLongPressVisualizer(QGraphicsItem* parent) : HbWidget(parent),
    active(false),
    spanAngle(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

HgLongPressVisualizer::~HgLongPressVisualizer()
{
    
}


void HgLongPressVisualizer::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    if (active) {
        QPen oldPen = painter->pen();
        
        QPen pen( Qt::lightGray );
        pen.setWidth(5);
        painter->setPen(pen);
        painter->drawArc(rect, 90*16, -spanAngle*16);

        painter->setPen(oldPen); //revert the painter to its old state
    }
}

void HgLongPressVisualizer::start(const QPointF& scenePos)
{
    prepareGeometryChange();
    rect = QRect( 0, 0, 30, 30);

    if (scenePos.y() < 60 ) {
        //Draw the animation below of the touch point
        rect.moveCenter( QPointF(scenePos.x(), scenePos.y()+50));
    }
    else {
        //Draw the animation above of the touch point
        rect.moveCenter( QPointF(scenePos.x(), scenePos.y()-50));
    }

    setFrame(0);
    active = true;
}

void HgLongPressVisualizer::stop()
{
    active = false;
    update();
}

void HgLongPressVisualizer::setFrame(int frame)
{
    spanAngle = frame*360/100;
    update();
}

QRectF HgLongPressVisualizer::boundingRect() const 
{
    return rect;
}
