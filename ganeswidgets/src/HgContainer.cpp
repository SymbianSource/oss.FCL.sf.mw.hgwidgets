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
#include <hbgridviewitem>
#include <hbmainwindow>
#include "hgcontainer.h"
#include "hgmediawallrenderer.h"
#include "hgquad.h"
#include "hgvgquadrenderer.h"
#include "hgvgimage.h"
#include "hgwidgetitem.h"
#include "trace.h"

#include <hbgridviewitem>
#include <hbgridview>
#include <hbiconitem>
#include <qabstractitemmodel>
#include "hglongpressvisualizer.h"

static const qreal KSpringKScrolling(50.0);
static const qreal KSpringKScrollBar(10.0);
static const qreal KSpringDampingScrolling(20.0);
static const qreal KSpringDampingScrollBar(5.0);
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
    mMarkImage(0),
    mSpringVelAtDragStart(0),
    mDragged(false),
    mFramesDragged(0),
    mHitItemView(NULL),
    mLongPressVisualizer(NULL),
    mLongPressTimer(NULL),
    mHitItemIndex(NULL),
    mItemSizePolicy(HgWidget::ItemSizeUserDefined),
    mOrientation(Qt::Vertical)
{
    FUNC_LOG;

    grabGesture(Qt::PanGesture);
    grabGesture(Qt::TapGesture);
    grabGesture(Qt::TapAndHoldGesture);
}

HgContainer::~HgContainer()
{
    FUNC_LOG;

    qDeleteAll(mItems);
    mItems.clear();
    delete mMarkImage;
    delete mRenderer;
}

void HgContainer::setItemCount(int itemCount)
{
    FUNC_LOG;

    qDeleteAll(mItems);
    mItems.clear();
    for (int i=0; i<itemCount; i++) {
        HgWidgetItem* item = new HgWidgetItem(mQuadRenderer);
        mItems.append(item);
    }
}

int HgContainer::itemCount() const
{
    return mItems.count();
}

int HgContainer::rowCount() const
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
void HgContainer::setSelectionModel(QItemSelectionModel *selectionModel)
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(selectionModel); // Parameter is always a valid QItemSelectionModel

    if (mSelectionModel != selectionModel) {
        if (mSelectionModel) { // mSelectionModel is 0 when called first time
            mSelectionModel->disconnect(SIGNAL(currentChanged(QModelIndex,QModelIndex)), this);

            if (mSelectionModel->currentIndex().isValid() &&
                !(selectionModel->currentIndex().isValid())) {
                selectionModel->setCurrentIndex(mSelectionModel->currentIndex(),
                    QItemSelectionModel::Current);
            }
        }
        mSelectionModel = selectionModel;
        connect(mSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
                                 SLOT(updateByCurrentIndex(QModelIndex)));
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

    // TODO, fix logic
    if (containerRect.height() > containerRect.width()) {
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
    mRenderer->setOrientation(orientation, false);
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

    if (index.isValid() && mRenderer->getRowCount() > 0) {
        scrollToPosition(QPointF(index.row()/mRenderer->getRowCount(), 0), false);
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
        if (mSelectionModel && mSelectionModel->isSelected(mSelectionModel->model()->index(index, 0))) {
            return 1; // TODO: Assign flag to mark indicator
        }
    }
    return 0;
}

const HgImage *HgContainer::indicator(int flags) const
{
    if (flags & 1) {
        return mMarkImage;
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
    if (mSpring.updatePositionIfNeeded())
    {
        // spring works always in one dimension, that is, x coord.
        qreal pos = mSpring.pos().x();
        onScrollPositionChanged(pos);
        emit scrollPositionChanged(pos, true);        
    }

    QRectF vp = painter->viewport();
    QRectF rts = mapRectToScene(drawableRect());
    QRectF r;

    // transform rectangle to vg space &
    // rotate rendering according to orientation
    if (mOrientation == Qt::Horizontal) {
        r = QRectF(vp.width()-(rts.height()+rts.top()), rts.left(), rts.height(), rts.width());

        mRenderer->setRect(r);

        mRenderer->setCameraRotationZ(-90);
    }
    else {
        r = QRectF(rts.left(), vp.height()-(rts.height()+rts.top()), rts.width(), rts.height());
        mRenderer->setCameraRotationZ(0);

        mRenderer->setRect(r);
        
        if (!mSpring.isActive() && mSpring.pos().x() > worldWidth())
            boundSpring();
    }

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
                    springVel, painter);

}

void HgContainer::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    HbWidget::resizeEvent(event);
}

