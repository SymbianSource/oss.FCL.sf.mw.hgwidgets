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

#ifndef HGCONTAINER_H
#define HGCONTAINER_H

#include <QTime>
#include <hbwidget.h>
#include <hgwidgets/hgwidgets.h>

#include "hgmediawalldataprovider.h"
#include "hgdrag.h"
#include "hgspring.h"

class HgWidgetItem;
class HgQuadRenderer;
class HgMediaWallRenderer;
class QPanGesture;
class HbAbstractViewItem;
class HbGestureSceneFilter;
class HgLongPressVisualizer;

class HgContainer: public HbWidget, public HgMediaWallDataProvider
{
    Q_OBJECT
    Q_DISABLE_COPY(HgContainer)

public:
    explicit HgContainer(QGraphicsItem* parent = 0);
    virtual ~HgContainer();
    
    void setItemCount(int count);
    int itemCount() const;
    int rowCount() const;

    QList<HgWidgetItem*> items() const;
    HgWidgetItem* itemByIndex(const QModelIndex &index) const;
    HgWidgetItem* itemByIndex(const int &index) const;

    void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel() const;
    void setSelectionMode(HgWidget::SelectionMode mode, bool resetSelection);
    HgWidget::SelectionMode selectionMode() const;

    void dimensions(qreal &screenSize, qreal &worldSize);
    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation, bool animate=true);

    // new size for the widget. calls resize.
    void scrollToPosition(qreal value, bool animate = false);
    virtual void scrollToPosition(const QPointF& pos, bool animate);
    void scrollTo(const QModelIndex &index);

    void itemDataChanged(const QModelIndex &firstIndex, const QModelIndex &lastIndex);

    void addItems(int start, int end);
    void removeItems(int start, int end);
    void moveItems(int start, int end, int destination);

    bool getItemPoints(int index, QPolygonF& points);

    QList<QModelIndex> getVisibleItemIndices() const;

    virtual void itemDataChanged(const int &firstIndex, const int &lastIndex);

    void init(Qt::Orientation scrollDirection);

signals:

    // emit this signal when scrolling. for example scrollbar can be connected to this signal.
    void scrollPositionChanged(qreal value, bool scrollBarAnimation);
    void centerItemChanged(const QModelIndex &index);
    void activated(const QModelIndex &index);
    void longPressed(const QModelIndex &index, const QPointF &coords);
    void scrollingStarted();
    void scrollingEnded();

private slots:

    void updateBySpringPosition();
    void redraw();
    void updateLongPressVisualizer();
    void updateByCurrentIndex(const QModelIndex &current);
    void onScrollingEnded();

protected: // from HgMediaWallDataProvider

    int imageCount() const;
    const HgImage *image(int index) const;
    int flags(int index) const;
    const HgImage *indicator(int flags) const;

protected: // events

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void resizeEvent(QGraphicsSceneResizeEvent *event);

    // this needs to be implemented for gesture framework to work
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    // From HbWidget for gestures.
    virtual void gestureEvent(QGestureEvent *event);
protected:

    virtual HgMediaWallRenderer* createRenderer()=0;
    virtual qreal getCameraDistance(qreal springVelocity);
    virtual qreal getCameraRotationY(qreal springVelocity);
    virtual void handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void onScrollPositionChanged(qreal pos);
    virtual void handleCurrentChanged(const QModelIndex &current);

protected:

    enum ItemActionType
    {
        LongTap = 1,
        DoubleTap,
        NormalTap,
        TapStart,
        LongTapStart
    };

    qreal worldWidth() const;
    void initSpringForScrollBar();
    void initSpringForScrolling();
    void boundSpring();
    void handlePanning(QPanGesture *gesture);
    void handleTap(Qt::GestureState state, const QPointF &pos);
    void handleLongTap(Qt::GestureState state, const QPointF &pos);
    void handleItemAction(const QPointF &pos, ItemActionType action);

    void selectItem();
    void updateSelectedItem();
    void unselectItem();

    HgWidgetItem* getItemAt(const QPointF& pos, int& index);
    void startLongPressWatcher(const QPointF& pos);
    void stopLongPressWatcher();
    bool updateSelectionModel(HgWidgetItem* item);

    QTransform qtToVgTransform() const;
    QPointF mapQtToVg(const QPointF& p) const;

protected: // data

    QList<HgWidgetItem *> mItems;
    //int mRowCount;
    //int mColumnCount;

    //HbLabel *mLabel;
    HgQuadRenderer *mQuadRenderer;
    HgMediaWallRenderer *mRenderer;

    HgSpring mSpring;
    HgDrag mDrag;

    QTime mTapDuration; // Temp hack until HbTapGesture is available
    QTime mDoubleTapDuration;
    int mTapCount;

    bool mAnimateUsingScrollBar;

    HgWidget::SelectionMode mSelectionMode;
    QItemSelectionModel *mSelectionModel;
    HgImage *mMarkImage;
    
    qreal mSpringVelAtDragStart;
    bool mDragged;
    int mFramesDragged;

    HbAbstractViewItem* mHitItemView;
    QPixmap mHitItemPixmap;

    HgLongPressVisualizer* mLongPressVisualizer;
    QTimer* mLongPressTimer;
    HgWidgetItem* mHitItem;
    int mHitItemIndex;

    QPointF mOffsetAtDragStart;
    QTime mLongTapDuration;
    bool mScrollBarPressed;
        
};

#endif