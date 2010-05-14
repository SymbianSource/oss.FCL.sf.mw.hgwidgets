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

#ifndef HGWIDGET_P_H
#define HGWIDGET_P_H

#include <hgwidgets/hgwidgets.h>
#include <QTimer>
#include <QTime>

class QAbstractItemModel;
class HgContainer;
class HgScrollBufferManager;
class QGraphicsLinearLayout;
class HgIndexFeedback;

class HgWidgetPrivate
{
    Q_DECLARE_PUBLIC(HgWidget)

public:

    HgWidgetPrivate();
    virtual ~HgWidgetPrivate();

    void init(HgContainer *container);

    void setModel(QAbstractItemModel *model);

    void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel() const;

    void setSelectionMode(HgWidget::SelectionMode mode, bool resetSelection);
    HgWidget::SelectionMode selectionMode() const;
    void selectAll();
    void clearSelection();

    QModelIndex currentIndex() const;
    void setCurrentIndex(const QModelIndex &index,
        QItemSelectionModel::SelectionFlags selectionFlag=QItemSelectionModel::NoUpdate);

    void scrollTo(const QModelIndex &index);

    bool getItemOutline(const QModelIndex& index, QPolygonF& points);
    void aboutToChangeOrientation();
    void orientationChanged(Qt::Orientation orientation);
    
    QList<QModelIndex> getVisibleItemIndices() const;
    Qt::Orientation scrollDirection() const;

    void setIndexFeedbackPolicy( HgWidget::IndexFeedbackPolicy policy);
    HgWidget::IndexFeedbackPolicy indexFeedbackPolicy() const;
    void setDefaultImage(QImage defaultImage);
    
    void setItemSizePolicy(HgWidget::ItemSizePolicy policy);
    HgWidget::ItemSizePolicy itemSizePolicy() const;

    void setItemSize(const QSizeF& size);
    QSizeF itemSize() const;
    
    void setItemSpacing(const QSizeF& size);
    QSizeF itemSpacing() const;
    
    HgWidget *q_ptr;
    
private:

    void clearCurrentModel();
    void initializeNewModel();

    void adjustGeometry();

    void createScrollBar(Qt::Orientation orientation);
    
    void setScrollBarPolicy(HgWidget::ScrollBarPolicy policy);
    void replaceScrollBar(HbScrollBar *scrollBar);

    virtual void updateScrollMetrics( qreal pos = 0 );
    void prepareScrollBars( qreal pos = 0);

    virtual void updateScrollBar(qreal pos = 0);
    void displayScrollBar(qreal pos = 0);
    void setScrollBarMetrics(qreal pos = 0);

    void lostForeground();
    void gainedForeground();
    
    // private slot functions
    void _q_scrollPositionChanged(qreal index, bool scrollBarAnimation);
    void _q_releaseItems( int releaseStart, int releaseEnd );
    void _q_requestItems( int requestStart, int requestEnd );
    void _q_hideScrollBars();
    void _q_thumbPositionChanged(qreal value, Qt::Orientation orientation);
    void _q_insertRows(const QModelIndex &parent, int start, int end);
    void _q_removeRows(const QModelIndex &parent, int start, int end);
    void _q_moveRows(const QModelIndex &sourceParent, int sourceStart,
                     int sourceEnd, const QModelIndex &destinationParent,
                     int destinationRow);
    void _q_modelReset();
    void _q_groovePressed(qreal value, Qt::Orientation orientation);

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    void initBufferManager(int itemCount);
    
protected:

    QGraphicsLinearLayout *mLayout;

    HgContainer *mContainer;
    HgScrollBufferManager *mBufferManager;

    QAbstractItemModel *mModel;
    QItemSelectionModel *mSelectionModel;
    QItemSelectionModel *mDefaultSelectionModel;

    HbScrollBar *mScrollBar;
    HgWidget::ScrollBarPolicy mScrollBarPolicy;
    bool mAbleToScroll;
    QTimer* mScrollBarHideTimer;

    bool mHandleLongPress;
    bool mForeground;
    int mBufferSize;
//    HgIndexFeedback *mIndexFeedback;
    bool mStaticScrollDirection;
};

#endif  //HGWIDGET_p_H

