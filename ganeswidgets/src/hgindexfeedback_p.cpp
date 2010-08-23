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
#include <HbStyle>
#include <HbApplication>
#include <HbEffect>
#include <HbFrameItem>
#include <HbTextItem>
#include <hgwidgets/hgwidgets.h>

#include <QTimer>
#include <QSize>
#include <QGraphicsScene>
#include <qitemselectionmodel>

// rather than having magic numbers
// defaults and constants are defined here.
namespace {
/* 
    Press timeout is the duration after the scrollbar is 
    pressed to when the index feedback will be dismissed.

    In the case that the press timeout has not yet passed
    and the timer should be fired with the release timeout,
    the timer with the press timeout will continue running.
*/
static const int DEFAULT_INDEX_FEEDBACK_PRESS_TIMEOUT = 1000;

/*
    Dwell timeout is the duration after which if there is not a change
    in the scrollbar's value the index feedback will be dismissed.
*/
static const int DEFAULT_INDEX_FEEDBACK_DWELL_TIMEOUT = 2000;

/*
    Release timeout is the duration after which the user has lifted
    their finger from the scrollbar to when the index feedback will be dismissed.
*/
static const int DEFAULT_INDEX_FEEDBACK_RELEASE_TIMEOUT = 250;

/*
    The default value for the index feedback policy.
*/
static const HgWidget::IndexFeedbackPolicy DEFAULT_INDEX_FEEDBACK_POLICY = HgWidget::IndexFeedbackNone;

/*
    The value of the index feedback's disappear animation
*/
static const int INDEX_FEEDBACK_DISAPPEAR_DURATION = 300;
}

/*
    constructor
*/
HgIndexFeedbackPrivate::HgIndexFeedbackPrivate() :
    mIndexFeedbackPressTimeout(DEFAULT_INDEX_FEEDBACK_PRESS_TIMEOUT),
    mIndexFeedbackDwellTimeout(DEFAULT_INDEX_FEEDBACK_DWELL_TIMEOUT),
    mIndexFeedbackReleaseTimeout(DEFAULT_INDEX_FEEDBACK_RELEASE_TIMEOUT),
    mIndexFeedbackPolicy(DEFAULT_INDEX_FEEDBACK_POLICY),
    mScrollBarValue(-1.0),
    mIndexFeedbackTimer(0),
    mDisappearTimer(0),
    mTextItem(0),
    mPopupItem(0),
    mOneCharHeight(1.0),
    mThreeCharHeight(1.0),
    mThreeCharWidth(1.0),
    mStringOffset(0.0),
    mWidget(0),
    mScrollBarPressed(false)
{

}

/*
    Destructor
*/
HgIndexFeedbackPrivate::~HgIndexFeedbackPrivate()
{

}

/*
    2ndary construction, called from public's constructor.
*/
void HgIndexFeedbackPrivate::init()
{
    Q_Q( HgIndexFeedback );

    //mItemView = 0; // double check that this is safe.

    HbEffect::add(HB_INDEXFEEDBACK_TYPE, "indexfeedback_appear", EFFECT_IFAPPEAR);
    if (!HbEffect::add(HB_INDEXFEEDBACK_TYPE, "indexfeedback_disappear", EFFECT_IFDISAPPEAR)) {
        mDisappearTimer = new QTimer(q);
        mDisappearTimer->setSingleShot(true);
        mDisappearTimer->setInterval(INDEX_FEEDBACK_DISAPPEAR_DURATION);
        q->connect(mDisappearTimer, SIGNAL(timeout()),
            q, SLOT(_q_hideIndexFeedbackNow()));
    }

    mIndexFeedbackTimer = new QTimer(q);
    mIndexFeedbackTimer->setSingleShot(true);
    q->connect(mIndexFeedbackTimer, SIGNAL(timeout()),
        q, SLOT(_q_hideIndexFeedback()));

    createPrimitives();
}

/*
    Update the data for, and show the index feedback (if it's not already shown)
*/
void HgIndexFeedbackPrivate::showIndexFeedback()
{
    if (!mWidget 
        || mIndexFeedbackPolicy == HgWidget::IndexFeedbackNone) {
        return;
    }

    QModelIndex targetIndex = mWidget->currentIndex();   

    if (targetIndex.isValid()) {
        QVariant data = targetIndex.data(Hb::IndexFeedbackRole);
        if (data.canConvert<QString>()) {

            QString testString = displayText(data);
            if (testString != mPopupContent) {
                mPopupContent = testString;
                updatePrimitives();
            }

            if (mTextItem->opacity() == 0.0) {
                HbEffect::start(mPopupItemList, HB_INDEXFEEDBACK_TYPE, EFFECT_IFAPPEAR);
            }
            if (mTextItem) {
                mTextItem->show();
            }

            if (mPopupItem) {
                mPopupItem->show();
            }

            if (mDisappearTimer) {
                mDisappearTimer->stop();
            }
        } else {
            _q_hideIndexFeedback();
        }
    }
}

