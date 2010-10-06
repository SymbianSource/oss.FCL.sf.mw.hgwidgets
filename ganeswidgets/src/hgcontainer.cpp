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
#include <HbMainWindow>
#include <HbCheckBox>
#include <HbGridViewItem>
#include <HbGridView>
#include <HbIconItem>
#include <QAbstractItemModel>
#include <HbTapGesture>

#include "hgcontainer.h"
#include "hgmediawallrenderer.h"
#include "hgquad.h"
#include "hgimage.h"
#include "hgwidgetitem.h"
#include "trace.h"
#include "hgquadrenderer.h"
#include "hglongpressvisualizer.h"


static const qreal KSpringKScrolling(50.0);
static const qreal KSpringKScrollBar(50.0);
static const qreal KSpringDampingScrolling(20.0);
static const qreal KSpringDampingScrollBar(20.0);
static const qreal KFramesToZeroVelocity(60.0);
static const int   KLongTapDuration(400);


HgContainer::HgContainer(QGraphicsItem* parent) :
    HbWidget(parent),
    mQuadRenderer(0),
    mRenderer(0),
    mTapCount(0),
    mAnimateUsingScrollBar(false),
    mSelectionMode(HgWidget::NoSelection),
    mSelectionModel(0),
    mMarkImageOn(0),
    mMarkImageOff(0),
    mSpringVelAtDragStart(0),
    mDragged(false),
    mFramesDragged(0),
    mHitItemView(NULL),
    mLongPressVisualizer(NULL),
    mLongPressTimer(NULL),
    mHitItemIndex(-1),
    mItemSizePolicy(HgWidget::ItemSizeAutomatic),
    mOrientation(Qt::Vertical),
    mDelayedScrollToIndex(),
    mIgnoreGestureAction(false),
    mHandleLongPress(false),
    mReflectionsEnabled(false)
{
    FUNC_LOG;

    setFlag(QGraphicsItem::ItemHasNoContents, false);

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::TapGesture);
}

HgContainer::~HgContainer()
{
    FUNC_LOG;

    qDeleteAll(mItems);
    mItems.clear();
    delete mMarkImageOn;
    delete mMarkImageOff;
    delete mRenderer;
}

void HgContainer::setItemCount(int itemCount)
{
    FUNC_LOG;

    qDeleteAll(mItems);
    mItems.clear();
    for (int i=0; i<itemCount; i++) {
        HgWidgetItem* item = new HgWidgetItem(mQuadRenderer);

        if ( mReflectionsEnabled && (mRenderer->coverflowModeEnabled() || 
                (scrollDirection() == Qt::Horizontal))) {
            // Reflection image is created only for bottom row in grid mode.
            item->enableReflection((i+1)%currentRowCount() == 0);
        }
        mItems.append(item);
    }
}

// TODO: This does exactly the same as HgContainer::imageCount(), should this be thus removed?
int HgContainer::itemCount() const
{
    return mItems.count();
}

int HgContainer::currentRowCount() const
{
    return mRenderer ? mRenderer->getRowCount() : 0;
}

QList<HgWidgetItem*> HgContainer::items() const
{
    return mItems;
}

HgWidgetItem* HgContainer::itemByIndex(const QModelIndex& index) const
{
    foreach (HgWidgetItem* item, mItems) {
        if (item->modelIndex() == index)
            return item;
    }

    return 0;
}

HgWidgetItem* HgContainer::itemByIndex(const int& index) const
{
    if (mItems.count() > index && index >= 0)
        return mItems.at(index);
    else
        return 0;
}

/*!
    Changes the selection model of the container.
    Ownership is not transferred.
    Widget is redrawn to make new selection visible.
*/
void HgContainer::setSelectionModel(QItemSelectionModel *selectionModel, const QModelIndex &defaultItem)
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(selectionModel); // Parameter is always a valid QItemSelectionModel

    if (mSelectionModel == selectionModel) return;

    bool defaultCurrentSet(false);

    if (!selectionModel->currentIndex().isValid()) { // If there is valid current item, do not change it
        if (!mSelectionModel && defaultItem.isValid()) { // mSelectionModel is 0 when called first time
            selectionModel->setCurrentIndex(defaultItem, QItemSelectionModel::Current);
            defaultCurrentSet = true;
        }
        else if (mSelectionModel && mSelectionModel->currentIndex().isValid()) {
            selectionModel->setCurrentIndex(mSelectionModel->currentIndex(),
            QItemSelectionModel::Current);
        }
    }

    mSelectionModel = selectionModel;

    if (mSelectionModel->currentIndex().isValid() && !defaultCurrentSet) {
        scrollTo(mSelectionModel->currentIndex());
    }
    else {
        update();
    }
}

