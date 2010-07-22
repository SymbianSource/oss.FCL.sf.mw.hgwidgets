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

#ifndef HGGRID_H
#define HGGRID_H

#include <hgwidgets/hgwidgets.h>

class HgGridPrivate;

class HG_WIDGETS_EXPORT HgGrid : public HgWidget
{
    Q_OBJECT
    Q_PROPERTY(bool effect3dEnabled  READ effect3dEnabled  WRITE setEffect3dEnabled )
public:

    explicit HgGrid(Qt::Orientation scrollDirection, QGraphicsItem *parent = 0 );
    virtual ~HgGrid();

    bool effect3dEnabled() const;
    void setEffect3dEnabled(bool effect3dEnabled);
    
protected:
    HgGrid(Qt::Orientation scrollDirection, HgGridPrivate &dd, QGraphicsItem *parent);

private:
    Q_DECLARE_PRIVATE_D(p_ptr, HgGrid)
    Q_DISABLE_COPY(HgGrid)
};

#endif  //HGGRID_H

