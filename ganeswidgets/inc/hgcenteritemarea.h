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

#ifndef HGCENTERICONAREA_H
#define HGCENTERICONAREA_H

#include <hbwidget.h>

class HgCenterItemArea: public HbWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(HgCenterItemArea)

public:
    explicit HgCenterItemArea(QGraphicsItem* parent = 0);
    virtual ~HgCenterItemArea();

signals:
    void geometryChanged();

private:
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void moveEvent(QGraphicsSceneMoveEvent *event);
    
private: // Data

};

#endif
