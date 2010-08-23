/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hgindexfeedback.h"
#include "hgindexfeedback_p.h"

#include <HbScrollbar>
#include <HbStyleParameters>
#include <HbStyle>
#include <HbDeviceProfile>
#include <hgwidgets/hgwidgets.h>
#include <HbStyleLoader>

#include <QEvent>
#include <QObject>
#include <QGraphicsScene>

/* rather than magic numbers, let's define some constants. */
namespace {
/*
    string name from the index feedback .css for the single character height parameter.
*/
static const QString ONE_CHAR_HEIGHT = QLatin1String("one-char-height");

/*
    string name from the index feedback .css for the three character height parameter.
*/
static const QString THREE_CHAR_HEIGHT = QLatin1String("three-char-height");

/*
    string name from the index feedback .css for the three character width parameter.
*/
static const QString THREE_CHAR_WIDTH = QLatin1String("three-char-width");

/*
    string name from the index feedback .css for the string mode offest parameter.
*/
static const QString STRING_OFFSET = QLatin1String("string-offset");
}

/*!
    Constructs a new HgIndexFeedback with a parent.
*/
HgIndexFeedback::HgIndexFeedback(QGraphicsItem *parent)
    : HbWidget(parent), p_ptr(new HgIndexFeedbackPrivate)

{
    Q_D( HgIndexFeedback );
    d->q_ptr = this;

    HbStyleLoader::registerFilePath(":/hgindexfeedback.css");

    d->init();
}

/*!
    Destructs the index feedback.
*/
HgIndexFeedback::~HgIndexFeedback()
{
    HbStyleLoader::unregisterFilePath(":/hgindexfeedback.css");
    delete p_ptr;
}

/*!
    \brief sets the index feedback policy.

    \param policy - The HgIndexFeedback::IndexFeedbackPolicy to use.

    \sa HgIndexFeedback::IndexFeedbackPolicy
*/
void HgIndexFeedback::setIndexFeedbackPolicy(HgWidget::IndexFeedbackPolicy policy)
{
    Q_D( HgIndexFeedback );

    if (policy != d->mIndexFeedbackPolicy) {
        d->_q_hideIndexFeedbackNow();
        d->mIndexFeedbackPolicy = policy;
        d->calculatePopupRects();
        d->updatePrimitives();
    }
}

/*!
    \brief Returns the index feedback policy.

    \return The HgIndexFeedback::IndexFeedbackPolicy that's currently in use.
*/
HgWidget::IndexFeedbackPolicy HgIndexFeedback::indexFeedbackPolicy() const
{
    Q_D( const HgIndexFeedback );

    return d->mIndexFeedbackPolicy;
}

/*!
    \brief sets the item view for the index feedback.

    Disconnects from the existing item view, then connects to the specified one.

    If set to NULL the index feedback is simply disconnected.

    \param itemView The HbAbstractItemView* to provide index feedback for.
*/
void HgIndexFeedback::setWidget(HgWidget *widget)
{
    Q_D( HgIndexFeedback );

    if (widget == d->mWidget) {
        return;
    }

    d->disconnectItemView();

    d->mWidget = widget;

    if (!d->mWidget) {
        return;
    }

    d->connectModelToIndexFeedback(d->mWidget->selectionModel());

    d->connectWidgetToIndexFeedback();

    connect(d->mWidget, SIGNAL(destroyed(QObject*)),
        this, SLOT(_q_itemViewDestroyed()));

    if (d->mWidget->scene()) {
        d->mWidget->scene()->addItem(this);
        d->mWidget->installSceneEventFilter(this);
    }

    d->calculatePopupRects();
    updatePrimitives();
}

/*!
    Returns the HbAbstractItemView which the index feedback currently monitors.

    \return HbAbstractItemView*.
*/
HgWidget* HgIndexFeedback::widget() const
{
    Q_D( const HgIndexFeedback );

    return d->mWidget;
}

/*
    A scene event filter.  It's purpose is to call calculatePopupRects on
    a resize event for the item view.
*/
bool HgIndexFeedback::sceneEventFilter(QGraphicsItem *watched, QEvent *ev)
{
    Q_D( HgIndexFeedback );

    if (ev->type() == QEvent::GraphicsSceneResize) {
        d->calculatePopupRects();
        d->updatePrimitives();
    }

    return QGraphicsItem::sceneEventFilter(watched, ev);
}

/*
    Rather than adding signals to HbScrollBar specifically to implement
    index feedback, an event filter is used.

    Specifically, if a scrollbar which is interactive is pressed or released
    this function will call the appropriate function in HgIndexFeedbackPrivate.
*/
bool HgIndexFeedback::eventFilter(QObject *obj, QEvent *ev)
{
    Q_D( HgIndexFeedback );
    HbScrollBar *scrollBar = qobject_cast<HbScrollBar*>(obj);
    HgWidget *widget = qobject_cast<HgWidget*>(obj);
    
    if (d->mIndexFeedbackPolicy != HgWidget::IndexFeedbackNone) {
        if (scrollBar) {
            switch (ev->type()) {
                case QEvent::GraphicsSceneMousePress:
                case QEvent::MouseButtonPress:
                    if (scrollBar->isInteractive()) {
                        d->scrollBarPressed();
                    }
                    break;
    
                case QEvent::GraphicsSceneMouseRelease:
                case QEvent::MouseButtonRelease:
                    if (scrollBar->isInteractive()) {
                        d->scrollBarReleased();
                    }
                    break;
                default:
                    // do nothing, ignore other events.
                    break;
            }
        }
        if (widget && (ev->type() == QEvent::GraphicsSceneResize || ev->type() == QEvent::Resize)) {
            // widget size has changed and size of the popup letter box depends from it
            // so recalculate the popup rects
            d->_q_hideIndexFeedbackNow();
            d->calculatePopupRects();
            d->updatePrimitives();
        }
    }

    return QObject::eventFilter(obj, ev);
}

void HgIndexFeedback::polish(HbStyleParameters& params)
{
    Q_D( HgIndexFeedback );

    params.addParameter( ONE_CHAR_HEIGHT );
    params.addParameter( THREE_CHAR_HEIGHT );
    params.addParameter( THREE_CHAR_WIDTH );
    params.addParameter( STRING_OFFSET );

    HbWidget::polish( params );

    d->mOneCharHeight = params.value( ONE_CHAR_HEIGHT ).toDouble();
    d->mThreeCharHeight = params.value( THREE_CHAR_HEIGHT ).toDouble();
    d->mThreeCharWidth = params.value( THREE_CHAR_WIDTH ).toDouble();
    d->mStringOffset = params.value( STRING_OFFSET ).toDouble();

    d->calculatePopupRects();
    d->updatePrimitives();
}

#include "moc_hgindexfeedback.cpp"