/*!
    Returns the selection model of the container.
    Ownership is not transferred.
*/
QItemSelectionModel *HgContainer::selectionModel() const
{
    FUNC_LOG;

    return mSelectionModel;
}

/*!
    Changes the selection mode of the container (no selection/multiselection).
*/
void HgContainer::setSelectionMode(HgWidget::SelectionMode mode, bool resetSelection)
{
    FUNC_LOG;

    if (mSelectionMode != mode) {
        mSelectionMode = mode;

        if (mSelectionModel && resetSelection) {
            mSelectionModel->clearSelection();
            update();
        }
    }
}

/*!
    Returns the selection mode of the container (no selection/multiselection).
*/
HgWidget::SelectionMode HgContainer::selectionMode() const
{
    FUNC_LOG;

    return mSelectionMode;
}

void HgContainer::dimensions(qreal &screenSize, qreal &worldSize)
{
    const QRectF containerRect(rect());

    if (scrollDirection()== Qt::Vertical) {
        // assume we are in portrait mode, ofcource this might not be the case
        screenSize = containerRect.height()/(mRenderer->getImageSize().height() + mRenderer->getSpacing().height());
        worldSize = worldWidth();
    }
    else{
        screenSize = containerRect.width()/(mRenderer->getImageSize().width() + mRenderer->getSpacing().width());
        worldSize = worldWidth();
    }
}

Qt::Orientation HgContainer::orientation() const
{
    FUNC_LOG;

    return mOrientation;
}

void HgContainer::setOrientation(Qt::Orientation orientation, bool animate)
{
    FUNC_LOG;

    mOrientation = orientation;
    mRenderer->setOrientation(orientation);
    mRenderer->setScrollDirection(orientation, animate);
    if (mSpring.isActive()) {
        // Need to stop scrolling.
        mSpring.cancel();
    }
}

void HgContainer::scrollToPosition(qreal value, bool animate)
{
    FUNC_LOG;

    scrollToPosition(QPointF(value*worldWidth(), 0), animate);
}

void HgContainer::scrollToPosition(const QPointF& pos, bool animate)
{
    FUNC_LOG;

    mAnimateUsingScrollBar = animate;
    initSpringForScrollBar();

    if (animate)
        mSpring.animateToPos(pos);
    else
        mSpring.gotoPos(pos);
}

void HgContainer::scrollTo(const QModelIndex &index)
{
    FUNC_LOG;
    INFO("Scrolling to" << index);

    if (index.isValid() && mRenderer->getRowCount() > 0 ) {

        QRectF containerRect(rect());
        if (containerRect.isNull()) {
            // Container hasn't been resized yet. We need to know the container
            // size before we can calculate if index we are scrolling to is valid.
            // Store scrollTo index and scrolling is performed when container is resized.
            mDelayedScrollToIndex = index;
            return;
        }

        // Container has some size. We can try to calculate if scrollto index is valid.
        // ScrollTo index will be the top item in grid and left item on coverflow.

        if (!mRenderer->coverflowModeEnabled()) {
            // Grid case
            int itemsOnScreen = 0;
            if (scrollDirection()== Qt::Vertical) {
                const int rowHeight = mRenderer->getImageSize().height() + mRenderer->getSpacing().height();
                itemsOnScreen = containerRect.height()/rowHeight;
                if ((int)containerRect.height()%rowHeight) {
                    itemsOnScreen++;
                }
                itemsOnScreen *= currentRowCount();
                if (itemsOnScreen + index.row() > mItems.count()) {
                    int newItem = mItems.count()-itemsOnScreen;

                    if (mItems.count()%currentRowCount())
                        newItem += currentRowCount() - (mItems.count()%currentRowCount());
                    if (newItem < 0)
                        newItem = 0;

                    scrollToPosition(QPointF(newItem/mRenderer->getRowCount(),0), false);
                } else {
                    scrollToPosition(QPointF(index.row()/mRenderer->getRowCount(), 0), false);
                }
            } else {
                // Scrolldirection is horizontal
                const int rowWidth = mRenderer->getImageSize().width() + mRenderer->getSpacing().width();
                itemsOnScreen = containerRect.width()/rowWidth;
                if ((int)containerRect.height()%rowWidth) {
                    itemsOnScreen++;
                }
                itemsOnScreen *= currentRowCount();
                if (itemsOnScreen + index.row() > mItems.count()) {
                    int newItem = mItems.count()-itemsOnScreen;

                    if (mItems.count()%currentRowCount())
                        newItem += currentRowCount() - (mItems.count()%currentRowCount());
                    if (newItem < 0) newItem = 0;

                    scrollToPosition(QPointF(newItem/mRenderer->getRowCount(),0), false);
                } else {
                    scrollToPosition(QPointF(index.row()/mRenderer->getRowCount(), 0), false);
                }
            }
            updateBySpringPosition();
        } else {
            // Coverflow case. TODO, this will need some finetuning.
            scrollToPosition(QPointF(index.row()/mRenderer->getRowCount(), 0), false);
            updateBySpringPosition();
        }
    }
}

