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

#ifndef HGINDEXFEEDBACKPRIVATE_H
#define HGINDEXFEEDBACKPRIVATE_H

#include "hgindexfeedback.h"
#include <hbwidget_p.h>

#define HB_INDEXFEEDBACK_TYPE "indexfeedback"
#define EFFECT_IFAPPEAR "appear"
#define EFFECT_IFDISAPPEAR "disappear"

QT_BEGIN_NAMESPACE
class QGraphicsWidget;
class QGraphicsItem;
class QRectF;
class QItemSelectionModel;
QT_END_NAMESPACE

class HgIndexFeedbackPrivate: public HbWidgetPrivate
{
    Q_DECLARE_PUBLIC( HgIndexFeedback )

public:
    HgIndexFeedbackPrivate();
    virtual ~HgIndexFeedbackPrivate();
    void init();

    void showIndexFeedback();
    void updateIndex();

    void scrollBarPressed();
    void scrollBarReleased();

    void connectScrollBarToIndexFeedback(HbScrollBar* scrollBar);
    void disconnectItemView();
    void connectModelToIndexFeedback(QItemSelectionModel* model);

    void _q_scrollPositionChanged(qreal value, Qt::Orientation orientation);
    void _q_hideIndexFeedback();
    void _q_itemViewDestroyed();
    void _q_hideIndexFeedbackNow();
    void _q_currentModelIndexChanged(const QModelIndex& current, const QModelIndex& previous);

    void updatePrimitives();
    void createPrimitives();

    void calculatePopupRects();
    qreal textHeight() const;
    qreal textWidth() const;

    QString displayText(const QVariant &data) const;

public:
    int mIndexFeedbackPressTimeout;
    int mIndexFeedbackDwellTimeout;
    int mIndexFeedbackReleaseTimeout;
    HgWidget::IndexFeedbackPolicy mIndexFeedbackPolicy;

    // TODO::The values are storred here is a work around for a bug in hbscrollbar.
    // the bug is that the value changed signal is emitted when the thumb
    // is pressed, and again when it is released, regaurdless of if there was a value change.
    // once that bug is fixed, this should be removed.
    qreal mScrollBarValue;

    QTimer *mIndexFeedbackTimer;
    QTimer *mDisappearTimer;

    QString mPopupContent;
    QGraphicsItem *mTextItem;
    QGraphicsItem *mPopupItem;

    QRectF mPopupTextRect;
    QRectF mPopupBackgroundRect;
    QRectF mItemViewContentsRect;

    qreal mOneCharHeight;
    qreal mThreeCharHeight;
    qreal mThreeCharWidth;
    qreal mStringOffset;

    HgWidget* mWidget;
    QList<QGraphicsItem*> mPopupItemList;
    bool mScrollBarPressed;
};

#endif //HBINDEXFEEDBACKPRIVATE_H
