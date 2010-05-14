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
* Description:  Another view for test application.
*
*/
#include "hgcoveritem.h"

HgCoverItem::HgCoverItem(QPixmap pixmap, QGraphicsItem *parent) : HbIconItem(parent)
{
    setIcon(HbIcon(QIcon(pixmap)));
}

void HgCoverItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
    emit close();
    }

HgListWidget::HgListWidget(QGraphicsItem *parent) : HbListWidget(parent)
{
    setScrollDirections( Qt::Horizontal | Qt::Vertical );
    setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
}

void HgListWidget::leftGesture(int value)
{
    if (value > 30) {
        emit close();
    }
}

