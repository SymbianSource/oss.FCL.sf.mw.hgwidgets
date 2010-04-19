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

#include <QGraphicsSceneResizeEvent>
#include <hbmainwindow>
#include <hbscrollbar.h>
#include <qapplication.h>
#include <hgwidgets/hgwidgets.h>
#include <hbstyleloader.h>

#include "hgwidgets_p.h"
#include "hgcontainer.h"
#include "hgwidgetitem.h"
#include "hgscrollbuffermanager.h"
#include "hgcoverflowcontainer.h"
#include "hggridcontainer.h"
#include "trace.h"

HgWidget::HgWidget(HbWidgetPrivate* widgetPrivate, QGraphicsItem *parent ):
    HbWidget(*widgetPrivate, parent)
{
    Q_D(HgWidget);
    d->q_ptr = this;

    HbStyleLoader::registerFilePath(":/hgwidget.css");
    HbStyleLoader::registerFilePath(":/hgwidget.widgetml");    
}

HgWidget::~HgWidget()
{
    HbStyleLoader::unregisterFilePath(":/hgwidget.css");
    HbStyleLoader::unregisterFilePath(":/hgwidget.widgetml");
}

/*!
    Returns model that view is currently presenting.
*/
QAbstractItemModel *HgWidget::model() const
{
    Q_D(const HgWidget);
    return d->mModel;
}

/*!
    Sets the model to \a model and replaces current item prototype with \a prototype.
    Ownership of the model is not taken. Ownership of the prototype is taken.
    If no prototype has been passed, default prototype is used.
 */
void HgWidget::setModel(QAbstractItemModel *model )
{
    Q_D(HgWidget);
    d->setModel(model);
}

void HgWidget::setSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_D(HgWidget);
    d->setSelectionModel(selectionModel);
}

QItemSelectionModel *HgWidget::selectionModel() const
{
    Q_D(const HgWidget);
    return d->selectionModel();
}

HgWidget::SelectionMode HgWidget::selectionMode() const
{
    Q_D(const HgWidget);
    return d->selectionMode();
}

/*!
    If newMode is not same as current selection mode of view, updates
    selection mode and all viewitems. If resetSelection is true (the default),
    it clears all existing selections.
*/
void HgWidget::setSelectionMode(SelectionMode mode, bool resetSelection)
{
    Q_D(HgWidget);
    d->setSelectionMode(mode, resetSelection);
}

void HgWidget::selectAll()
{
    Q_D(HgWidget);
    d->selectAll();
}

void HgWidget::clearSelection()
{
    Q_D(HgWidget);
    d->clearSelection();
}

QModelIndex HgWidget::currentIndex() const
{
    Q_D(const HgWidget);
    return d->currentIndex();
}

void HgWidget::setCurrentIndex(
    const QModelIndex &index, QItemSelectionModel::SelectionFlags selectionFlag)
{
    Q_D(HgWidget);
    d->setCurrentIndex(index, selectionFlag);
}

void HgWidget::scrollTo(const QModelIndex &index)
{
    Q_D(HgWidget);
    d->scrollTo(index);
}

void HgWidget::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_D( HgWidget);
    // TODO,take columns into count
    for( int i = topLeft.row(); i <= bottomRight.row(); i++ ){
        // if data for item outside our current buffer has changed
        // we just have to ignore it since we dont have resources
        // to handle it(or we dont want to waste resources).
        if(d->mBufferManager->positionInsideBuffer(i)){
            HgWidgetItem* item = d->mContainer->itemByIndex( i );
            if( item ){
                item->updateItemData();
            }
        }
    }
    d->mContainer->itemDataChanged( topLeft, bottomRight );
}

/*!
 * Returns true if the scroll area handles
 * long press gestures, false otherwise
 *
 * \sa HbScrollArea::setHandleLongPress()
 */
bool HgWidget::longPressEnabled() const
{
    Q_D( const HgWidget );

    return d->mHandleLongPress;
}

