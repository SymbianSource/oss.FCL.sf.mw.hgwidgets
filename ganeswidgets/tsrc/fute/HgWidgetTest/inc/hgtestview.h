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
#ifndef HGTESTVIEW_H_
#define HGTESTVIEW_H_

#include <hbview.h>

class HbAction;
class HbLabel;

class HgTestView : public HbView
{
	Q_OBJECT

public:
	HgTestView(const QString &title1, const QString &title2, const QPixmap &pixmap, QGraphicsItem *parent=0);

signals:
    void closeRequested();

private:
    void resizeEvent(QGraphicsSceneResizeEvent *event);

private:
    HbLabel *mIconLabel;
    QPixmap mPixmap;
};

#endif /* HGTESTVIEW_H_ */
