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
#include "hgcoverflowwidget.h"
#include "trace.h"

#include <qgraphicslinearlayout.h>
#include <qgraphicssceneresizeevent>



HgCoverflowWidget::HgCoverflowWidget(QGraphicsItem *parent) : HgMediawall(parent), 
mTextPositionsDirty(false)
{
    mTitleLabel = new HbLabel("Title", this);
    mTitleLabel->setAlignment(Qt::AlignCenter);
    mDescLabel = new HbLabel("Description", this);
    mDescLabel->setAlignment(Qt::AlignCenter);    
}

HgCoverflowWidget::~HgCoverflowWidget()
{
    
}

void HgCoverflowWidget::frontItemChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    if (!this->model())
        return;
    
    QVariant variant = this->model()->data(current, Qt::DisplayRole);
    QStringList texts = variant.toStringList();
    HbLabel* labels[2];
    labels[0] = mTitleLabel;
    labels[1] = mDescLabel;
    int n = texts.size() > 2 ? 2 : texts.size();
    for (int i = 0; i < n; i++)
        labels[i]->setPlainText(texts[i]);
    
    mTextPositionsDirty = true;
}

void HgCoverflowWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HgMediawall::resizeEvent(event);
    
    mTextPositionsDirty = true;
    
    QObject::connect(selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
        this, SLOT(frontItemChanged(const QModelIndex&, const QModelIndex&)));
}

void HgCoverflowWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    HgMediawall::paint(painter, option, widget);

    if (mTextPositionsDirty)
    {
        QModelIndex index = this->currentIndex();

        if (index.isValid())
        {
            QRectF bounds;

            QPointF halfSize = QPointF(itemSize().width()/2, itemSize().height()/2);
            bounds.setTopLeft(rect().center() + frontItemPositionDelta() - halfSize);              
            bounds.setBottomRight(rect().center() + frontItemPositionDelta() + halfSize);

            mTitleLabel->resize(QSizeF(size().width(), mTitleLabel->minimumSize().height()));
            mTitleLabel->setPos(QPointF(bounds.center().x() - size().width()/2,
                bounds.top() - 40));
            mDescLabel->resize(QSizeF(size().width(), mDescLabel->minimumSize().height()));
            mDescLabel->setPos(QPointF(bounds.center().x() - size().width()/2,
                    bounds.bottom() + 40 - mDescLabel->minimumSize().height()));
            
            frontItemChanged(index, index);
            
            mTextPositionsDirty = false;        
        }
        
    }
    
}

void HgCoverflowWidget::updateTextPositions()
{
    mTextPositionsDirty = true;
}



