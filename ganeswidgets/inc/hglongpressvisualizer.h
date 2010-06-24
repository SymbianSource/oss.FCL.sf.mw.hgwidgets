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

#ifndef HGLONGPRESSVISUALIZER_H
#define HGLONGPRESSVISUALIZER_H

#include <QTime>
#include <HbWidget>
#include <hgwidgets/hgwidgets.h>

#include "hgmediawalldataprovider.h"
#include "hgdrag.h"
#include "hgspring.h"

class HgLongPressVisualizer : public HbWidget
{
public:
    HgLongPressVisualizer(QGraphicsItem* parent=0);
    virtual ~HgLongPressVisualizer();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=0);
    QRectF boundingRect() const;
    
    void start(const QPointF& pos);
    void stop();
    void setFrame(int frame);
private:
    QRectF rect;    
    bool active;
    qreal spanAngle;
};

#endif
