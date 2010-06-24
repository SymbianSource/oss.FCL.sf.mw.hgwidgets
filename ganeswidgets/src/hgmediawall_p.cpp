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

#include <HbTextItem>
#include "hgmediawall_p.h"
#include "hgwidgetitem.h"
#include "hgcoverflowcontainer.h"
#include "hgcenteritemarea.h"
#include "trace.h"

HgMediawallPrivate::HgMediawallPrivate() : HgWidgetPrivate()
{
    FUNC_LOG;
    mStaticScrollDirection = true;
}

HgMediawallPrivate::~HgMediawallPrivate()
{
    FUNC_LOG;
}

void HgMediawallPrivate::init(Qt::Orientation orientation)
{
    FUNC_LOG;

    Q_Q(HgMediawall);

    mTitleItem = new HbTextItem("", q);
    q->style()->setItemName(mTitleItem, "title");

    mDescriptionItem = new HbTextItem("", q);
    q->style()->setItemName(mDescriptionItem, "description");

    mCenterItemArea = new HgCenterItemArea(q);
    q->style()->setItemName(mCenterItemArea, "centeritem");

    HgCoverflowContainer* container = new HgCoverflowContainer(q);
    q->style()->setItemName(container, "content");
    container->setCenterItemArea(mCenterItemArea);
    // Mediawall supports only horizontal scrolling.
    container->init(orientation);

    HgWidgetPrivate::init(container);

    q->connect( container, SIGNAL(animationAboutToEnd(QModelIndex)),
                q, SIGNAL(animationAboutToEnd(QModelIndex)) );
}

void HgMediawallPrivate::setTitleFontSpec(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    Q_Q(HgMediawall);
    if (mTitleItem) {
        mTitleItem->setFontSpec(fontSpec);
    }
}

HbFontSpec HgMediawallPrivate::titleFontSpec() const
{
    FUNC_LOG;

    if (mTitleItem) {
        return mTitleItem->fontSpec();
    }
    return HbFontSpec();
}

void HgMediawallPrivate::setDescriptionFontSpec(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    Q_Q(HgMediawall);
    if (mDescriptionItem) {
        mDescriptionItem->setFontSpec(fontSpec);
    }
}

HbFontSpec HgMediawallPrivate::descriptionFontSpec() const
{
    FUNC_LOG;

    if (mDescriptionItem) {
        return mDescriptionItem->fontSpec();
    }
    return HbFontSpec();
}

HgCoverflowContainer *HgMediawallPrivate::container()
{
    HANDLE_ERROR_NULL(mContainer);
    return qobject_cast<HgCoverflowContainer *>(mContainer);
}

const HgCoverflowContainer *HgMediawallPrivate::container() const
{
    HANDLE_ERROR_NULL(mContainer);
    return qobject_cast<const HgCoverflowContainer *>(mContainer);
}

void HgMediawallPrivate::updateCurrentItem(const QModelIndex &currentItem)
{
    FUNC_LOG;

    if (!mContainer || !currentItem.isValid()) return;

    HgWidgetItem* item = mContainer->itemByIndex(currentItem);
    INFO("Updating texts for index" << currentItem << ", " << item);

    if (item) {
        mTitleItem->setText(item->title());
        mDescriptionItem->setText(item->description());
    }
}

// EOF
