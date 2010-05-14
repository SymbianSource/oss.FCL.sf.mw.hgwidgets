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

#include <QGraphicsLinearLayout>
#include <QApplication>
#include <hbscrollbar_p.h>
#include <hgwidgets/hgwidgets.h>

#include "hgwidgets_p.h"
#include "hgcontainer.h"
#include "hgcoverflowcontainer.h"
#include "hgscrollbuffermanager.h"
#include "hgwidgetitem.h"
#include "trace.h"
//#include "hgindexfeedback.h"

static const int INITIAL_SCROLLBAR_HIDE_TIMEOUT(4000);
static const int DEFAULT_BUFFER_SIZE(30);

HgWidgetPrivate::HgWidgetPrivate() :
    mLayout(0),
    mContainer(0),
    mBufferManager(0),
    mModel(0),
    mSelectionModel(0),
    mDefaultSelectionModel(0),
    mScrollBar(0),
    mAbleToScroll(false),
    mHandleLongPress(false),
    mBufferSize(DEFAULT_BUFFER_SIZE),
    mStaticScrollDirection(false)
{
    FUNC_LOG;
}

HgWidgetPrivate::~HgWidgetPrivate()
{
    FUNC_LOG;

    delete mScrollBarHideTimer;
    delete mDefaultSelectionModel;
    delete mBufferManager;
}

void HgWidgetPrivate::init(HgContainer *container)
{
    FUNC_LOG;
    Q_Q(HgWidget);

    mScrollBarHideTimer = new QTimer();
    mScrollBarHideTimer->setSingleShot(true);

    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    q->setFlag( QGraphicsItem::ItemClipsChildrenToShape, true );
    q->setFocusPolicy(Qt::StrongFocus);

    createScrollBar(container->scrollDirection());
    
    //QObject::connect(&(mScrollBarHideTimer), SIGNAL(timeout()), q, SLOT(_q_hideScrollBars()));

    mContainer = container;

    mScrollBarPolicy = HgWidget::ScrollBarAutoHide;

    q->connect(mContainer, SIGNAL(scrollPositionChanged(qreal,bool)),
               q, SLOT(_q_scrollPositionChanged(qreal,bool)));
    q->connect(mContainer, SIGNAL(activated(const QModelIndex&)),
               q, SIGNAL(activated(const QModelIndex&)));
    q->connect(mContainer, SIGNAL(longPressed(const QModelIndex&, const QPointF &)),
               q, SIGNAL(longPressed(const QModelIndex&, const QPointF &)));
    q->connect(mContainer, SIGNAL(scrollingStarted()), q, SIGNAL(scrollingStarted()));
    q->connect(mContainer, SIGNAL(scrollingEnded()), q, SIGNAL(scrollingEnded()));
    q->connect(mScrollBarHideTimer, SIGNAL(timeout()), q, SLOT(_q_hideScrollBars()));
    
//    mIndexFeedback = new HgIndexFeedback(q);
//    mIndexFeedback->setWidget(q);
    
}

void HgWidgetPrivate::setModel( QAbstractItemModel *model )
{
    FUNC_LOG;

    if (model != mModel) {
        clearCurrentModel();
        mModel = model;
        initializeNewModel();
    }
}

void HgWidgetPrivate::setSelectionModel(QItemSelectionModel *selectionModel)
{
    FUNC_LOG;
    
    Q_Q(HgWidget);

    if (mContainer) {
        if (selectionModel == 0) {
            QItemSelectionModel *oldSelectionModel = mDefaultSelectionModel;
            mDefaultSelectionModel = 0;
            mDefaultSelectionModel = new QItemSelectionModel(mModel);
            mContainer->setSelectionModel(mDefaultSelectionModel);
            delete oldSelectionModel;
        }
        else if (selectionModel != mContainer->selectionModel()) {
            QItemSelectionModel *oldSelectionModel = mDefaultSelectionModel;
            mDefaultSelectionModel = 0;
            mContainer->setSelectionModel(selectionModel);
            delete oldSelectionModel;
        }
        if (mContainer->selectionModel()) {
//            if (mIndexFeedback) {
//                delete mIndexFeedback;
//                mIndexFeedback = 0;
//            }
//            mIndexFeedback = new HgIndexFeedback(q);
//            mIndexFeedback->setWidget(q);
        }            
    }
}

