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

#ifndef HGGRID_P_H
#define HGGRID_P_H

#include <hgwidgets/hggrid.h>
#include "hgwidgets_p.h"

class HgGridPrivate : public HgWidgetPrivate
{
    Q_DECLARE_PUBLIC(HgGrid)

public:

    HgGridPrivate();
    virtual ~HgGridPrivate();

    void init(Qt::Orientation scrollDirection);

private:

};

#endif  //HGGRID_P_H

