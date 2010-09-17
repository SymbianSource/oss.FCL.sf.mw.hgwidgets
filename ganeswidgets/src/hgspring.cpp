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

#include "hgspring.h"
#include <QTimer>
#include "trace.h"

const int KTimeDelta(10);
const qreal KTimeDeltaF(0.01f);
const qreal KVelocitySnap(0.06f);
const qreal KPositionSnap(0.01f);
const int KTimerInterval(10);

HgSpring::HgSpring() :
mStartPos(QPointF(0,0)),
mPos(QPointF(0,0)),
mEndPos(QPointF(0,0)),
mEndPosOverListBoundary(QPointF(0,0)),
mVelocity(QPointF(0,0)),
mK(30.1),
mDamping(10.1),
mAccumulator(0.0),
mDoNotUpdate(false),
mEndPosOverListEdge(false)
{
    mTimer = new QTimer(this);

    QObject::connect( mTimer, SIGNAL( timeout() ), this, SLOT( update() ) );

}

HgSpring::~HgSpring()
{

}

void HgSpring::setK(qreal k)
{
    mK = k;
}

void HgSpring::setDamping(qreal damping)
{
    mDamping = damping;
}

qreal HgSpring::k() const
{
    return mK;
}

qreal HgSpring::damping() const
{
    return mDamping;
}

// TODO: Remove this function and use only the animateToPosAfterPanning version?
void HgSpring::animateToPos(const QPointF& pos)
{    
    if (mPos == pos) {
        // No need to animate, we are already in correct position.
        return;
    }
            
    mStartPos = mPos;
    mEndPos = pos;


    if (!mTimer->isActive())
    {
        emit started();
        mTimer->start(KTimerInterval);
        mPrevTime.start();
    }
}

void HgSpring::animateToPosAfterPanning(const QPointF& pos, qreal worldWidth)
{    
    mWorldWidth = worldWidth;
    mStartPos = mPos;
    
    qreal xPos = pos.x();
    if( xPos < 0.0 )
    {
        mEndPosOverListEdge = true;
        mEndPosOverListBoundary = pos;
        mEndPos = QPointF(0, 0);
    }
    else if( xPos > worldWidth )
    {
        mEndPosOverListEdge = true;
        mEndPosOverListBoundary = pos;
        mEndPos = QPointF(worldWidth, 0);
    }
    else
    {
        mEndPosOverListEdge = false;
        mEndPos = pos;
    }

    emit started();

    if (!mTimer->isActive())
    {
        mTimer->start(KTimerInterval);
        mPrevTime.start();
    }
}

void HgSpring::gotoPos(const QPointF& pos)
{
    if (mTimer->isActive())
    {
        mTimer->stop();
    }

    mPos = pos;
    mEndPos = pos;
}

void HgSpring::cancel()
{
    if (mTimer->isActive()) {
        mEndPosOverListEdge = false;
        mEndPos = mPos;
        emit ended();
        mTimer->stop(); 
    }
    
}

const QPointF& HgSpring::startPos() const
{
    return mStartPos;
}

const QPointF& HgSpring::pos() const
{
    return mPos;
}

const QPointF& HgSpring::endPos() const
{
    return mEndPos;
}

const QPointF& HgSpring::velocity() const
{
    return mVelocity;
}

void HgSpring::update()
{
    int deltaTime = mPrevTime.elapsed();

    mPrevTime.start();

    mAccumulator += deltaTime;

    bool stopped = false;
    while (mAccumulator >= KTimeDelta)
    {
        QPointF delta;
        if(mEndPosOverListEdge)
        {
            delta = mEndPosOverListBoundary - mPos;
            
            if( mPos.x() < KPositionSnap || mPos.x() > mWorldWidth )
            {
                // When list's position goes past the world boundary
                // we reset our mEndPosOverListEdge boolean flag
                // -> the passed boundary will be used as end point,
                // and the K value of this spring will be modified.
                mEndPosOverListEdge = false; //reset
                mEndPosOverListBoundary = QPointF(0,0); //reset
                mWorldWidth = 0.0; //reset
                mK = 60.0;
            }
            
        }
        else
        {
            delta = mEndPos - mPos;
        }
        
        QPointF force = delta * mK - mVelocity * mDamping;
        mVelocity += force * KTimeDeltaF;
        mPos += mVelocity * KTimeDeltaF;
        
        if ( (qAbs(mPos.x() - mEndPos.x()) < KPositionSnap &&
                  qAbs(mPos.y() - mEndPos.y()) < KPositionSnap)
             && qAbs(mVelocity.x()) < KVelocitySnap )
        {
            mPos = mEndPos;
            mAccumulator = 0;
            mVelocity = QPointF(0,0);
            mTimer->stop();
            stopped = true;
            break;
        }

        mAccumulator -= KTimeDelta;
    }
    
    if (!mDoNotUpdate)
        emit updated();
    
    if (stopped)
        emit ended();

}

bool HgSpring::isActive() const
{
    return mTimer->isActive();
}

bool HgSpring::updatePositionIfNeeded()
{
    if (isActive() && mPrevTime.elapsed() > KTimeDelta) {
        mDoNotUpdate = true;
        update();
        mDoNotUpdate = false;
        mTimer->stop();
        mTimer->start(KTimerInterval);
        return true;
    }
    return false;
}

void HgSpring::resetVelocity()
{
    mVelocity = QPointF(0,0);
}