QItemSelectionModel *HgWidgetPrivate::selectionModel() const
{
    FUNC_LOG;

    if (mContainer) {
        return mContainer->selectionModel();
    }
    return 0;
}

HgWidget::SelectionMode HgWidgetPrivate::selectionMode() const
{
    FUNC_LOG;

    if (mContainer) {
        return mContainer->selectionMode();
    }
    return HgWidget::NoSelection;
}

void HgWidgetPrivate::setSelectionMode(HgWidget::SelectionMode mode, bool resetSelection)
{
    FUNC_LOG;

    if (mContainer) {
        mContainer->setSelectionMode(mode, resetSelection);
    }
}

void HgWidgetPrivate::selectAll()
{
    FUNC_LOG;
    Q_Q(HgWidget);

    if (mContainer &&
        (mContainer->selectionMode() == HgWidget::MultiSelection ||
         mContainer->selectionMode() == HgWidget::ContiguousSelection) &&
        mModel && mModel->columnCount() > 0 && mModel->rowCount() > 0) {
        QItemSelection selection(
                mModel->index(0, 0),
                mModel->index(mModel->rowCount()-1, mModel->columnCount()-1));
        mContainer->selectionModel()->select(selection, QItemSelectionModel::Select);
        q->update();
    }
}

void HgWidgetPrivate::clearSelection()
{
    FUNC_LOG;
    Q_Q(HgWidget);

    if (mContainer) {
        mContainer->selectionModel()->clearSelection();
        q->update();
    }
}

QModelIndex HgWidgetPrivate::currentIndex() const
{
    FUNC_LOG;

    if (mContainer && mContainer->selectionModel()) {
        return mContainer->selectionModel()->currentIndex();
    }
    return QModelIndex();
}

void HgWidgetPrivate::setCurrentIndex(
    const QModelIndex &index, QItemSelectionModel::SelectionFlags selectionFlag)
{
    FUNC_LOG;

    if (mContainer && mContainer->selectionModel()) {
        mContainer->selectionModel()->setCurrentIndex(index, selectionFlag);
    }
}

void HgWidgetPrivate::scrollTo(const QModelIndex &index)
{
    FUNC_LOG;

    if (index.isValid()) {
        if (mContainer) {
            mContainer->scrollTo(index);
            updateScrollMetrics(mContainer->scrollPosition());
        }
        if (mBufferManager) {
            mBufferManager->scrollPositionChanged(index.row());
        }
    }
}

void HgWidgetPrivate::initializeNewModel()
{
    FUNC_LOG;
    Q_Q(HgWidget);

    if (mModel) {
        // These asserts do basic sanity checking of the model
        Q_ASSERT_X(mModel->index(0,0) == mModel->index(0,0),
                   "HbAbstractItemView::setModel",
                   "A model should return the exact same index "
                   "(including its internal id/pointer) when asked for it twice in a row.");
        Q_ASSERT_X(mModel->index(0,0).parent() == QModelIndex(),
                   "HbAbstractItemView::setModel",
                   "The parent of a top level index should be invalid");

        q->connect(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                           SLOT( dataChanged(QModelIndex,QModelIndex)));
        q->connect(mModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
                           SLOT(_q_insertRows(QModelIndex, int, int)));
        q->connect(mModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                           SLOT(_q_removeRows(QModelIndex, int, int)));
        q->connect(mModel, SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)),
                           SLOT(_q_moveRows(QModelIndex, int, int, QModelIndex, int)));
        q->connect(mModel, SIGNAL(modelReset()),SLOT(_q_modelReset()));
        
        mContainer->setItemCount(mModel->rowCount(QModelIndex()));
        QList<HgWidgetItem*> items = mContainer->items();

        // set model indexes for the items firsts
        const int itemCount = items.count();
        for( int i=0; i<itemCount; i++)
        {
            items.at(i)->setModelIndex(mModel->index(i, 0, QModelIndex()));
        }

        initBufferManager(itemCount);
        
        setSelectionModel(0); // Default

        if (mModel->rowCount() > 0)
        {
            setCurrentIndex(mModel->index(0, 0));
            scrollTo(mModel->index(0, 0));
        }
    }
}