void HgContainer::itemDataChanged(const QModelIndex &firstIndex, const QModelIndex &lastIndex)
{
    FUNC_LOG;

    // TODO, fix this
    int columns = firstIndex.model()->columnCount(QModelIndex());

    // Check this!!
    int index = columns*firstIndex.row()+firstIndex.column();
    int index2 = columns*lastIndex.row()+lastIndex.column();

    // convert indeces to match one dimensional item array
    itemDataChanged( index, index2 );
}

void HgContainer::addItems(int start, int end)
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mSelectionModel);

    int first = qBound(0, start, mItems.count()-1);
    int last = qBound(0, end, mItems.count()-1);
    for (int i = 0; i <= end-start; i++) {
        HgWidgetItem* item = new HgWidgetItem(mQuadRenderer);
        mItems.insert(start, item);
    }
    scrollTo(mSelectionModel->currentIndex());
}

void HgContainer::removeItems(int start, int end)
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mSelectionModel);

    int first = qBound(0, start, mItems.count()-1);
    int last = qBound(0, end, mItems.count()-1);
    for (int i = last; i >= first; i--) {
        delete mItems.at(i);
        mItems.removeAt(i);
    }
    scrollTo(mSelectionModel->currentIndex());
}

void HgContainer::moveItems(int start, int end, int destination)
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mSelectionModel);

    int first = qBound(0, start, mItems.count()-1);
    int last = qBound(0, end, mItems.count()-1);
    int target = qBound(0, destination, mItems.count()-1);

    if (target < first) {
        for (int i = 0; i <= last-first; i++) {
            mItems.move(first+i, target+i);
        }
    }
    else if (target > last) {
        for (int i = 0; i <= last-first; i++) {
            mItems.move(last-i, target);
        }
    }
    // else do nothing
    scrollTo(mSelectionModel->currentIndex());
}

// TODO: This does exactly the same as HgContainer::itemCount(), should this be thus removed?
int HgContainer::imageCount() const
{
    return mItems.count();
}

const HgImage *HgContainer::image(int index) const
{
    return mItems[index]->image();
}

int HgContainer::flags(int index) const
{
    if (index >= 0 && index < itemCount()) {
        if (mSelectionMode != HgWidget::NoSelection) {
            if (mSelectionModel && 
                    mSelectionModel->model() &&
                    mSelectionModel->isSelected(mSelectionModel->model()->index(index, 0))) {
                return 1; // TODO: Assign flag to mark indicator
            } else
                return 2;
        }
    }
    return 0;
}

const HgImage *HgContainer::indicator(int flags)
{
    if (flags != 0 && (!mMarkImageOff || !mMarkImageOn)) {
        // indicators haven't been loaded yet.
        loadIndicatorGraphics();
    }
    
    if (flags & 1) {
        return mMarkImageOn;
    }
    else if (flags & 2) {
        return mMarkImageOff;
    }

    return 0;
}

