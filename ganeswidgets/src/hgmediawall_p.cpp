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

#include "hgmediawall_p.h"
#include "hgcoverflowcontainer.h"
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
    HgCoverflowContainer* container = new HgCoverflowContainer(q);
    // Mediawall supports only horizontal scrolling.
    container->init(orientation);
    HgWidgetPrivate::init(container);
    
    q->connect( container, SIGNAL(animationAboutToEnd(QModelIndex)),
                q, SIGNAL(animationAboutToEnd(QModelIndex)) );
}

void HgMediawallPrivate::setTitlePosition(HgMediawall::LabelPosition position)
{
    FUNC_LOG;

    container()->setTitlePosition(position);
}

HgMediawall::LabelPosition HgMediawallPrivate::titlePosition() const
{
    FUNC_LOG;

    return container()->titlePosition();
}

void HgMediawallPrivate::setDescriptionPosition(HgMediawall::LabelPosition position)
{
    FUNC_LOG;

    container()->setDescriptionPosition(position);
}

HgMediawall::LabelPosition HgMediawallPrivate::descriptionPosition() const
{
    FUNC_LOG;

    return container()->descriptionPosition();
}

void HgMediawallPrivate::setTitleFontSpec(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    container()->setTitleFontSpec(fontSpec);
}

HbFontSpec HgMediawallPrivate::titleFontSpec() const
{
    FUNC_LOG;

    return container()->titleFontSpec();
}

void HgMediawallPrivate::setDescriptionFontSpec(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    container()->setDescriptionFontSpec(fontSpec);
}

HbFontSpec HgMediawallPrivate::descriptionFontSpec() const
{
    FUNC_LOG;

    return container()->descriptionFontSpec();
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

// EOF
