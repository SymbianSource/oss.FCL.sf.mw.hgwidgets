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

#include "hgdrag.h"
#include "trace.h"

HgDrag::HgDrag()
{
}

void HgDrag::reset(const qreal delta, const qreal viewPos)
{
    mViewPosAtStart = viewPos;
    mDuration.start();
    mLength = 0;
    mCumulatedDelta = 0;
    mDirection = delta;
    mDirectionChange = 0;
    INFO("Drag reset at view pos:" << mViewPosAtStart);
}

qreal HgDrag::update(const qreal delta, const qreal viewPos, const qreal itemWidth)
{
//    INFO("delta:" << delta << ", direction:" << mDirection);
    if ((delta > 0 && mDirection < 0) || (delta < 0 && mDirection > 0))
    {
        if (mDirectionChange > 2)
        {
            mDirectionChange = 0;
            reset(delta, viewPos);
        }
        else
        {
            mDirectionChange++;
        }
    }
    else
    {
        mDirection = delta;
        mDirectionChange = 0;
    }

    mCumulatedDelta = (mCumulatedDelta+delta)/2;
    mLength += delta/itemWidth;
    qreal pos = mViewPosAtStart-mLength;
//  INFO("Drag len:" << mLength << ", new pos:" << pos);
    return pos;
}

bool HgDrag::finish(const qreal viewPos, bool stopToFullItem, qreal &newPos)
{
    int dt = mDuration.elapsed();
    if (dt > 10)
    {
        qreal t = (qreal)dt/1000;
        INFO("Drag len:" << mLength << ", duration:" << t << ", cumulated:" << mCumulatedDelta);
        if (qAbs(mCumulatedDelta) > 4)
        {
            newPos = viewPos-mLength/t * 0.5f;
        }
        else
        {
            newPos = mViewPosAtStart-mLength;
        }
        if (stopToFullItem)
        {
            qreal i = floorf(newPos);
            newPos = (newPos - i > 0.5f) ? ceilf(newPos) : i;
        }

        return true;
    }
    return false;
}

qreal HgDrag::viewPosAtStart() const
{
    return mViewPosAtStart;
}

