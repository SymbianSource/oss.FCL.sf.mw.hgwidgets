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

#include <QGesture>
#include <QPainter>
#include <QTimer>
#include <HbGridViewItem>
#include <HbMainWindow>
#include "hggridcontainer.h"
#include "hgmediawallrenderer.h"
#include "hgquad.h"
#include "hgwidgetitem.h"
#include "trace.h"

#include <HbGridView>
#include <HbIconItem>
#include <QAbstractItemModel>
#include "hglongpressvisualizer.h"
#include <HbPinchGesture>
#include <QGraphicsSceneMouseEvent>

static const qreal KCameraMaxYAngle(20);
static const qreal KSpringVelocityToCameraYAngleFactor(2);

HgGridContainer::HgGridContainer(QGraphicsItem *parent) :
    HgContainer(parent),
    mEffect3dEnabled(true),
    mPinchEnabled(false),
    mPinchingOngoing(false),
    mTempImageHeightForLineGrid(-1),
    mTempImageHeightFinal(-1),
    mTempRowCount(-1),
    mPinchEndAlreadyHandled(false),
    mReactToOnlyPanGestures(false),
    mHorizontalRowCount(3),
    mVerticalColumnCount(3),
    mHorizontalPinchLevels(QPair<int,int>(2,3)),
    mVerticalPinchLevels(QPair<int,int>(2,5))
{
    mUserItemSize = QSize(120,120);
    mUserItemSpacing = QSize(0,0);
}

HgGridContainer::~HgGridContainer()
{
}

void HgGridContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    HgContainer::paint(painter, option, widget);

    // Draw these only while pinching
    if(mPinchingOngoing)
    {         
        const QPen& oldPen = painter->pen();
        
        // dim the background, i.e. draw trasparent black screen-sized rect (alpha is 100 of 255)
        painter->fillRect(rect(), QColor(0, 0, 0, 100));
        
        QPen pen = painter->pen();
        pen.setColor(Qt::white);
        pen.setWidth(4);
        painter->setPen(pen);
        
        int imageXCount;
        int imageYCount;
        int widthSpacingPlusImage;
        int heightSpacingPlusImage;
        // Use temp values that are updated during pinching
        QSize imageSize(mTempImageHeightForLineGrid, mTempImageHeightForLineGrid);
        
        if (scrollDirection() == Qt::Horizontal )   //landscape mode
        {
            imageXCount = rect().width() / imageSize.width();
            imageYCount = mTempRowCount;
            widthSpacingPlusImage = mRenderer->getSpacing().height() + imageSize.height();
            heightSpacingPlusImage = mRenderer->getSpacing().width() + imageSize.width();
        }
        else                                        //portrait mode
        {
            imageXCount = mTempRowCount;
            imageYCount = rect().height() / imageSize.height();
            widthSpacingPlusImage = mRenderer->getSpacing().width() + imageSize.width();
            heightSpacingPlusImage = mRenderer->getSpacing().height() + imageSize.height();
        }
        
        int yOffset(0);
        if (scrollDirection() == Qt::Horizontal ) {            
            yOffset = (rect().height() - (mUserItemSize.height()*mHorizontalRowCount))/2;
        }
        
        for (int xCounter = 0; xCounter < (imageXCount+1); ++xCounter)
        {
            for (int yCounter = 0; yCounter < (imageYCount+1); ++yCounter)
            {
                painter->drawLine(QPoint(0, yOffset + yCounter * heightSpacingPlusImage), 
                                  QPoint(rect().width(), yOffset + yCounter * heightSpacingPlusImage));
            }
            
            painter->drawLine(QPoint(xCounter * widthSpacingPlusImage, yOffset), 
                              QPoint(xCounter * widthSpacingPlusImage, rect().height()-yOffset));
        }
        
        painter->setPen(oldPen);
        
    }
    
    updateSelectedItem();
}

HgMediaWallRenderer* HgGridContainer::createRenderer(Qt::Orientation scrollDirection)
{

    HgMediaWallRenderer* renderer = new HgMediaWallRenderer(this, scrollDirection, scrollDirection, false);
    renderer->enableCoverflowMode(false);
    renderer->setImageSize(mUserItemSize);
    const int rowCount = scrollDirection == Qt::Horizontal ? mHorizontalRowCount : mVerticalColumnCount;
    renderer->setRowCount(rowCount, renderer->getImageSize(), false);    
    renderer->enableReflections(false);
    renderer->setSpacing(mUserItemSpacing);
    renderer->setFrontCoverElevationFactor(0.5);    

    return renderer;
}

qreal HgGridContainer::getCameraDistance(qreal springVelocity)
{
    if (mRenderer->getScrollDirection() == Qt::Vertical || !mEffect3dEnabled)
        return 0;
    
    return qAbs(springVelocity * 0.01f);
}

