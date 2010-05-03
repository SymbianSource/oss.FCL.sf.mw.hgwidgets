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
#ifndef HGCOVERFLOWWIDGET_H_
#define HGCOVERFLOWWIDGET_H_

#include <hbwidget>
#include <hblabel>
#include <qgraphicssceneresizeevent>
#include <hgwidgets/hgmediawall.h>
#include <QAbstractListModel>

/**
 * Coverflowwidget class which adds labels to normal MediaWall
 */
class HgCoverflowWidget : public HgMediawall
{
    Q_OBJECT
public:
    
	HgCoverflowWidget(QGraphicsItem *parent=0);
	virtual ~HgCoverflowWidget();

	void updateTextPositions();
	
private slots:
    void frontItemChanged ( const QModelIndex & current, const QModelIndex & previous );	

private:
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
private:
    HbLabel* mTitleLabel;
    HbLabel* mDescLabel;
    bool mTextPositionsDirty;
};

#endif /* HGTESTVIEW_H_ */
