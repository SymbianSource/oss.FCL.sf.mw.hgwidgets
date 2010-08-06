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
#include <HbWidget>
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

    void setSelectionModel(QItemSelectionModel *selectionModel, const QModelIndex &defaultItem);
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

    void setDefaultImage(QImage defaultImage);

    void setItemSizePolicy(HgWidget::ItemSizePolicy policy);
    HgWidget::ItemSizePolicy itemSizePolicy() const;

    void setItemSize(const QSizeF& size);
    QSizeF itemSize() const;

    void setItemSpacing(const QSizeF& size);
    QSizeF itemSpacing() const;

    Qt::Orientation scrollDirection() const;
    qreal scrollPosition() const;

    void setHandleLongPress(bool handleLongPress);
        
signals:

    // emit this signal when scrolling. for example scrollbar can be connected to this signal.
    void scrollPositionChanged(qreal value, bool scrollBarAnimation);
    void centerItemChanged(const QModelIndex &index);
    void activated(const QModelIndex &index);
    void longPressed(const QModelIndex &index, const QPointF &coords);
    void scrollingStarted();
    void scrollingEnded();

protected slots:

    virtual void onScrollingStarted();
    virtual void onScrollingEnded();

private slots:

    void updateBySpringPosition();
    void redraw();
    void updateLongPressVisualizer();

protected: // from HgMediaWallDataProvider

    int imageCount() const;
    const HgImage *image(int index) const;
    int flags(int index) const;
    const HgImage *indicator(int flags);

protected: // events

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void resizeEvent(QGraphicsSceneResizeEvent *event);

    // this needs to be implemented for gesture framework to work
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    // From HbWidget for gestures.
    virtual void gestureEvent(QGestureEvent *event);
protected:

    virtual HgMediaWallRenderer* createRenderer(Qt::Orientation scrollDirection)=0;
    virtual qreal getCameraDistance(qreal springVelocity);
    virtual qreal getCameraRotationY(qreal springVelocity);
    virtual void handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual void onScrollPositionChanged(qreal pos);
    virtual void loadIndicatorGraphics(bool loadIfExists = false);
    
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
    bool handlePanning(QPanGesture *gesture);
    bool handleTap(Qt::GestureState state, const QPointF &pos);
    bool handleLongTap(Qt::GestureState state, const QPointF &pos);
    bool handleItemAction(const QPointF &pos, ItemActionType action);

    void selectItem(int index);
    void updateSelectedItem();
    void unselectItem();

    bool hasItemAt(const QPointF& pos);
    HgWidgetItem* getItemAt(const QPointF& pos, int& index);
    void startLongPressWatcher(const QPointF& pos);
    void stopLongPressWatcher();
    bool updateSelectionModel(HgWidgetItem* item);


    virtual void updateItemSizeAndSpacing();
    virtual QSizeF getAutoItemSize() const;
    virtual QSizeF getAutoItemSpacing() const;

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
    HgImage *mMarkImageOn;
    HgImage *mMarkImageOff;

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

    HgWidget::ItemSizePolicy mItemSizePolicy;
    QSizeF mUserItemSize;
    QSizeF mUserItemSpacing;

    Qt::Orientation mOrientation;
    QModelIndex mDelayedScrollToIndex;
    bool mIgnoreGestureAction;
    bool mHandleLongPress;
};

#endif