void HgIndexFeedbackPrivate::updateIndex()
{
    QModelIndex targetIndex = mWidget->currentIndex();   

    if (targetIndex.isValid()) {
        QVariant data = targetIndex.data(Hb::IndexFeedbackRole);
        if (data.canConvert<QString>()) {
            QString testString = displayText(data);
            if (testString != mPopupContent) {
                mPopupContent = testString;
                updatePrimitives();
            }            
        }
    }
}


/*
    Returns the qstring which will be the text in the index feedback.
*/
QString HgIndexFeedbackPrivate::displayText(const QVariant &data) const
{
    QString retVal = QString();

    switch (mIndexFeedbackPolicy) {
        case HgWidget::IndexFeedbackSingleCharacter:
            retVal = data.toString().left(1);
            break;

        case HgWidget::IndexFeedbackThreeCharacter:
            retVal = data.toString().left(3);
            break;

        case HgWidget::IndexFeedbackString:
            retVal = data.toString();
            break;

        default:
            qt_noop();
            break;
    }

    return retVal;
}

/*
    Handle the case of the scrollbar being pressed.

    This is show the index feedback and start the dismissal timer with the
    press timeout.
*/
void HgIndexFeedbackPrivate::scrollBarPressed()
{
    showIndexFeedback();

    // need to record the scrollbar values    

    // TODO::The values are storred here is a work around for a bug in hbscrollbar.
    // the bug is that the value changed signal is emitted when the thumb
    // is pressed, and again when it is released, regaurdless of if there was a value change.
    // once that bug is fixed, this should be removed.
    mScrollBarValue = mWidget->scrollBar()->value();
    mScrollBarPressed = true;
}

/*
    Handle the case of the scrollbar being released.

    This is to start the dismissal timer with the release timeout.
    ...but not if the timer is still running with the press timeout...
*/
void HgIndexFeedbackPrivate::scrollBarReleased()
{
    // record the scrollbar values in case position changed is emitted after us w/o a real change
    
    // TODO::The values are storred here is a work around for a bug in hbscrollbar.
    // the bug is that the value changed signal is emitted when the thumb
    // is pressed, and again when it is released, regaurdless of if there was a value change.
    // once that bug is fixed, this should be removed.
    mScrollBarValue = mWidget->scrollBar()->value();

    // start this timer.
    if (!(mIndexFeedbackTimer->isActive()
         && mIndexFeedbackTimer->interval() == mIndexFeedbackPressTimeout)) {
        mIndexFeedbackTimer->setInterval(mIndexFeedbackReleaseTimeout);
        mIndexFeedbackTimer->start();
    }

    mScrollBarPressed = false;

}

/*
    Handle the case of the scrollbar being moved.

    This is to stop any existing timers (only if the scrollbar actually moved),
    and start a timer with the dwell timeout.

    NOTE:: this should be much simpler once the valueChanged signal from hbscrollbar
    is emitted at the correct times.
*/
void HgIndexFeedbackPrivate::_q_scrollPositionChanged(qreal value, Qt::Orientation orientation )
{
    // using 3 timers.  If the press timer is active, stop it, assuming the value actually changed.

    // TODO::The value check here is a work around for a bug in hbscrollbar.
    // the bug is that the value changed signal is emitted when the thumb
    // is pressed, and again when it is released, regaurdless of if there was a value change.
    // once that bug is fixed, This should be just setting the dwell interval,
    // starting the timer, and showing the index feedback.
    if (value != mScrollBarValue && orientation == mWidget->scrollDirection()) {
        showIndexFeedback();
    }
}

/*
    The private slot for hiding the index feedback.

    If effects are active, use the disappear effect to hide the index feedback's
    primitives.  Otherwise simply hide them.
*/
void HgIndexFeedbackPrivate::_q_hideIndexFeedback()
{
    if (qFuzzyCompare(mTextItem->opacity(), (qreal) 1.0)) {
        HbEffect::start(mPopupItemList, HB_INDEXFEEDBACK_TYPE, EFFECT_IFDISAPPEAR);
    }

    if (mDisappearTimer) {
        mDisappearTimer->start();
    }
}

/*
    A private slot for actually calling hide on the index feedback.

    This should happen after the index feedback's hide animation is completed
    regardless of if it went successfully.
*/
void HgIndexFeedbackPrivate::_q_hideIndexFeedbackNow()
{
    if (mTextItem) {
        mTextItem->hide();
    }

    if (mPopupItem) {
        mPopupItem->hide();
    }
}

