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
* Description: Container for pan (drag) gesture -related data and logic.
*
*/

#ifndef HGDRAG_H
#define HGDRAG_H

#include <QTime>

class HgDrag
{
public:
    HgDrag();
    void reset(const qreal delta, const qreal viewPos);
    qreal update(const qreal delta, const qreal viewPos, const qreal itemWidth);
    bool finish(const qreal viewPos, bool stopToFullItem, qreal &newPos);
    qreal viewPosAtStart() const;
    
private:
    QTime mDuration;
    qreal mViewPosAtStart;
    qreal mLength;
    qreal mCumulatedDelta;
    int mDirection;
    int mDirectionChange;
};

#endif // HGDRAG_H