qreal HgGridContainer::getCameraRotationY(qreal springVelocity)
{
    if (mRenderer->getScrollDirection() == Qt::Vertical || !mEffect3dEnabled)
        return 0;

    return qBound(-KCameraMaxYAngle, springVelocity * KSpringVelocityToCameraYAngleFactor, KCameraMaxYAngle);
}

bool HgGridContainer::handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(pos)

    if (mSelectionMode != HgWidget::NoSelection) {
        return handleItemSelection(hitItem);
    }
    
    if (!mIgnoreGestureAction) {
        selectItem(hitItemIndex);
        emit activated(hitItem->modelIndex());
    } else {
        mSpring.resetVelocity();
        update();
        mIgnoreGestureAction = false;
    }
    return true;
}

bool HgGridContainer::handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(hitItemIndex)
    
    INFO("Long tap:" << hitItem->modelIndex().row());
    
    selectItem(hitItemIndex);
    
    if (!mIgnoreGestureAction) {
        if (mHandleLongPress){
            emit longPressed(hitItem->modelIndex(), pos);
        }
    } else {
        mSpring.resetVelocity();
        update();
        mIgnoreGestureAction = false;
    }
    return true;
}

void HgGridContainer::onScrollPositionChanged(qreal pos)
{
    HgContainer::onScrollPositionChanged(pos);

    if (pos < 0) return;    
    const int index = ((int)pos)*currentRowCount();
    if (index > itemCount()) return;
    
    HgWidgetItem* item = itemByIndex(index);
    if (item && item->modelIndex() != mSelectionModel->currentIndex()) {
        mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
    }    
}

void HgGridContainer::setEffect3dEnabled(bool effect3dEnabled)
{
    if (mEffect3dEnabled != effect3dEnabled) {
        // Setting has changed. redraw screen.
        mEffect3dEnabled = effect3dEnabled;
        update();
    }
}

bool HgGridContainer::effect3dEnabled() const
{
    return mEffect3dEnabled;
}

bool HgGridContainer::handleTap(Qt::GestureState state, const QPointF &pos)
{
    FUNC_LOG;
    
    bool handleGesture = false;
    
    if (hasItemAt(pos)) {
        switch (state) 
            {
            case Qt::GestureStarted:
                {
                // TODO IS THIS IF REALLY NEEDED
                if(mSpring.isActive()) {                    
                    qreal springPos = mSpring.pos().x();
                    int gridTotalHeightInImages = ceilf( mItems.count() / mRenderer->getRowCount() );
                    qreal currentViewHeightInImages;
                    if (scrollDirection() == Qt::Horizontal ) {
                        int rowHeight = mRenderer->getImageSize().width() + mRenderer->getSpacing().width();
                        currentViewHeightInImages = rect().width() / rowHeight;
                    } else {
                        int rowHeight = mRenderer->getImageSize().height() + mRenderer->getSpacing().height();
                        currentViewHeightInImages = rect().height() / rowHeight;
                    }
                    
                    // If list does not currently fill the whole screen (some theme background behind the list
                    // is visible), and list is moving, then do not react to tapping.
                    if( springPos >= 0 
                        && springPos <= (gridTotalHeightInImages - currentViewHeightInImages) ) {
                        mSpring.cancel();
                        mEmitScrollingEnded = true;
                    }
                    mIgnoreGestureAction = true;
                } else if (mHandleLongPress){
                    startLongPressWatcher(pos);
                }
                break;
                }
            case Qt::GestureFinished:
                {
                int hitItemindex = -1;
                HgWidgetItem* hitItem = getItemAt(pos,hitItemindex);
                handleGesture = handleTapAction(pos, hitItem, hitItemindex);
                if (mEmitScrollingEnded) {
                    mEmitScrollingEnded = false;
                    emit scrollingEnded();
                }
                }
            case Qt::GestureUpdated:
            case Qt::GestureCanceled:
            default:
                stopLongPressWatcher();
                break;
            }
        
        handleGesture = true;
    } else {
        if (state == Qt::GestureFinished) {
            mSpring.resetVelocity();
            mSpring.cancel();
            update();
            emit emptySpacePressed();
        }
    }    
    return handleGesture;
}

bool HgGridContainer::handleLongTap(Qt::GestureState state, const QPointF &pos)
{
    // HContainer handles the long tap if there is item at the pos.
    bool handled = HgContainer::handleLongTap(state,pos);
    if (!handled && state == Qt::GestureFinished) {
        mSpring.resetVelocity();
        mSpring.cancel();
        update();
        emit emptySpacePressed();
    }
    return handled;
}