void HgWidgetPrivate::clearCurrentModel()
{
    FUNC_LOG;
    Q_Q(HgWidget);

    if (mModel) {
        mModel->disconnect(q, SLOT(dataChanged(QModelIndex, QModelIndex)));
        mModel->disconnect(q, SLOT(_q_insertRows(QModelIndex, int, int)));
        mModel->disconnect(q, SLOT(_q_removeRows(QModelIndex, int, int)));
        mModel->disconnect(q, SLOT(_q_moveRows(QModelIndex, int, int, QModelIndex, int)));
        mModel->disconnect(q, SLOT(_q_modelReset()));
        mModel = 0;
    }
    else if (mContainer) {
        mContainer->setItemCount(0);
    }
//  TODO: setSelectionModel(0);

}
void HgWidgetPrivate::_q_releaseItems( int releaseStart, int releaseEnd )
{
    FUNC_LOG;

    QList<HgWidgetItem*> items = mContainer->items();
    const int itemCount = items.count();

    int start = qBound(0, releaseStart, itemCount-1);
    int end = qBound(0, releaseEnd, itemCount-1);

    INFO("Release items:" << start << "-" << end);
    // request data for items
    for (int i = start; i <= end; i++) {
        HgWidgetItem* item = items.at(i);
        if (item) {
            item->releaseItemData();
        }
    }
}

void HgWidgetPrivate::_q_requestItems(int requestStart, int requestEnd)
{
    FUNC_LOG;

    QList<HgWidgetItem*> items = mContainer->items();
    const int itemCount = items.count();

    int start = qBound(0, requestStart, itemCount-1);
    int end = qBound(0, requestEnd, itemCount-1);

    // variables to track which items are really updated.
    int firstUpdated = -1;
    int lastUpdated = -1;

    INFO("Request items:" << start << "-" << end);
    // request data for items
    for (int i = start; i <= end; i++) {
        HgWidgetItem* item = items.at(i);
        if (item && item->updateItemData()) {
            if (firstUpdated == -1) firstUpdated = i;
            lastUpdated = i;
        }
    }

    // notify container which items have updated data available.
    // container is responsible to redraw view if some of the items
    // is visible.
    if (firstUpdated != -1 && lastUpdated != -1) {
        mContainer->itemDataChanged(firstUpdated, lastUpdated);
    }
}

void HgWidgetPrivate::_q_scrollPositionChanged(qreal index,bool scrollBarAnimation)
{
    int newPos = index;
    newPos *= mContainer->rowCount();
    if (mBufferManager) {
        mBufferManager->scrollPositionChanged(newPos);
    }

    if (!scrollBarAnimation)
        updateScrollMetrics(index);
}

void HgWidgetPrivate::replaceScrollBar(HbScrollBar *scrollBar)
{
    Q_Q(HgWidget);
    delete mScrollBar;
    mScrollBar = scrollBar;

    scrollBar->setParentItem(q);

    // make sure the scrollbar is on top
    scrollBar->setZValue(q->zValue() + 1);

    prepareScrollBars();

    QObject::connect(scrollBar, SIGNAL(valueChanged(qreal, Qt::Orientation)), q, SLOT(_q_thumbPositionChanged(qreal, Qt::Orientation)));
}

void HgWidgetPrivate::prepareScrollBars( qreal pos )
{
    Q_Q( HgWidget );

    QRectF boundingRect = q->boundingRect();

    bool scrollBarsVisible(false);

    if ((mAbleToScroll && mScrollBarPolicy != HgWidget::ScrollBarAlwaysOff) ||
        mScrollBarPolicy == HgWidget::ScrollBarAlwaysOn) {

        displayScrollBar(pos );
        scrollBarsVisible = true;
    }
    else if((!mAbleToScroll || mScrollBarPolicy == HgWidget::ScrollBarAlwaysOff)
               && mScrollBar->isVisible()){
            mScrollBar->setVisible(false);
    }

    if (scrollBarsVisible) {
        mScrollBarHideTimer->stop();
        mScrollBarHideTimer->start(INITIAL_SCROLLBAR_HIDE_TIMEOUT);
    }
}

void HgWidgetPrivate::displayScrollBar(qreal pos)
{
    Q_Q(HgWidget);
    // Layout the scrollbar
    setScrollBarMetrics(pos);

    // Activate the scrollbar
    if ( !mScrollBar->isVisible() && q->isVisible()) {
        mScrollBar->setVisible(true);
    }
}