void HgContainer::updateBySpringPosition()
{
    // spring works always in one dimension, that is, x coord.
    qreal pos = mSpring.pos().x();

    onScrollPositionChanged(pos);

    emit scrollPositionChanged(pos, mAnimateUsingScrollBar);
    update();
}

void HgContainer::redraw()
{
    update();
}

void HgContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // update spring position at paint if needed,
    // this is hack for scrollbar, since dragging it
    // causes also paint events in here
    if (mSpring.updatePositionIfNeeded())
    {
        qreal pos = mSpring.pos().x();
        onScrollPositionChanged(pos);
        emit scrollPositionChanged(pos, true);
    }

    QPainter::RenderHints hints = painter->renderHints();
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    // interpolate spring velocity towards zero, this is done
    // so that spring velocity for rendering doesn't drop directly to
    // zero when dragging starts
    qreal springVel = mSpring.velocity().x();
    if (mDragged) {
        qreal t = qBound(mFramesDragged / KFramesToZeroVelocity, 0.0f, 1.0f);
        springVel = mSpringVelAtDragStart * (1.0f - t);
        mFramesDragged++;
    }

    // setup rendering and draw the current view
    mRenderer->setCameraDistance(getCameraDistance(springVel));
    mRenderer->setCameraRotationY(getCameraRotationY(springVel));
    mRenderer->draw(mSpring.startPos(), mSpring.pos(), mSpring.endPos(),
                    springVel, painter, sceneTransform(), rect());

    painter->setRenderHint(QPainter::SmoothPixmapTransform, (hints.testFlag(QPainter::SmoothPixmapTransform)) );
}

void HgContainer::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    HbWidget::resizeEvent(event);

    if (mDelayedScrollToIndex.isValid()) {
        scrollTo(mDelayedScrollToIndex);
        // set scrollto index to invalid value.
        mDelayedScrollToIndex = QModelIndex();
    }
    // Indicator size depends from the item size so
    // indicators need to be updated. Indicators are created
    // in runtime only for need so if they dont exists they aren't created.
    loadIndicatorGraphics(true);
}

// this needs to be implemented for gesture framework to work
void HgContainer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void HgContainer::gestureEvent(QGestureEvent *event)
{
    FUNC_LOG;

    if (mItems.count() == 0) {
        // we have no items so no need to handle the gesture.
        event->ignore();
        return;
    }
    
    bool eventHandled(false);
    // Event may contain more than one gesture type
    HbTapGesture *tap = 0;
    if (QGesture *gesture = event->gesture(Qt::TapGesture)) {
        tap = static_cast<HbTapGesture *>(event->gesture(Qt::TapGesture));
        if (mHandleLongPress && tap->tapStyleHint() == HbTapGesture::TapAndHold) {
            eventHandled = handleLongTap(tap->state(),
                    mapFromScene(event->mapToGraphicsScene(tap->hotSpot())));
        
        } else {
            eventHandled = handleTap(tap->state(),
                    mapFromScene(event->mapToGraphicsScene(tap->hotSpot())));
        }
    }
    if (QGesture *pan = event->gesture(Qt::PanGesture)) {
        eventHandled = handlePanning(static_cast<QPanGesture*>(pan));
    } else if( mIgnoreGestureAction && tap && tap->state() == Qt::GestureCanceled ) {
        // user has tapped or long pressed in grid while scrolling so we need to
        // stop the 3d effect.
        mSpring.resetVelocity();
        update();
        mIgnoreGestureAction = false;
    }

    eventHandled ? event->accept() : event->ignore();    
}

void HgContainer::init(Qt::Orientation scrollDirection)
{
    FUNC_LOG;

    setFlag( QGraphicsItem::ItemClipsToShape, true );    
    
    mRenderer = createRenderer(scrollDirection);
    mOrientation = scrollDirection;

    mQuadRenderer = mRenderer->getRenderer();

    connect(&mSpring, SIGNAL(updated()), SLOT(updateBySpringPosition()));
    connect(&mSpring, SIGNAL(started()), SIGNAL(scrollingStarted()));
    connect(&mSpring, SIGNAL(started()), SLOT(onScrollingStarted()));
    connect(&mSpring, SIGNAL(ended()), SIGNAL(scrollingEnded()));
    connect(&mSpring, SIGNAL(ended()), SLOT(onScrollingEnded()));
    connect(mRenderer, SIGNAL(renderingNeeded()), SLOT(redraw()));

}