void HgGridContainer::setPinchEnabled(bool pinchEnabled)
{
    if (mPinchEnabled != pinchEnabled) {
        mPinchEnabled = pinchEnabled;
        if (mPinchEnabled) {
            grabGesture(Qt::PinchGesture);
            iFadeAnimation.setTargetObject(this);
            iFadeAnimation.setPropertyName("opacity");
            iFadeAnimation.setDuration(500);
            iFadeAnimation.setStartValue(1.0);
            iFadeAnimation.setEndValue(0.0);
            connect(&iFadeAnimation, SIGNAL(finished()), SLOT(effectFinished()));

        } else {
            iFadeAnimation.stop();
            setOpacity(1);
            disconnect(&iFadeAnimation,SIGNAL(finished()), this, SLOT(effectFinished()));
            ungrabGesture(Qt::PinchGesture);
        }
    }
}

bool HgGridContainer::pinchEnabled() const
{
    return mPinchEnabled;
}

void HgGridContainer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->type() == QEvent::GraphicsSceneMousePress)
    {
        //reset, just in case
        mPinchingOngoing = false;
        mPinchEndAlreadyHandled = false;
        mTempImageHeightForLineGrid = -1;
        mTempImageHeightFinal = -1;
    }
}

void HgGridContainer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        handlePinchEnd();
    }
}

void HgGridContainer::gestureEvent(QGestureEvent* event)
{
    if (mItems.count() == 0) {
        // we have no items so no need to handle the gesture.
        event->ignore();
        return;
    }
    
    if (!mPinchingOngoing) {
        HgContainer::gestureEvent(event);
    }

    bool eventHandled(false);
    
    QGesture* pinchGesture = event->gesture(Qt::PinchGesture);
    if(mPinchEnabled && !mReactToOnlyPanGestures && pinchGesture)
    {
        HbPinchGesture* pinch = static_cast<HbPinchGesture *>(pinchGesture);
        switch (pinch->state())
            {
            case Qt::GestureUpdated:
                handlePinchUpdate( pinch );
                break;
            case Qt::GestureStarted:                
                mTempRowCount = -1;         //reset, just in case
                mTempImageHeightForLineGrid = -1;  //reset, just in case
                mTempImageHeightFinal = -1;   //reset, just in case
                iTargetRowCountList.clear();
                mPinchingOngoing = true;
                mPinchEndAlreadyHandled = false;
                break;
            case Qt::GestureCanceled:
                mPinchingOngoing = false;
                mPinchEndAlreadyHandled = true;
                update();   //redraw
                break;
            case Qt::GestureFinished:
                handlePinchEnd();
                break;
            default:
                break;                
            }
        
        eventHandled = true;
    }    
    
    eventHandled ? event->accept() : event->ignore();
}

void HgGridContainer::handlePinchUpdate(HbPinchGesture* pinch)
{
    // while user is scaling down scale factor changes from 1 -> 0. When scaling up scale factor
    // changes from 1 -> x
    qreal change = 0.0;
    qreal scaleFactor = pinch->scaleFactor();// + pinch->lastScaleFactor())/2;
    if (scaleFactor < 1) {
        change = -5*(1-scaleFactor);
    } else {
        change = scaleFactor - 1;
    }    

    qreal wannaBeRowCount = mRenderer->getRowCount() + change*1.5;
    
    int minRowCount = scrollDirection() == Qt::Horizontal ? 
        mHorizontalPinchLevels.first : mVerticalPinchLevels.first;
    int maxRowCount = scrollDirection() == Qt::Horizontal ? 
        mHorizontalPinchLevels.second : mVerticalPinchLevels.second;
    
    if(wannaBeRowCount < minRowCount) {
        wannaBeRowCount = minRowCount;
    }
    else if(wannaBeRowCount > maxRowCount) {
        wannaBeRowCount = maxRowCount;
    }

    mTempRowCount = (int)wannaBeRowCount;

    while (iTargetRowCountList.count() >= 4) {
        iTargetRowCountList.dequeue();
    }            
        
    iTargetRowCountList.enqueue(wannaBeRowCount);
    
    
    qreal averageRowCount = 0;
    int count = iTargetRowCountList.count();
    if (count >= 2 ) {
        averageRowCount += iTargetRowCountList.at(count-1);
        averageRowCount += iTargetRowCountList.at(count-2);
        averageRowCount /= 2;
    } else {
        averageRowCount = wannaBeRowCount;
    }
    
    if (scrollDirection() == Qt::Horizontal ) {
        int centerAreaheight = mUserItemSize.height()*mHorizontalRowCount; 
        mTempImageHeightForLineGrid = (centerAreaheight - ((int)averageRowCount + 1) * mRenderer->getSpacing().height()) / averageRowCount;
    } else {
        mTempImageHeightForLineGrid = (rect().width() - ((int)averageRowCount + 1) * mRenderer->getSpacing().width()) / averageRowCount;
    }
    
    update();   //redraw
}

