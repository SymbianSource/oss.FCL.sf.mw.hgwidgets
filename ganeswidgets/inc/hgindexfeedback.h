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

#ifndef HGINDEXFEEDBACK_H
#define HGINDEXFEEDBACK_H

#include <HbGlobal>
#include <HbWidget>
#include <hgwidgets/hgwidgets.h>

class HbScrollBar;
class HgIndexFeedbackPrivate;
class HbStyleOptionIndexFeedback;
class HgWidget;

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

class HgIndexFeedback : public HbWidget
{
    Q_OBJECT

public:

    explicit HgIndexFeedback(QGraphicsItem *parent=0);
    virtual ~HgIndexFeedback();

    enum { Type = Hb::ItemType_IndexFeedback };
    int type() const { return Type; }
    
    void setIndexFeedbackPolicy( HgWidget::IndexFeedbackPolicy policy);
    HgWidget::IndexFeedbackPolicy indexFeedbackPolicy() const;

    void setWidget(HgWidget *itemView);
    HgWidget* widget() const;

    virtual QGraphicsItem *primitive(HbStyle::Primitive primitive) const;

protected:
    virtual void polish( HbStyleParameters& params);
    virtual bool sceneEventFilter(QGraphicsItem* watched, QEvent* event);
    virtual bool eventFilter(QObject *obj, QEvent *event);
    void initStyleOption(HbStyleOptionIndexFeedback *option) const;

private:
    Q_DECLARE_PRIVATE_D(d_ptr, HgIndexFeedback)
    Q_DISABLE_COPY(HgIndexFeedback)

    Q_PRIVATE_SLOT(d_func(), void _q_scrollPositionChanged(qreal, Qt::Orientation))
    Q_PRIVATE_SLOT(d_func(), void _q_hideIndexFeedback())
    Q_PRIVATE_SLOT(d_func(), void _q_itemViewDestroyed())
    Q_PRIVATE_SLOT(d_func(), void _q_hideIndexFeedbackNow())
    Q_PRIVATE_SLOT(d_func(), void _q_currentModelIndexChanged(QModelIndex, QModelIndex))
};

#endif // HGINDEXFEEDBACK_H
