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

#ifndef HGGRIDCONTAINER_H
#define HGGRIDCONTAINER_H

#include "hgcontainer.h"
#include <QPropertyAnimation>
#include <QQueue>

class HgWidgetItem;
class HgMediaWallRenderer;
class HbPinchGesture;

class HgGridContainer : public HgContainer
{
    Q_OBJECT
    Q_DISABLE_COPY(HgGridContainer)

public:
    explicit HgGridContainer(QGraphicsItem *parent = 0);
    
    virtual ~HgGridContainer();

    void setEffect3dEnabled(bool effect3dEnabled);
    bool effect3dEnabled() const;
    
    void setPinchEnabled(bool pinchEnabled);
    bool pinchEnabled() const;
    void setPinchLevels(QPair<int,int> levels, Qt::Orientation scrollDirection);
    QPair<int,int> pinchLevels(Qt::Orientation scrollDirection) const;
    
    void setRowCount(int count, Qt::Orientation scrollDirection = Qt::Horizontal);
    int rowCount(Qt::Orientation scrollDirection) const;

    void setReflectionsEnabled(bool reflectionsEnabled);
    bool reflectionsEnabled() const;    
    
signals:

    void emptySpacePressed();
    
protected:

    // events
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
    // from HgContainer
    virtual HgMediaWallRenderer* createRenderer(Qt::Orientation scrollDirection);
    virtual qreal getCameraDistance(qreal springVelocity);
    virtual qreal getCameraRotationY(qreal springVelocity);
    virtual bool handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex);
    virtual bool handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemindex);
    virtual void onScrollPositionChanged(qreal pos);
    virtual bool handleTap(Qt::GestureState state, const QPointF &pos);
    virtual bool handleLongTap(Qt::GestureState state, const QPointF &pos);
    virtual void setOrientation(Qt::Orientation orientation, bool animate=true);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void gestureEvent(QGestureEvent* event);
    bool event(QEvent *e);
    
    void handlePinchEnd();
    void handlePinchUpdate(HbPinchGesture* pinch);    

    void updateReflections(bool enable, int start, int end);

    virtual void addItems(int start, int end);
    virtual void removeItems(int start, int end);
    virtual void moveItems(int start, int end, int destination);
    
public slots:
    
    void effectFinished();
    
private:    
    
    bool mEffect3dEnabled;
    bool mPinchEnabled;
    bool mPinchingOngoing;
    int mTempImageHeightForLineGrid;    //this is changed during pinching
    int mTempImageHeightFinal;          //this is changed during pinching
    int mTempRowCount;                  //this is changed during pinching
    int mTargetRowCount;
    QQueue<qreal> iTargetRowCountList;
    QSizeF mTargetImageSize;
    bool mPinchEndAlreadyHandled;
    bool mReactToOnlyPanGestures;
    QPropertyAnimation iFadeAnimation;
    int mHorizontalRowCount;
    int mVerticalColumnCount;
    QPair<int,int> mHorizontalPinchLevels;
    QPair<int,int> mVerticalPinchLevels;
};

#endif