qreal HgContainer::worldWidth() const
{
    return (qreal)mRenderer->getWorldWidth();
}

void HgContainer::initSpringForScrollBar()
{
    FUNC_LOG;

    mSpring.setDamping(KSpringDampingScrollBar);
    mSpring.setK(KSpringKScrollBar);
}

void HgContainer::initSpringForScrolling()
{
    FUNC_LOG;

    mSpring.setDamping(KSpringDampingScrolling);
    mSpring.setK(KSpringKScrolling);
}

void HgContainer::boundSpring()
{
    FUNC_LOG;

    qreal x = mSpring.endPos().x();
    if (mRenderer->coverflowModeEnabled()) {
        qreal i = floorf(x);
        x = (x - i > 0.5f) ? ceilf(x) : i;
    }

    mSpring.animateToPosAfterPanning(QPointF(x, 0), worldWidth());

}

bool HgContainer::handlePanning(QPanGesture *gesture)
{
    mAnimateUsingScrollBar = false;
    initSpringForScrolling();

    qreal pos = mSpring.pos().x();
    qreal delta(0);
    qreal itemSide(0);

    if (mOrientation == mRenderer->getScrollDirection()) {
        delta = gesture->delta().y();
    }
    else {
        delta = gesture->delta().x();
    }

    if (mRenderer->getScrollDirection() == Qt::Vertical)
        itemSide = mRenderer->getImageSize().height()+mRenderer->getSpacing().height();
    else
        itemSide = mRenderer->getImageSize().width()+mRenderer->getSpacing().width();

    if (gesture->state() == Qt::GestureStarted) {
        mOffsetAtDragStart = gesture->offset();
    }
    else if (gesture->state() == Qt::GestureUpdated) {
        QPointF offset = gesture->offset();
        QPointF offsetDelta = offset - mOffsetAtDragStart;
        if (!mDragged && (qAbs(offsetDelta.x()) > 8 ||
            qAbs(offsetDelta.y()) > 8)) {
            mDragged = true;
            mDrag.reset(delta, mSpring.pos().x());
            mDragged = true;
            mSpringVelAtDragStart = mSpring.velocity().x();
            mFramesDragged = 0;
        }

        if (mDragged)
        {
            emit scrollingStarted();

            qreal newPosition = mDrag.update(delta, pos, itemSide);
            if (qAbs(newPosition - mSpring.pos().x()) > 0.01f) {
                mSpring.gotoPos(QPointF(newPosition, 0));
                if (mRenderer->coverflowModeEnabled()) {
                    emit scrollPositionChanged(newPosition,true);
                    update();                
                } else {
                    updateBySpringPosition();
                }
            }
        }
    }
    else if (mDragged && gesture->state() == Qt::GestureFinished) {
        mDrag.update(delta, pos, itemSide);
        mDragged = false;
        qreal newPos(0);
        if (mDrag.finish(pos, mRenderer->coverflowModeEnabled(), newPos)) {
            
            mSpring.animateToPosAfterPanning(QPointF(newPos, 0), worldWidth());
            
            HgWidgetItem* item = itemByIndex(newPos);
            if (item && item->modelIndex() != mSelectionModel->currentIndex()) {
            //    mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
            }
        }
        else {
            boundSpring();
        }
    }
    else if(!mDragged && gesture->state() == Qt::GestureFinished) {
        if (!mRenderer->coverflowModeEnabled()) {
            mSpring.resetVelocity();
            update();
        }
    }
    else if (gesture->state() == Qt::GestureCanceled) {
        boundSpring();
    }

    return true;
}