// this needs to be implemented for gesture framework to work
void HgContainer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void HgContainer::gestureEvent(QGestureEvent *event)
{
    FUNC_LOG;

    bool eventHandled(false);
    // Event may contain more than one gesture type
    if (QGesture *gesture = event->gesture(Qt::TapAndHoldGesture)) {
        QTapAndHoldGesture *tapAndHold = static_cast<QTapAndHoldGesture *>(gesture);
        if (handleLongTap(tapAndHold->state(),
                mapFromScene(event->mapToGraphicsScene(tapAndHold->position())))) {
        }
    }
    else if (QGesture *gesture = event->gesture(Qt::TapGesture)) {
        // Tap and hold is not working yet in HW, tap gesture is delivered instead
        QTapGesture *tap = static_cast<QTapGesture *>(gesture);
        eventHandled = handleTap(tap->state(),
            mapFromScene(event->mapToGraphicsScene(tap->position())));
    }
    else if (QGesture *pan = event->gesture(Qt::PanGesture)) {
        eventHandled = handlePanning(static_cast<QPanGesture*>(pan));
    }

    if (eventHandled) {
        event->accept();
        event->accept(Qt::TapAndHoldGesture);
        event->accept(Qt::TapGesture);
        event->accept(Qt::PanGesture);
    }
    else {
        event->ignore();
        event->ignore(Qt::TapAndHoldGesture);
        event->ignore(Qt::TapGesture);
        event->ignore(Qt::PanGesture);
    }
}

void HgContainer::init(Qt::Orientation scrollDirection)
{
    FUNC_LOG;

    mRenderer = createRenderer(scrollDirection);
    mOrientation = scrollDirection;

    mQuadRenderer = mRenderer->getRenderer();

    QImage markImage(":/images/mark.svg");
    if (markImage.isNull()) {
        ERROR("Failed to load :/images/mark.svg");
    }
    mMarkImage = mQuadRenderer->createNativeImage();
    HANDLE_ERROR_NULL(mMarkImage);
    if (mMarkImage) {
        mMarkImage->setImage(markImage);
    }
    
    connect(&mSpring, SIGNAL(updated()), SLOT(updateBySpringPosition()));
    connect(&mSpring, SIGNAL(ended()), SIGNAL(scrollingEnded()));
    connect(&mSpring, SIGNAL(started()), SIGNAL(scrollingStarted()));
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
    x = qBound(qreal(0), x, worldWidth());
    if (mRenderer->coverflowModeEnabled()) {
        qreal i = floorf(x);
        x = (x - i > 0.5f) ? ceilf(x) : i;
        mSpring.animateToPos(QPointF(x, 0));
    }

    mSpring.animateToPos(QPointF(x, 0));

}

bool HgContainer::handlePanning(QPanGesture *gesture)
{
    mAnimateUsingScrollBar = false;
    initSpringForScrolling();

    qreal pos = mSpring.pos().x();
    qreal delta(0);
    qreal itemSide(0);

    if (mOrientation == mRenderer->getOrientation()) {
        delta = gesture->delta().y();
    }
    else {
        delta = gesture->delta().x();
    }

    if (mRenderer->getOrientation() == Qt::Vertical)
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
            if (qAbs(newPosition - mSpring.pos().x()) > 0.01f)
            {
                mSpring.gotoPos(QPointF(newPosition, 0));
                emit scrollPositionChanged(newPosition,true);
                update();
            }
        }
    }
    else if (mDragged && gesture->state() == Qt::GestureFinished) {
        mDrag.update(delta, pos, itemSide);
        mDragged = false;
        qreal newPos(0);
        if (mDrag.finish(pos, mRenderer->coverflowModeEnabled(), newPos)) {
            mSpring.animateToPos(QPointF(qBound(qreal(0), newPos, worldWidth()), 0));
            HgWidgetItem* item = itemByIndex(newPos);
            if (item && item->modelIndex() != mSelectionModel->currentIndex()) {
            //    mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
            }
        }
        else {
            boundSpring();
        }
    }
    else if (gesture->state() == Qt::GestureCanceled) {
        boundSpring();
    }

    return true;
}