void HgGridContainer::handlePinchEnd()
{
    if(mPinchingOngoing && !mPinchEndAlreadyHandled) {
        mPinchingOngoing = false;
        mPinchEndAlreadyHandled = true;
        
        qreal averageRowCount = 0;
        int count = iTargetRowCountList.count();
        while (!iTargetRowCountList.isEmpty()) {
            qreal value = iTargetRowCountList.dequeue();
            averageRowCount += value;
        }
        
        averageRowCount /= count;

        qreal temp = floorf(averageRowCount);
        averageRowCount = (averageRowCount - temp > 0.5f) ? ceilf(averageRowCount) : temp;
        mTempRowCount = averageRowCount;
        
        // change the row count if it has been changed by pinching
        if ( (mTempRowCount != -1) 
             && (mTempRowCount != mRenderer->getRowCount()) ) {
        
            if (scrollDirection() == Qt::Horizontal ) {
                int centerAreaheight = mUserItemSize.height()*mHorizontalRowCount; 
                mTempImageHeightFinal = (centerAreaheight - ((int)mTempRowCount + 1) * mRenderer->getSpacing().height()) / (int)mTempRowCount;
            } else {
                mTempImageHeightFinal = (rect().width() - ((int)mTempRowCount + 1) * mRenderer->getSpacing().width()) / (int)mTempRowCount;
            }
                            
            mTargetRowCount = mTempRowCount;
            mTargetImageSize = QSizeF(mTempImageHeightFinal,mTempImageHeightFinal);
            iFadeAnimation.setDirection(QAbstractAnimation::Forward);
            iFadeAnimation.start();
        }
    }
}

bool HgGridContainer::event(QEvent *e) 
{    
    if (e->type() == QEvent::TouchBegin)
    {
        // The TouchBegin event must be accepted (i.e. return true) to be able to receive Pinch events.
        return true;
    }
    else if(e->type() == QEvent::Gesture)
    {
        // Since pinch gesture is not forwarded to 
        // gestureEvent function so lets handle it here. 
        QGestureEvent* gesture = static_cast<QGestureEvent*>(e);
        gestureEvent(gesture);
        return true;
    }

    return QGraphicsObject::event(e);
}

void HgGridContainer::effectFinished()
{
    if (iFadeAnimation.direction() == QAbstractAnimation::Forward) {
        mRenderer->setRowCount(mTargetRowCount, mTargetImageSize);
        mRenderer->setImageSize(mTargetImageSize);
//        mSpring.setDamping( mTargetRowCount != 3 ? 
//            KSpringDampingScrolling*(mTargetRowCount-3)*4 : KSpringDampingScrolling );
//        mSpring.setK( mTargetRowCount != 3 ? 
//            KSpringKScrolling/((mTargetRowCount-3)*4) : KSpringKScrolling );
        scrollTo(mSelectionModel->currentIndex());
        iFadeAnimation.setDirection(QAbstractAnimation::Backward);
        iFadeAnimation.start();
    }
}

void HgGridContainer::setRowCount(int count, Qt::Orientation scrollDirection)
{
    if (scrollDirection == Qt::Horizontal) {
        mHorizontalRowCount = count;
    } else {
        mVerticalColumnCount = count;
    }
}

int HgGridContainer::rowCount(Qt::Orientation scrollDirection) const
{
    return scrollDirection == Qt::Horizontal ? mHorizontalRowCount : mVerticalColumnCount;
}

void HgGridContainer::setOrientation(Qt::Orientation orientation, bool animate)
{
    HgContainer::setOrientation(orientation, animate);
    
    if (orientation == Qt::Horizontal) {
            mRenderer->enableReflections(false);
            mRenderer->setImageSize(mUserItemSize);
        if (currentRowCount() != mHorizontalRowCount) {
            mRenderer->setRowCount(mHorizontalRowCount, mUserItemSize, false);
            scrollTo(mSelectionModel->currentIndex());
        }
    } else {
        mRenderer->enableReflections(false);
        mRenderer->setImageSize(mUserItemSize);
        if (currentRowCount() != mVerticalColumnCount) {
            mRenderer->setRowCount(mVerticalColumnCount, mUserItemSize, false);        
            scrollTo(mSelectionModel->currentIndex());
        }
    }
}

void HgGridContainer::setPinchLevels(QPair<int,int> levels, Qt::Orientation scrollDirection)
{
    if (scrollDirection == Qt::Horizontal) {
        mHorizontalPinchLevels = levels;
    } else {
        mVerticalPinchLevels = levels;
    }
}

QPair<int,int> HgGridContainer::pinchLevels(Qt::Orientation scrollDirection) const
{
    return scrollDirection == Qt::Horizontal ? 
        mHorizontalPinchLevels : mVerticalPinchLevels;
}

// End of file