/*!
 * Sets the value of the handleLongPress property.  This value is set
 * to true if the widget is to respond to long press gestures, false otherwise.
 *
 * The default value is false.
 *
 * \sa HbScrollArea::handleLongPress()
 */
void HgWidget::setLongPressEnabled (bool value)
{
    Q_D( HgWidget );

    if (d->mHandleLongPress != value)
    {
        d->mHandleLongPress = value;
        if (value)
        {
            grabGesture(Qt::TapAndHoldGesture);
        }
        else
        {
            ungrabGesture(Qt::TapAndHoldGesture);
        }
    }

    // TODO, should we do something like this?????
//    if (isChanged) {
//        d->updateGestures();
//        emit gestureSceneFilterChanged( d->mGestureFilter );
//    }
}

HgWidget::ScrollBarPolicy HgWidget::scrollBarPolicy() const
{
    Q_D(const HgWidget);
    return d->mScrollBarPolicy;
}

/*!
    Sets the policy for vertical scrollbar

    The default policy is HgWidget::ScrollBarAutoHide.

    \sa setHorizontalScrollBarPolicy(), verticalScrollBarPolicy()
*/
void HgWidget::setScrollBarPolicy(ScrollBarPolicy policy)
{
    Q_D(HgWidget);
    d->setScrollBarPolicy(policy);
}

/*!
  Returns the vertical scroll bar.

  \sa verticalScrollBarPolicy(), horizontalScrollBar()
 */
HbScrollBar *HgWidget::scrollBar() const
{
    Q_D(const HgWidget);
    return d->mScrollBar;
}

/*!
   Replaces the existing vertical scroll bar with \a scrollBar. The former
   scroll bar is deleted.

   HgWidget already provides vertical and horizontal scroll bars by
   default. You can call this function to replace the default vertical
   scroll bar with your own custom scroll bar.

   \sa verticalScrollBar(), setHorizontalScrollBar()
*/
void HgWidget::setScrollBar(HbScrollBar *scrollBar)
{
    Q_D(HgWidget);
    if (!scrollBar) {
        qWarning("HgWidget::setVerticalScrollBar: Cannot set a null scroll bar");
        return;
    }

    d->replaceScrollBar(scrollBar);
}

bool HgWidget::eventFilter(QObject *obj,QEvent *event)
{
    Q_D(HgWidget);
    switch (event->type() )
        {
        case QEvent::ApplicationActivate:
            {
            d->gainedForeground();
            break;
            }
        case QEvent::ApplicationDeactivate:
            {
            d->lostForeground();
            break;
            }
        case QEvent::GraphicsSceneResize:
            {
            d->adjustGeometry();
            break;
            }
        default:
            break;
        }
    return QObject::eventFilter(obj, event);
}

bool HgWidget::event(QEvent *event)
{
    Q_D(HgWidget);

    bool value(false);
    if (event){
        value = HbWidget::event(event);
        if (event->type() == QEvent::GraphicsSceneResize){
            d->adjustGeometry();
        }
    }
    return value;
}

bool HgWidget::getItemOutline(const QModelIndex& index, QPolygonF& points)
{
    Q_D(HgWidget);

    return d->getItemOutline(index, points);
}

void HgWidget::aboutToChangeOrientation()
{

}

void HgWidget::orientationChanged(Qt::Orientation orientation)
{
    Q_D(HgWidget);
    d->orientationChanged(orientation);
}

Qt::Orientation HgWidget::scrollDirection() const
{
    Q_D(const HgWidget);
    return d->scrollDirection();
}

QList<QModelIndex> HgWidget::getVisibleItemIndices() const
{
    Q_D(const HgWidget);
    return d->getVisibleItemIndices();
}

void HgWidget::setIndexFeedbackPolicy(IndexFeedbackPolicy policy)
{
    Q_D(HgWidget);
    d->setIndexFeedbackPolicy(policy);
}

HgWidget::IndexFeedbackPolicy HgWidget::indexFeedbackPolicy() const
{
    Q_D(const HgWidget);
    return d->indexFeedbackPolicy();
}

// EOF