bool HgContainer::handleLongTap(Qt::GestureState state, const QPointF &pos)
{
    FUNC_LOG;

    bool handleGesture = false;
    
    if (hasItemAt(pos)) {

        switch (state) 
            {
            case Qt::GestureUpdated:
                {
                int hitItemIndex = -1;
                HgWidgetItem* hitItem = getItemAt(pos,hitItemIndex);
                handleLongTapAction(pos,hitItem, hitItemIndex);
                }
            case Qt::GestureStarted:
            case Qt::GestureCanceled:
            case Qt::GestureFinished:
            default:
                stopLongPressWatcher();
                break;                
            }
        handleGesture = true;
    } else {
        mIgnoreGestureAction = true;        
    }

    return handleGesture;
}


bool HgContainer::getItemPoints(int index, QPolygonF& points)
{
    return mRenderer->getItemPoints(index, points);
}

QList<QModelIndex> HgContainer::getVisibleItemIndices() const
{
    QList<HgQuad*> quads = mRenderer->getVisibleQuads();
    QList<QModelIndex> result;
    for (int i = 0; i < quads.count(); i++) {
        bool ok;
        int index = quads.at(i)->userData().toInt(&ok);
        HgWidgetItem *item = itemByIndex(index);
        if (item)
            result.append(item->modelIndex());
    }
    qSort(result);
    return result;
}

void HgContainer::itemDataChanged(const int &firstIndex, const int &lastIndex)
{
    FUNC_LOG;
    
    int firstItemOnScreen = 0, lastItemOnScreen = 0;
    firstItemOnScreen = mSpring.pos().x();
    firstItemOnScreen *= currentRowCount();

    int itemsOnScreen = mRenderer->getVisibleQuads().count();
    lastItemOnScreen = firstItemOnScreen+itemsOnScreen;

    if ( itemsOnScreen == 0 || (firstIndex >= firstItemOnScreen && firstIndex < lastItemOnScreen) ||
        (lastIndex >= firstItemOnScreen && lastIndex < lastItemOnScreen)) {
        update();
    }    
}

void HgContainer::selectItem(int index)
{
    Q_UNUSED(index)
    // TODO: replace this with own selection implementation
/*    if (index < 0 && index >= mItems.count())
        return;
    
    mHitItemIndex = index;
    
    if (mHitItemView)
    {
        delete mHitItemView;
        mHitItemView = NULL;
    }
    
    mHitItemView = new HbGridViewItem(this);
    mHitItemView->setVisible(false);
    mHitItemView->setPos(QPointF(0,0));
    mHitItemView->setPressed(true, false);

    const QImage& image = mItems[mHitItemIndex]->image()->getQImage();
    if (image.isNull())
    {
        mHitItemView->setVisible(false);
        return;
    }
    
    QPixmap pixmap = QPixmap::fromImage(image);        
    HbIcon icon(pixmap.scaled(mRenderer->getImageSize().toSize(), Qt::IgnoreAspectRatio));    
    QGraphicsItem* item = mHitItemView->style()->createPrimitive(HbStyle::P_GridViewItem_icon, mHitItemView);
    HbIconItem *iconItem = static_cast<HbIconItem*>(item);
    iconItem->setAlignment(Qt::AlignCenter);
    iconItem->setAspectRatioMode(Qt::IgnoreAspectRatio);    
    iconItem->setIcon(icon);

    mHitItemView->resize(mRenderer->getImageSize().width(),
        mRenderer->getImageSize().height());
        */
}

void HgContainer::updateSelectedItem()
{
    if (!mHitItemView || mHitItemIndex == -1)
        return;

    QPolygonF points;
    if (!getItemPoints(mHitItemIndex, points))
    {
        // the item was not rendered, we must hide
        // our qt item        
        mHitItemView->setVisible(false);
        return;
    }
    
    QRectF bounds = points.boundingRect();
    if (!(rect().contains(bounds) || rect().intersects(bounds)))
    {
        mHitItemView->setVisible(false);
        return;
    }

    QPolygonF img;
    img.append(QPointF(3,mHitItemView->boundingRect().height()-3));
    img.append(QPointF(mHitItemView->boundingRect().width()-3,mHitItemView->boundingRect().height()-3));
    img.append(QPointF(mHitItemView->boundingRect().width()-3,3));
    img.append(QPointF(3,3));

    QTransform t;
    QTransform::quadToQuad(img, points, t);

    //t.translate(50,50);
    bool bOk;
    mHitItemView->setTransform(t * this->transform().inverted(&bOk));
    mHitItemView->setVisible(true);
}

