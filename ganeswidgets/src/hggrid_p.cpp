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

#include "hggrid_p.h"
#include "hggridcontainer.h"

static const int BUFFERSIZE(120);

HgGridPrivate::HgGridPrivate() : HgWidgetPrivate()
{

}

HgGridPrivate::~HgGridPrivate()
{

}

void HgGridPrivate::init(Qt::Orientation scrollDirection)
{
    Q_Q(HgGrid);
    HgGridContainer *container = new HgGridContainer(q);
    container->init(scrollDirection);

    // Use a little larger buffer for the grid than the default one.
    mBufferSize = BUFFERSIZE;

    HgWidgetPrivate::init(container);
    q->setPinchEnabled(false);
    
    q->connect(mContainer, SIGNAL(emptySpacePressed()),
               q, SIGNAL(emptySpacePressed()));    
}

void HgGridPrivate::orientationChanged(Qt::Orientation orientation)
{
    Q_Q(HgGrid);

    HgWidgetPrivate::orientationChanged(orientation);
    q->repolish();
}

HgGridContainer* HgGridPrivate::container()
{
    return qobject_cast<HgGridContainer*>(mContainer);
}

const HgGridContainer* HgGridPrivate::container() const
{
    return qobject_cast<const HgGridContainer*>(mContainer);
}


// EOF