/*
    Do the appropriate thing if the item view we're pointing at is destroyed
*/
void HgIndexFeedbackPrivate::_q_itemViewDestroyed()
{
    mWidget = 0;
}

/*
    Update the primitives for the index feedback.
*/
void HgIndexFeedbackPrivate::updatePrimitives()
{
    Q_Q( HgIndexFeedback );

    if (mTextItem) {
        if (HbTextItem* textItem = qgraphicsitem_cast<HbTextItem*>(mTextItem)) {
            textItem->setFontSpec(FontSpec());
            textItem->setGeometry(mPopupTextRect);
            textItem->setText(mPopupContent);
        }
    }
    if (mPopupItem) {
        if (HbFrameItem* frameItem = qgraphicsitem_cast<HbFrameItem*>(mPopupItem)) {
            frameItem->setGeometry(mPopupBackgroundRect);
        }
    }
}

/*
    Create the primitives for the index feedback.
*/
void HgIndexFeedbackPrivate::createPrimitives()
{
    Q_Q( HgIndexFeedback );

    mPopupItemList.clear();

    if (!mTextItem) {
        HbTextItem *textItem = new HbTextItem(q);
        textItem->setAlignment(Qt::AlignCenter);
        textItem->setTextWrapping(Hb::TextNoWrap);
        HbStyle::setItemName(textItem, QLatin1String("index-text"));
        textItem->setZValue(q->zValue()+2);
        mTextItem = textItem;
        mTextItem->hide();
        mPopupItemList.append(mTextItem);
    }

    if (!mPopupItem) {
        HbFrameItem *frame = new HbFrameItem(q);
        frame->frameDrawer().setFrameGraphicsName("qtg_fr_popup_preview");
        frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        qreal cornerPieceSize = 0;
        q->style()->parameter(QLatin1String("hb-param-background-popup-preview"),cornerPieceSize);
        frame->frameDrawer().setBorderWidths(cornerPieceSize, cornerPieceSize);
        HbStyle::setItemName(frame, QLatin1String("index-background"));
        frame->setZValue(q->zValue()+1);
        mPopupItem = frame;
        mPopupItem->hide();
        mPopupItemList.append(mPopupItem);
    }
}

/*
    disconnects the current item view from HgIndexFeedback's slots & event filters.
*/
void HgIndexFeedbackPrivate::disconnectItemView()
{
    Q_Q( HgIndexFeedback );

    if (mWidget) {
        // disconnect from the existing itemView's scrollbars
        mWidget->disconnect(q);
        // uninstall the event filters;
        mWidget->scrollBar()->removeEventFilter(q);
        mWidget->removeEventFilter(q);
        
        mWidget->removeSceneEventFilter(q);
        if (mWidget->scene()) {
            mWidget->scene()->removeItem(q);
        }
    }

    mWidget = 0;
}

/*
    Hooks up the private slots & event filter to a scrollbar.
*/
void HgIndexFeedbackPrivate::connectWidgetToIndexFeedback()
{
    Q_Q( HgIndexFeedback );

    if(!mWidget) return;

    // Install event filter so we receive resize events from parent widget.
    mWidget->installEventFilter(q);

    // Install eventfilter to scrollbar so we can receive mousepress and released events
    HbScrollBar *scrollBar = mWidget->scrollBar();    
    if (scrollBar) {
        scrollBar->installEventFilter(q);
    }
}