bool HgContainer::handleTap(Qt::GestureState state, const QPointF &pos)
{
    FUNC_LOG;

    if (state == Qt::GestureStarted) {
        if (hasItemAt(pos)) {
            mTapDuration.start();
            startLongPressWatcher(pos);
            return true;
        }
        return false;
    }
    else if (state == Qt::GestureCanceled)
    {
        stopLongPressWatcher();

        if (hasItemAt(pos)) {
            return true;
        }
        return false;
    }
    else if (state == Qt::GestureFinished) {
        stopLongPressWatcher();
        return handleItemAction(pos, mTapDuration.elapsed() > KLongTapDuration ? LongTap : NormalTap);
    }

    return false;
}

bool HgContainer::handleLongTap(Qt::GestureState state, const QPointF &pos)
{
    FUNC_LOG;

    if (hasItemAt(pos)) {
        mAnimateUsingScrollBar = false;
        initSpringForScrolling();

        if (state == Qt::GestureFinished) {
            handleItemAction(pos, LongTap);
        }
        return true;
    }

    return false;
}

/*!
    Handle tap, lang tap and double tap action.
    Finds out the item in the tap position and sends out suitable signal,
    Sets the item as the current item and in multiselection mode toggles the
    item selection status.
*/
bool HgContainer::handleItemAction(const QPointF &pos, ItemActionType action)
{
    FUNC_LOG;

    // If there is content, mSelectionModel must always exist - either default or client-provided
    if (!mSelectionModel) return false;

    mHitItem = getItemAt(pos, mHitItemIndex);
    if (mHitItem)
    {
        int index = mHitItemIndex;

        HgWidgetItem* item = itemByIndex(index);
        if (item && action != DoubleTap) {
            if (action == LongTap) {
                INFO("Long tap:" << item->modelIndex().row());
                mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);

                if (!mRenderer->coverflowModeEnabled())
                    selectItem();

                emit longPressed(item->modelIndex(), pos);
            }
            else if (mSelectionMode == HgWidget::MultiSelection) {
                mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
                INFO("Select:" << item->modelIndex().row());
                mSelectionModel->select(item->modelIndex(), QItemSelectionModel::Toggle);
                update(); // It would be enough to update the item
            }
            else if (mSelectionMode == HgWidget::SingleSelection) {
                mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
                INFO("Select:" << item->modelIndex().row());
                mSelectionModel->select(item->modelIndex(), QItemSelectionModel::ClearAndSelect);
                update(); // It would be enough to update the item
            }
            else if (mSelectionMode == HgWidget::ContiguousSelection) {
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
            }
            else {
                INFO("Tap:" << item->modelIndex().row());

                if (mRenderer->coverflowModeEnabled())
                {
                    if (qAbs(qreal(index) - mSpring.pos().x()) < 0.01f)
                    {
                        mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
                        emit activated(item->modelIndex());
                    }
                    else
                    {
                        mSpring.animateToPos(QPointF(index, 0));
                    }
                }
                else
                {
                    mSelectionModel->setCurrentIndex(item->modelIndex(), QItemSelectionModel::Current);
                    selectItem();
                    emit activated(item->modelIndex());
                }
            }
        }

        return true;
    }
    else {
        INFO("No quad at pos:" << pos);

        unselectItem();
        return false;
    }
}

bool HgContainer::getItemPoints(int index, QPolygonF& points)
{
    QPolygonF poly;
    if (!mRenderer->getItemPoints(index, poly))
        return false;

    bool invertible;
    QTransform t = qtToVgTransform().inverted(&invertible);

    points = t.map(poly);
    return true;
}

