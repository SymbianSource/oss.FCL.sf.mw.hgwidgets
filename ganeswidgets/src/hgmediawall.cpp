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

#include <hgwidgets/hgmediawall.h>
#include "hgmediawall_p.h"
#include "hgwidgets_p.h"
#include "hgcoverflowcontainer.h"
#include <hbmainwindow>

HgMediawall::HgMediawall(QGraphicsItem *parent ) :
    HgWidget( *new HgMediawallPrivate, parent )
{
    Q_D(HgMediawall);
    d->q_ptr = this;
    d->init(mainWindow()->orientation());
}

HgMediawall::HgMediawall(HgMediawallPrivate &dd, QGraphicsItem *parent) : 
    HgWidget( dd, parent )
{
    Q_D(HgMediawall);
    d->q_ptr = this;    
    d->init(mainWindow()->orientation());
}

HgMediawall::~HgMediawall()
{
}

/*!
    Sets the placement of the title.
*/
void HgMediawall::setTitlePosition(LabelPosition position)
{
    Q_D(HgMediawall);
    d->setTitlePosition(position);
}

/*!
    Returns the placement of the title.
*/
HgMediawall::LabelPosition HgMediawall::titlePosition() const
{
    Q_D(const HgMediawall);
    return d->titlePosition();
}

/*!
    Sets the placement of the description.
*/
void HgMediawall::setDescriptionPosition(LabelPosition position)
{
    Q_D(HgMediawall);
    d->setDescriptionPosition(position);
}

/*!
    Returns the placement of the description.
*/
HgMediawall::LabelPosition HgMediawall::descriptionPosition() const
{
    Q_D(const HgMediawall);
    return d->descriptionPosition();
}

/*!
    Sets the fontSpec property of the title.

    The font specification defines the font with a font role and optional
    other parameters.
*/
void HgMediawall::setTitleFontSpec(const HbFontSpec &fontSpec)
{
    Q_D(HgMediawall);
    d->setTitleFontSpec(fontSpec);
}

/*!
    Returns the fontSpec property of the title.
*/
HbFontSpec HgMediawall::titleFontSpec() const
{
    Q_D(const HgMediawall);
    return d->titleFontSpec();
}

/*!
    Sets the fontSpec property of the description.

    The font specification defines the font with a font role and optional
    other parameters.
*/
void HgMediawall::setDescriptionFontSpec(const HbFontSpec &fontSpec)
{
    Q_D(HgMediawall);
    d->setDescriptionFontSpec(fontSpec);
}

/*!
    Returns the fontSpec property of the description.
*/
HbFontSpec HgMediawall::descriptionFontSpec() const
{
    Q_D(const HgMediawall);
    return d->descriptionFontSpec();
}

/*!
 *  Sets front item delta from the center of the widget.
 */
void HgMediawall::setFrontItemPositionDelta(const QPointF& position)
{
    Q_D(HgMediawall);
    return d->container()->setFrontItemPositionDelta(position);
}

/*!
 * Gets current front items position delta.
 */
QPointF HgMediawall::frontItemPositionDelta() const
{
    Q_D(const HgMediawall);
    return d->container()->frontItemPositionDelta();
}

/**
 * Enables/disables reflections in the mediawall.
 */
void HgMediawall::enableReflections(bool enabled)
{
    Q_D(HgMediawall);
    d->container()->enableReflections(enabled);
}

/**
 * Returns true if reflections are enabled.
 */
bool HgMediawall::reflectionsEnabled() const
{
    Q_D(const HgMediawall);
    return d->container()->reflectionsEnabled();
}

// EOF
