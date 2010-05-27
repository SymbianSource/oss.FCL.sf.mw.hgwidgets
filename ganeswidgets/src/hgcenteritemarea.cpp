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

#include <QGraphicsSceneResizeEvent>
#include "hgcenteritemarea.h"
#include "trace.h"

HgCenterItemArea::HgCenterItemArea(QGraphicsItem* parent) :
    HbWidget(parent)
{
    FUNC_LOG;
}

HgCenterItemArea::~HgCenterItemArea()
{
    FUNC_LOG;
}

void HgCenterItemArea::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;
    INFO("HgCenterItemArea: size:" << size() << event->newSize());

    emit sizeChanged();
    HbWidget::resizeEvent(event);
}