/*
    Calculates the rects for the popup text and background.

    Does nothing if the rect is the same as the last time it was called.
*/
void HgIndexFeedbackPrivate::calculatePopupRects()
{
    Q_Q( HgIndexFeedback );

    if (!mWidget) {
        return;
    }

    QRectF contentRect = mWidget->boundingRect();
    
    HbScrollBar *scrollBar = mWidget->scrollBar();
    if (scrollBar->isInteractive() && mWidget->scrollDirection() == Qt::Vertical) {
        contentRect.setWidth( contentRect.width() - scrollBar->rect().width() );
        if (HbApplication::layoutDirection() == Qt::RightToLeft) {
            contentRect.setLeft( contentRect.left() + scrollBar->rect().width() );
        }
    }    
    else if (scrollBar->isInteractive()) {
        contentRect.setHeight( contentRect.height() - scrollBar->rect().height() );
    }

    if (contentRect == mItemViewContentsRect) {
        return;
    }
        
    qreal margin = 0;
    q->style()->parameter(QLatin1String("hb-param-margin-gene-popup"), margin);

    QSizeF backgroundSize;
    QSizeF textSize;

    switch (mIndexFeedbackPolicy) {
        case HgWidget::IndexFeedbackSingleCharacter:
        case HgWidget::IndexFeedbackThreeCharacter:
            {
                textSize.setHeight( textHeight() );
                textSize.setWidth ( textWidth() );

                backgroundSize.setHeight( textSize.height() + 2 * margin );
                backgroundSize.setWidth ( textSize.width() + 2 * margin );

                mPopupBackgroundRect.setLeft( contentRect.left() + (contentRect.width() -
                    backgroundSize.width()) / 2.0 );
                mPopupBackgroundRect.setTop ( contentRect.top() + (contentRect.height() - 
                    backgroundSize.height()) / 2.0 );

                mPopupTextRect.setLeft( mPopupBackgroundRect.left() + margin );
                mPopupTextRect.setTop ( mPopupBackgroundRect.top()  + margin );

                mPopupBackgroundRect.setSize(backgroundSize);
                mPopupTextRect.setSize(textSize);
            }
            break;

        case HgWidget::IndexFeedbackString:
            {
                textSize.setHeight( textHeight() );
                backgroundSize.setHeight( textSize.height() + 2 * margin );

                backgroundSize.setWidth( contentRect.width() - 2 * mStringOffset );
                textSize.setWidth( backgroundSize.width() - 2 * margin );

                mPopupBackgroundRect.setLeft( contentRect.left() + mStringOffset );
                mPopupBackgroundRect.setTop( contentRect.top() + (contentRect.height() - 
                    backgroundSize.height()) / 2.0 );

                mPopupTextRect.setLeft( mPopupBackgroundRect.left() + margin );
                mPopupTextRect.setTop ( mPopupBackgroundRect.top() + margin );

                mPopupBackgroundRect.setSize(backgroundSize);
                mPopupTextRect.setSize(textSize);
            }
            break;

        case HgWidget::IndexFeedbackNone:
            {
                mPopupTextRect = QRectF();
                mPopupBackgroundRect = QRectF();
            }
            break;
    }
}

/*
    Returns the text height in pixels.
*/
qreal HgIndexFeedbackPrivate::textHeight() const
{
    Q_Q( const HgIndexFeedback );

    qreal retVal;

    switch (mIndexFeedbackPolicy) {
        case HgWidget::IndexFeedbackNone:
            retVal = 0.0;
            break;

        case HgWidget::IndexFeedbackSingleCharacter:
            retVal = mOneCharHeight;
            break;

        case HgWidget::IndexFeedbackThreeCharacter:
            retVal = mThreeCharHeight;
            break;

        case HgWidget::IndexFeedbackString:
            q->style()->parameter(QLatin1String("hb-param-text-height-primary"), retVal);
            break;
    }

    return retVal;
}

/*
    Returns the text width in units.
*/
qreal HgIndexFeedbackPrivate::textWidth() const
{
    qreal retVal = 0.0;

    switch (mIndexFeedbackPolicy) {
        case HgWidget::IndexFeedbackString:
        case HgWidget::IndexFeedbackNone:
            retVal = 0.0;
            break;

        case HgWidget::IndexFeedbackSingleCharacter:
            retVal = mOneCharHeight;
            break;

        case HgWidget::IndexFeedbackThreeCharacter:
            retVal = mThreeCharWidth;
            break;
    }

    return retVal;
}

/*
    Connects model currentSelectionChanged slot to index feedback.
 */
void HgIndexFeedbackPrivate::connectModelToIndexFeedback(QItemSelectionModel* model)
{
    Q_Q(HgIndexFeedback);
    
    if (!model)
        return;
    
    q->connect(model, SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
        q, SLOT(_q_currentModelIndexChanged(const QModelIndex&, const QModelIndex&)));
    
}

void HgIndexFeedbackPrivate::_q_currentModelIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(current)
    Q_UNUSED(previous)
    
    if (mScrollBarPressed)
        updateIndex();
}

HbFontSpec HgIndexFeedbackPrivate::FontSpec() const
{
    HbFontSpec fontSpec(HbFontSpec::Primary);
    switch (mIndexFeedbackPolicy) {
        case HgWidget::IndexFeedbackSingleCharacter:
            {
            fontSpec.setTextHeight(textHeight());
            }
            break;
        case HgWidget::IndexFeedbackThreeCharacter:
            {
            fontSpec.setTextHeight(textHeight());
            }
            break;
        case HgWidget::IndexFeedbackString:
            {
            qreal textPaneHeight = 0;
            Q_Q(const HgIndexFeedback);
            q->style()->parameter(QLatin1String("hb-param-text-height-primary"), textPaneHeight);
            fontSpec.setTextHeight( textPaneHeight );
            }
            break;
        case HgWidget::IndexFeedbackNone:
        default:
            break;
    }
    return fontSpec;
}