void HgWidgetPrivate::setScrollBarPolicy(HgWidget::ScrollBarPolicy policy)
{
    mScrollBarPolicy = policy;

    if (mScrollBarPolicy == HgWidget::ScrollBarAlwaysOff &&
        mScrollBar->isVisible()){
        mScrollBar->setVisible(false);
    }

    if(policy != HgWidget::ScrollBarAlwaysOff){
        updateScrollMetrics();
    }
}

void HgWidgetPrivate::_q_hideScrollBars()
{
    if (mScrollBar && mScrollBarPolicy == HgWidget::ScrollBarAutoHide) {

        // TODO, do we need to know if scrollbar was pressed? we cannot access the private methods, since
        // only scrollareaprivate is a friend class.
        if (false/*scrollBarPressed(mHorizontalScrollBar) ||
                scrollBarPressed(mVerticalScrollBar)*/) {
            mScrollBarHideTimer->start();
        } else if(mScrollBarPolicy != HgWidget::ScrollBarAlwaysOn
                  && mScrollBar->isVisible()){
            mScrollBar->setVisible(false);

        }
    }
}

/**
 *
 */
void HgWidgetPrivate::_q_thumbPositionChanged(qreal value, Qt::Orientation orientation)
{
    Q_UNUSED(orientation)

    mContainer->scrollToPosition( value, true );

    // TODO, stop all scrolling and animations

    if (mScrollBarHideTimer->isActive()) {
        mScrollBarHideTimer->stop();
        mScrollBarHideTimer->start();
    }
}

void HgWidgetPrivate::_q_insertRows(const QModelIndex &parent, int start, int end)
{
    FUNC_LOG;
    INFO("Insert rows" << start << "-" << end);
    Q_UNUSED(parent)
    Q_Q(HgWidget);

    if (mContainer) {
        const int oldItemCount = mContainer->itemCount();
        if (oldItemCount == 0) {
            // rows have been inserted to empty model. This is a special case
            // that reset function should handle.
            _q_modelReset();
            return;
        }
        
        mBufferManager->addItems(start, end);
        mContainer->addItems(start, end);
        // re-set model indexes for the items including and after the added indexes
        QList<HgWidgetItem *> items = mContainer->items();
        int newItemCount = items.count();
        for (int i = start; i < newItemCount; i++) {
            items.at(i)->setModelIndex(mModel->index(i, 0, QModelIndex()));
        }
        mBufferManager->flushRequestBuffers();
        if (oldItemCount == 0 && newItemCount > 0) {
            setCurrentIndex(mModel->index(0, 0));
        }
        q->update();
    }
}

void HgWidgetPrivate::_q_removeRows(const QModelIndex &parent, int start, int end)
{
    FUNC_LOG;
    INFO("Remove rows" << start << "-" << end);
    Q_UNUSED(parent)
    Q_Q(HgWidget);

    if (mContainer && mBufferManager) {
        int itemCount = mContainer->itemCount();
        int first= qBound(0, start, itemCount-1);
        int last = qBound(0, end, itemCount-1);

        mBufferManager->removeItems(first, last);
        mContainer->removeItems(first, last);
        // re-set model indexes for the items after the removed indexes
        QList<HgWidgetItem *> items = mContainer->items();
        int newItemCount = items.count();
        for (int i = first; i < newItemCount; i++) {
            items.at(i)->setModelIndex(mModel->index(i, 0, QModelIndex()));
        }
        mBufferManager->flushRequestBuffers();
        q->update();
    }
}

void HgWidgetPrivate::_q_moveRows(const QModelIndex &sourceParent,
    int sourceStart, int sourceEnd,
    const QModelIndex &destinationParent, int destinationRow)
{
    FUNC_LOG;
    INFO("Move rows" << sourceStart << "-" << sourceEnd << "to" << destinationRow);
    Q_UNUSED(sourceParent)
    Q_UNUSED(destinationParent)
    Q_Q(HgWidget);

    if (mContainer) {
        mContainer->moveItems(sourceStart, sourceEnd, destinationRow);
        mBufferManager->moveItems(sourceStart, sourceEnd, destinationRow);
        // re-set model indexes for the items after the removed indexes
        QList<HgWidgetItem *> items = mContainer->items();
        int itemCount = items.count();
        for (int i = qMin(sourceStart, destinationRow); i < itemCount; i++) {
            items.at(i)->setModelIndex(mModel->index(i, 0, QModelIndex()));
        }
        mBufferManager->flushRequestBuffers();
        q->update();
    }
}

