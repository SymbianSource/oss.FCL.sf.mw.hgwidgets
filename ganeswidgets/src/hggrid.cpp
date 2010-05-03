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

#include <hgwidgets/hggrid.h>
#include "hggrid_p.h"
#include "hggridcontainer.h"

HgGrid::HgGrid( Qt::Orientation scrollDirection, QGraphicsItem *parent ):
    HgWidget( *new HgGridPrivate, parent )
{
    Q_D(HgGrid);
    d->q_ptr = this;

    d->init(scrollDirection);
}

HgGrid::HgGrid( Qt::Orientation scrollDirection, HgGridPrivate &dd, QGraphicsItem *parent) : 
    HgWidget( dd, parent )
{
    Q_D( HgGrid );
    d->q_ptr = this;    
    
    d->init(scrollDirection);
}


HgGrid::~HgGrid()
{
}

// EOF