QList<QModelIndex> HgContainer::getVisibleItemIndices() const
{
    QList<HgQuad*> quads = mRenderer->getVisibleQuads();
    QList<QModelIndex> result;
    for (int i = 0; i < quads.count(); i++) {
        bool ok;
        int index = quads.at(i)->userData().toInt(&ok);
        result.append(itemByIndex(index)->modelIndex());
    }
    qSort(result);
    return result;
}

void HgContainer::itemDataChanged(const int &firstIndex, const int &lastIndex)
{
    FUNC_LOG;

    // if screen is frequently updated no need to update now.
    if (mSpring.isActive() || mDragged ) return;
    
    // TODO FIX THIS FUNCTION!!!!!!!!!!!!!!!!!!!!!!

    int firstItemOnScreen = 0, lastItemOnScreen = 0;
    firstItemOnScreen = mSpring.pos().x();
    firstItemOnScreen *= rowCount();

    int itemsOnScreen = mRenderer->getVisibleQuads().count();
    lastItemOnScreen = firstItemOnScreen+itemsOnScreen;

    if ((firstIndex >= firstItemOnScreen && firstIndex < lastItemOnScreen) ||
        (lastIndex >= firstItemOnScreen && lastIndex < lastItemOnScreen)) {
        update();
    }
}

void HgContainer::selectItem()
{
    // TODO: replace this with own selection implementation
    if (mHitItemIndex < 0 && mHitItemIndex >= mItems.count())
        return;
    
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
    }

    QPolygonF img;
    img.append(QPointF(3,mHitItemView->boundingRect().height()-3));
    img.append(QPointF(mHitItemView->boundingRect().width()-3,mHitItemView->boundingRect().height()-3));
    img.append(QPointF(mHitItemView->boundingRect().width()-3,3));
    img.append(QPointF(3,3));

    QTransform t;
    QTransform::quadToQuad(img, points, t);

    mHitItemView->setTransform(t);
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

void HgContainer::updateByCurrentIndex(const QModelIndex &current)
{
    handleCurrentChanged(current);
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
    QPointF p = mapQtToVg(pos);
    HgQuad* quad = mRenderer->getQuadAt(p);
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

QTransform HgContainer::qtToVgTransform() const
{
    QTransform t;
    if (mOrientation == Qt::Vertical)
    {
        t.translate(0, drawableRect().bottom());
        t.scale(1, -1);
    }
    else // horizontal
    {
        t.translate(drawableRect().bottom(), 0);
        t.scale(-1, 1);
        t.translate(0, drawableRect().right());
        t.rotate(-90, Qt::ZAxis);
    }
    return t;
}

QPointF HgContainer::mapQtToVg(const QPointF& p) const
{
    QTransform t = qtToVgTransform();
    return t.map(p);
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

void HgContainer::handleTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(pos)
    Q_UNUSED(hitItem)
    Q_UNUSED(hitItemIndex)
}

void HgContainer::handleLongTapAction(const QPointF& pos, HgWidgetItem* hitItem, int hitItemIndex)
{
    Q_UNUSED(pos)
    Q_UNUSED(hitItem)
    Q_UNUSED(hitItemIndex)
}

void HgContainer::onScrollPositionChanged(qreal pos)
{
    Q_UNUSED(pos)
}

void HgContainer::handleCurrentChanged(const QModelIndex &current)
{
    Q_UNUSED(current)
    // By default do nothing
}

QRectF HgContainer::drawableRect() const
{
    return rect();
}

void HgContainer::setDefaultImage(QImage defaultImage)
{
    HgQuadRenderer *renderer = mRenderer->getRenderer();
    if (renderer) {
        QImage scaled = defaultImage.scaled(mRenderer->getImageSize().toSize());
        renderer->setDefaultImage(scaled);
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
    return mRenderer->getOrientation();
}

qreal HgContainer::scrollPosition() const
{
    return mSpring.pos().x();
}