void HgWidgetPrivate::_q_modelReset()
{
    if (mContainer && mBufferManager) {
        const int oldItemCount = mContainer->itemCount();
        const int newItemCount = mModel->rowCount();
        if (newItemCount == oldItemCount) {
            // Model is reseted but itemcount is still the same.
            // Just reload all data for current buffer.
            mBufferManager->resetBuffer(mContainer->scrollPosition(), newItemCount);
        }
        else {
            // Container destroyes all old items when new itemcount is set.
            mContainer->setItemCount(newItemCount);

            // set model indexes for the items
            QList<HgWidgetItem*> items = mContainer->items();
            const int itemCount = items.count();
            for( int i=0; i<itemCount; i++) {
                items.at(i)->setModelIndex(mModel->index(i, 0, QModelIndex()));
            }
                        
            // Buffermanager requests items to be updated. 
            mBufferManager->resetBuffer(0, newItemCount);
            if (mModel->rowCount() > 0) {
                setCurrentIndex(mModel->index(0, 0));
                scrollTo(mModel->index(0, 0));
            }
        }        
    }
}

void HgWidgetPrivate::setScrollBarMetrics(qreal pos)
{
    Q_Q( HgWidget );

    if (!mContainer)
        return;

    qreal screenSize, worldSize;
    mContainer->dimensions(screenSize,worldSize);

    const qreal start(0.0);
    const qreal end(1.0);
    const qreal page(screenSize/worldSize);

    // Set handle size
    mScrollBar->setPageSize( qBound(start,page,end ) );

    updateScrollBar(pos);
}

void HgWidgetPrivate::updateScrollBar(qreal pos)
{
    Q_Q( HgWidget );

    if (pos < 0.0)
        pos = 0.0;
    else if (pos > 1.0)
        pos = 1.0;
        // The scrollbar "thumb" position is the current position of the contents widget divided
        // by the difference between the height of the contents widget and the height of the scroll area.
        // This formula assumes that the "thumb" of the the scroll bar is sized proportionately to
        // the height of the contents widget.
    mScrollBar->setValue(pos);
}

/*
 * updateScrollMetrics() is a private function called when
 * scrolling starts to set the metrics needed in scrolling.
 */
void HgWidgetPrivate::updateScrollMetrics( qreal pos)
{
    Q_Q(HgWidget);

    QRectF scrollAreaBoundingRect = q->boundingRect();


    qreal screenSize, worldSize;
    mContainer->dimensions(screenSize,worldSize);

    mAbleToScroll = false;
    if (worldSize > screenSize) {
        mAbleToScroll = true;
    }

    prepareScrollBars( pos/worldSize );
}

void HgWidgetPrivate::adjustGeometry()
{
    Q_Q(HgWidget);
    
    QRectF scrollAreaBoundingRect = q->boundingRect();
    if( scrollAreaBoundingRect.isNull() ||
            !scrollAreaBoundingRect.isValid() ||
            !mContainer ||
            scrollAreaBoundingRect == mContainer->boundingRect() )
        return;

    mContainer->resize(scrollAreaBoundingRect.size());

    updateScrollMetrics(mContainer->scrollPosition());
}


void HgWidgetPrivate::lostForeground()
{
    if( !mForeground ) return;
    
    mForeground = false;
    QList<HgWidgetItem*> list = mContainer->items();
    foreach(HgWidgetItem* item, list){
        item->releaseItemData();
    }
}

void HgWidgetPrivate::gainedForeground()
{
    if( mForeground ) return;
    
    mForeground = true;
    QList<HgWidgetItem*> list = mContainer->items();
    int bufferStart = 0;
    int bufferEnd = 0;
    mBufferManager->currentBuffer(bufferStart,bufferEnd);
    for(;bufferStart<=bufferEnd;bufferStart++){
        list.at(bufferStart)->updateItemData();
    }
}

bool HgWidgetPrivate::getItemOutline(const QModelIndex& index, QPolygonF& points)
{
    return mContainer->getItemPoints(index.row(), points);
}

void HgWidgetPrivate::aboutToChangeOrientation()
{

}

void HgWidgetPrivate::orientationChanged(Qt::Orientation orientation)
{
    Q_Q(HgWidget);
    if (mContainer->orientation() != orientation) {
        mContainer->setOrientation(orientation, q->isVisible());
        if (!mStaticScrollDirection) {
            createScrollBar(orientation);
        }
        q->repolish();
        adjustGeometry();
    }
}

