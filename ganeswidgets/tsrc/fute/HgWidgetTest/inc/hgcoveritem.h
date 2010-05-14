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
#ifndef HGCOVERITEM_H_
#define HGCOVERITEM_H_

#include <QGraphicsPixmapItem>
#include <hbiconitem>
#include <hblistwidget>

class HgCoverItem : public HbIconItem
{
    Q_OBJECT

public:

    HgCoverItem(QPixmap pixmap, QGraphicsItem *parent);

signals:

    void close();
    
private:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

class HgListWidget : public HbListWidget
{
    Q_OBJECT
  
public:
    
    HgListWidget(QGraphicsItem *parent = 0);

signals:

    void close();
    
private:
    
    virtual void leftGesture(int value);    
    
};

#endif /* HGCOVERITEM_H_*/

// EOF