void HgContainer::unselectItem()
{
    mHitItemIndex = -1;
    if (mHitItemView)
    {
        mHitItemView->setVisible(false);
    }
}

void HgContainer::updateLongPressVisualizer()
{
    int elapsed = mLongTapDuration.elapsed();

    if (elapsed > 80)
    {
        int frame = 100.0f * qreal(elapsed - 80) / qreal(KLongTapDuration - 80);
        mLongPressVisualizer->setFrame(frame);
    }
}

bool HgContainer::hasItemAt(const QPointF& pos)
{
    int dummy;
    HgWidgetItem *item = getItemAt(pos, dummy);
    if (item) {
        return item->modelIndex().isValid();
    }
    return false;
}

HgWidgetItem* HgContainer::getItemAt(const QPointF& pos, int& index)
{
    HgQuad* quad = mRenderer->getQuadAt(pos);
    if (quad)
    {
        bool ok;
        index = quad->userData().toInt(&ok);

        HgWidgetItem* item = itemByIndex(index);
        return item;
    }
    return NULL;
}

void HgContainer::startLongPressWatcher(const QPointF& pos)
{
    if (!mLongPressVisualizer)
    {
        mLongPressVisualizer = new HgLongPressVisualizer(this);
        mLongPressVisualizer->setZValue(zValue()+1);
    }

    mLongPressVisualizer->start(pos);

    if (!mLongPressTimer)
    {
        mLongPressTimer = new QTimer(this);
        QObject::connect(mLongPressTimer, SIGNAL(timeout()), this, SLOT(updateLongPressVisualizer()));
    }

    mLongPressTimer->start(20);

    mLongTapDuration.start();
}

void HgContainer::stopLongPressWatcher()
{
    if (mLongPressTimer && mLongPressVisualizer)
    {
        mLongPressTimer->stop();
        mLongPressVisualizer->stop();
    }
}

qreal HgContainer::getCameraDistance(qreal springVelocity)
{
    Q_UNUSED(springVelocity)
    return 0;
}

qreal HgContainer::getCameraRotationY(qreal springVelocity)
{
    Q_UNUSED(springVelocity)
    return 0;
}

bool HgContainer::handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(pos)
    Q_UNUSED(hitItem)
    Q_UNUSED(hitItemIndex)
    return false;
}

bool HgContainer::handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(pos)
    Q_UNUSED(hitItem)
    Q_UNUSED(hitItemIndex)
    return false;
}

void HgContainer::onScrollPositionChanged(qreal pos)
{
    Q_UNUSED(pos)
}

void HgContainer::onScrollingStarted()
{
    // By default do nothing
}

void HgContainer::onScrollingEnded()
{
    // By default do nothing
}

void HgContainer::setDefaultImage(QImage defaultImage)
{
    HgQuadRenderer *renderer = mRenderer->getRenderer();
    if (renderer) {
        QImage temp = defaultImage;
        if(!temp.isNull()) {
            temp = defaultImage.scaled(mRenderer->getImageSize().toSize());
        }
        renderer->setDefaultImage(QPixmap::fromImage(temp));
    }
}

void HgContainer::setItemSizePolicy(HgWidget::ItemSizePolicy policy)
{
    if (policy != mItemSizePolicy)
    {
        mItemSizePolicy = policy;
        
        updateItemSizeAndSpacing();
    }
}

HgWidget::ItemSizePolicy HgContainer::itemSizePolicy() const
{
    return mItemSizePolicy;
}

void HgContainer::setItemSize(const QSizeF& size)
{
    mUserItemSize = size;
    updateItemSizeAndSpacing();
}

QSizeF HgContainer::itemSize() const
{
    return mRenderer->getImageSize();
}

void HgContainer::setItemSpacing(const QSizeF& spacing)
{
    mUserItemSpacing = spacing;
    updateItemSizeAndSpacing();
}

QSizeF HgContainer::itemSpacing() const
{
    return mRenderer->getSpacing();
}

void HgContainer::updateItemSizeAndSpacing()
{    
    if (mItemSizePolicy == HgWidget::ItemSizeUserDefined)
    {
        mRenderer->setImageSize(mUserItemSize);
        mRenderer->setSpacing(mUserItemSpacing);
    }
    // Indicator size depends from the item size so
    // indicators need to be updated. Indicators are created
    // in runtime only for need so if they dont exists they aren't created.
    loadIndicatorGraphics(true);
}