void HgWidgetPrivate::_q_groovePressed(qreal value, Qt::Orientation orientation)
{
    Q_UNUSED(value);
    Q_UNUSED(orientation);
}

Qt::Orientation HgWidgetPrivate::scrollDirection() const
{
    return mContainer->orientation();
}

void HgWidgetPrivate::createScrollBar(Qt::Orientation orientation)
{
    Q_Q(HgWidget);
    
    delete mScrollBar;
    mScrollBar = 0;
    mScrollBar = new HbScrollBar(orientation,q);
    if (orientation == Qt::Vertical) {
        HbStyle::setItemName(mScrollBar, "scrollbar-vertical");
    }
    else {
        HbStyle::setItemName(mScrollBar, "scrollbar-horizontal");    
    }

    mScrollBar->setZValue(q->zValue() + 1);
    QObject::connect(mScrollBar, SIGNAL(valueChanged(qreal, Qt::Orientation)),
                     q, SLOT(_q_thumbPositionChanged(qreal, Qt::Orientation)));
    QObject::connect(mScrollBar, SIGNAL(valueChangeRequested(qreal, Qt::Orientation)),
                     q, SLOT(_q_groovePressed(qreal, Qt::Orientation)));
    mScrollBar->setVisible(false);
}

QList<QModelIndex> HgWidgetPrivate::getVisibleItemIndices() const
{
    return mContainer->getVisibleItemIndices();
}

void HgWidgetPrivate::setIndexFeedbackPolicy( HgWidget::IndexFeedbackPolicy policy)
{
//    mIndexFeedback->setIndexFeedbackPolicy(policy);
}

HgWidget::IndexFeedbackPolicy HgWidgetPrivate::indexFeedbackPolicy() const
{
//    return mIndexFeedback->indexFeedbackPolicy();
    return HgWidget::IndexFeedbackNone;
}

void HgWidgetPrivate::setDefaultImage(QImage defaultImage)
{
    mContainer->setDefaultImage(defaultImage);
}

void HgWidgetPrivate::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    FUNC_LOG;

    // TODO,take columns into count
    for( int i = topLeft.row(); i <= bottomRight.row(); i++ ){
        // if data for item outside our current buffer has changed
        // we just have to ignore it since we dont have resources
        // to handle it(or we dont want to waste resources).
        if (mBufferManager->positionInsideBuffer(i)) {
            HgWidgetItem* item = mContainer->itemByIndex( i );
            if (item) {
                item->updateItemData();
            }
        }
    }
    mContainer->itemDataChanged(topLeft, bottomRight);
}

void HgWidgetPrivate::setItemSizePolicy(HgWidget::ItemSizePolicy policy)
{
    mContainer->setItemSizePolicy(policy);
}

HgWidget::ItemSizePolicy HgWidgetPrivate::itemSizePolicy() const
{
    return mContainer->itemSizePolicy();
}

void HgWidgetPrivate::setItemSize(const QSizeF& size)
{
    mContainer->setItemSize(size);
}

QSizeF HgWidgetPrivate::itemSize() const
{
    return mContainer->itemSize();
}

void HgWidgetPrivate::setItemSpacing(const QSizeF& spacing)
{
    mContainer->setItemSpacing(spacing);
}

QSizeF HgWidgetPrivate::itemSpacing() const
{
    return mContainer->itemSpacing();
}

void HgWidgetPrivate::initBufferManager(int itemCount)
{
    Q_Q(HgWidget);
    if (mBufferManager) {
        q->disconnect( mBufferManager, SIGNAL(releaseItems(int,int)), q, SLOT(_q_releaseItems(int,int)));
        q->disconnect( mBufferManager, SIGNAL(requestItems(int,int)), q, SLOT(_q_requestItems(int,int)));
        delete mBufferManager;
        mBufferManager = 0;
    }

    mBufferManager = new HgScrollBufferManager(mBufferSize,qMax(mContainer->rowCount()*2,3),0,itemCount);
    q->connect( mBufferManager, SIGNAL(releaseItems(int,int)), q, SLOT(_q_releaseItems(int,int)));
    q->connect( mBufferManager, SIGNAL(requestItems(int,int)), q, SLOT(_q_requestItems(int,int)));
    mBufferManager->resetBuffer(0, itemCount);
}

#include "moc_hgwidgets.cpp"