QSizeF HgContainer::getAutoItemSize() const
{
    return mUserItemSize;
}

QSizeF HgContainer::getAutoItemSpacing() const
{
    return mUserItemSpacing;
}

Qt::Orientation HgContainer::scrollDirection() const
{
    return mRenderer->getScrollDirection();
}

qreal HgContainer::scrollPosition() const
{
    return mSpring.pos().x();
}

void HgContainer::loadIndicatorGraphics(bool loadIfExists)
{
    if (loadIfExists && !mMarkImageOn && !mMarkImageOff) return;
    
    if (!mMarkImageOn) {
        mMarkImageOn = mQuadRenderer->createNativeImage();
    }        
    HANDLE_ERROR_NULL(mMarkImageOn);
    if (!mMarkImageOff) {
        mMarkImageOff = mQuadRenderer->createNativeImage();
    }
    
    const QSizeF newIndicatorSize = itemSize()/2;
    
    // Validate if loading marking icons is really needed by comparing the sizes.
    // Both marking icons have the same size so its enough to check one.
    if (mMarkImageOn && mMarkImageOn->width() == newIndicatorSize.width() &&
            mMarkImageOn->height() == newIndicatorSize.height() ) return;
    
    HANDLE_ERROR_NULL(mMarkImageOff);    
    HbIcon selectedIcon(QLatin1String("qtg_small_selected"));
    HbIcon unselectedIcon(QLatin1String("qtg_small_unselected"));
    selectedIcon.setSize(newIndicatorSize);
    unselectedIcon.setSize(newIndicatorSize);

    QPixmap selectedPixmap = selectedIcon.pixmap();
    QPixmap unselectedPixmap = unselectedIcon.pixmap();

    if (!selectedPixmap.isNull() && !unselectedPixmap.isNull()) {
        if (mMarkImageOn) {
            mMarkImageOn->setPixmap(selectedPixmap, true);
        }
        if (mMarkImageOff) {
            mMarkImageOff->setPixmap(unselectedPixmap, true);
        }
    }    
}

void HgContainer::setHandleLongPress(bool handheLongPress)
{
    // this is just a flag that is used in gesturehandling logic.
    mHandleLongPress = handheLongPress;
}

bool HgContainer::handleItemSelection(HgWidgetItem* item) 
{
    bool handled = true;
    switch(mSelectionMode)
        {
        case HgWidget::MultiSelection: 
            {
            mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
            INFO("Select:" << item->modelIndex().row());
            mSelectionModel->select(item->modelIndex(), QItemSelectionModel::Toggle);
            update(); // It would be enough to update the item
            break;
            }
        case HgWidget::SingleSelection:
            {
            mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
            INFO("Select:" << item->modelIndex().row());
            mSelectionModel->select(item->modelIndex(), QItemSelectionModel::ClearAndSelect);
            update(); // It would be enough to update the item
            break;
            }
        case HgWidget::ContiguousSelection: 
            {
            mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
            QModelIndex newSelected = item->modelIndex();
            QModelIndexList oldSelection = mSelectionModel->selectedIndexes();
            INFO("Select:" << newSelected.row());
            if (oldSelection.count() > 0 && !mSelectionModel->isSelected(newSelected)) {
                if (newSelected.row() < oldSelection.front().row()) {
                    mSelectionModel->select(QItemSelection(newSelected, oldSelection.back()),
                        QItemSelectionModel::Select);
                }
                else { // newSelected.row() > oldSelection.back().row()
                    mSelectionModel->select(QItemSelection(oldSelection.front(), newSelected),
                        QItemSelectionModel::Select);
                }
            }
            else {
                mSelectionModel->select(newSelected, QItemSelectionModel::Select);
            }
            update(); // It would be enough to update the item
            break;
            }
        default:
            handled = false;
            break;
        }

    return handled;
}

// This is added to be able to get feedback (audio and tactile)
// when tapping items in both mediawall and grid.
int HgContainer::type() const
{
    return Hb::ItemType_GridViewItem;
}

// EOF
