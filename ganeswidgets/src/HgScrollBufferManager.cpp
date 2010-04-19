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


#include <QTimer>

#include "HgScrollBufferManager.h"
#include "trace.h"

// -----------------------------------------------------------------------------
// HgScrollBufferManager::HgScrollBufferManager()
// -----------------------------------------------------------------------------
//
HgScrollBufferManager::HgScrollBufferManager(
        int bufferSize,
        int bufferTreshold,
        int initialPosition,
        int totalCount )
:   mBufferSize( bufferSize ),
    mBufferTreshold( bufferTreshold ),
    mBufferPosition( initialPosition ),
    mDiff(0),
    mTotalCount( totalCount ),
    mResetOrdered(false),
    mRequestStart(0),
    mRequestCount(0),
    mReleaseStart(0),
    mReleaseCount(0)
    {
    init();
    }

// -----------------------------------------------------------------------------
// HgScrollBufferManager::init()
// -----------------------------------------------------------------------------
//
void HgScrollBufferManager::init()
    {
    mResetOrdered = ETrue;
    mTimer.setSingleShot(true);
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

// -----------------------------------------------------------------------------
// HgScrollBufferManager::~HgScrollBufferManager()
// -----------------------------------------------------------------------------
//
HgScrollBufferManager::~HgScrollBufferManager()
    {
    mTimer.stop();
    }

// -----------------------------------------------------------------------------
// HgScrollBufferManager::resetBuffer()
// -----------------------------------------------------------------------------
//
void HgScrollBufferManager::resetBuffer( int aPosition, int totalCount )
    {
    if( !mResetOrdered )
        {
        // release Old buffer
        mReleaseStart = mBufferPosition;
        mReleaseCount = mBufferSize;
        }

    // set position and count
    mBufferPosition = aPosition - (mBufferSize / 2);
    mTotalCount = totalCount;
    mDiff = 0;

    if( mBufferPosition + mBufferSize > mTotalCount - 1 )
        {
        mBufferPosition = mTotalCount - mBufferSize;
        }

    if(mBufferPosition < 0 )
        {
        mBufferPosition = 0;
        }

    //request new Buffer
    mRequestStart = mBufferPosition;
    mRequestCount = mBufferSize;
    mResetOrdered = ETrue;
    asyncUpdate();
    }

void HgScrollBufferManager::scrollPositionChanged( int newPosition )
    {
    // If all the items fit in the buffer no need to move the buffer.
    if( mTotalCount <= mBufferSize ) return;

    bool forceUpdate = EFalse;
    newPosition -= mBufferSize / 2; // normalize index to Buffer start

    if(newPosition < 0)
        {
        newPosition = 0;
        forceUpdate = ETrue;
        }
    else if( newPosition > mTotalCount - mBufferSize )
        {
        newPosition = mTotalCount - mBufferSize;
        forceUpdate = ETrue;
        }

    mDiff = mBufferPosition - newPosition;

    // Too large change reset whole buffer
    if( mDiff >= mBufferSize || -mDiff >= mBufferSize || mResetOrdered )
        {
        resetBuffer(newPosition + (mBufferSize/2), mTotalCount );
        }
    // Move Up
    else if( mDiff >= mBufferTreshold )
        {
        mRequestCount = mDiff;
        mReleaseCount = mDiff;
        asyncUpdate();
        }
    // Move Down
    else if( -mDiff >= mBufferTreshold )
        {
        mRequestCount = -mDiff;
        mReleaseCount = -mDiff;
        asyncUpdate();
        }
    // Top or bottom has been reached
    else if( forceUpdate && mDiff )
        {
        int diff = mDiff < 0 ? -mDiff : mDiff;
        mRequestCount = diff;
        mReleaseCount = diff;
        asyncUpdate();
        }
    }

void HgScrollBufferManager::timeout()
{
    if(mResetOrdered)
        {
        mResetOrdered = EFalse;
        }
    else
        {
        if(mDiff < 0)
            {
            mReleaseStart = mBufferPosition;
            mRequestStart = mBufferPosition + mBufferSize;
            }
        else if( mDiff > 0)
            {
            mReleaseStart = mBufferPosition + mBufferSize - mDiff;
            mRequestStart = mBufferPosition - mDiff;
            }
        }

    // Release
    int end = mReleaseStart + mReleaseCount < mTotalCount ?
        mReleaseStart + mReleaseCount: mTotalCount;
    end--;
    if(end >= mReleaseStart )
        {
        emit releaseItems( mReleaseStart, end );
        }

    mReleaseCount = 0;

    // Request
    end = mRequestStart + mRequestCount < mTotalCount ?
        mRequestStart + mRequestCount : mTotalCount;

    end--;
    if(end >= mRequestStart )
        {
        emit requestItems( mRequestStart, end );
        }

    mRequestCount = 0;

    // Move Buffer
    mBufferPosition -= mDiff;
    // Reset Diff
    mDiff = 0;
}

bool HgScrollBufferManager::positionInsideBuffer( int position )
{
    return position >= mBufferPosition && position <= (mBufferPosition+mBufferSize);
}

void HgScrollBufferManager::asyncUpdate()
{
    if( !mTimer.isActive())
        mTimer.start(0);
}

void HgScrollBufferManager::currentBuffer(int& bufferStart, int& bufferEnd)
{
    bufferStart = mBufferPosition;
    bufferEnd = mBufferPosition+mBufferSize > mTotalCount-1 ?
        mTotalCount-1 : mBufferPosition+mBufferSize;
}

void HgScrollBufferManager::removeItems(int start, int end, int totalCount)
{
    int oldTotalCount = mTotalCount;
    mTotalCount = totalCount;

    if (isInsideBuffer(start, end)) {
        if (mTotalCount > mBufferSize && mBufferPosition+mBufferSize == oldTotalCount) {
            // We are at the end of items, move buffer
            int oldBufferPos = mBufferPosition;
            mBufferPosition = qMax(0, totalCount-mBufferSize);

            if (start < oldBufferPos) { // Removed items are partially inside buffer
                emit requestItems(mBufferPosition, start-1);
            }
            else {
                emit requestItems(mBufferPosition, oldBufferPos-1);
            }
        }
        else {
            int first = qBound(mBufferPosition, start, mBufferPosition+mBufferSize-1);
            int last = qBound(mBufferPosition, end, mBufferPosition+mBufferSize-1);

            // Requested from the end
            emit requestItems(mBufferPosition+mBufferSize-(last-first+1),
                              qMin(mBufferPosition+mBufferSize-1, mTotalCount));
        }
    }
}

void HgScrollBufferManager::addItems(int start, int end, int totalCount)
{
    int oldTotalCount = mTotalCount;
    mTotalCount = totalCount;

    if (isInsideBuffer(start, end)) {
        int first = start;
        int last = end;

        if (mTotalCount > mBufferSize && mBufferPosition+mBufferSize == oldTotalCount) {
            // We are at the end of items, keep it that way
            int oldBufferPos = mBufferPosition;
            mBufferPosition = qMin(mBufferPosition+(end-start+1), totalCount-mBufferSize);

            if (oldBufferPos < mBufferPosition) {
                // Release from the beginning
                emit releaseItems(oldBufferPos, mBufferPosition-1);
            }

            // Added items may fall outside the buffer as the buffer is moved
            if (isInsideBuffer(start, end)) {
                first = qBound(mBufferPosition, start, mBufferPosition+mBufferSize-1);
                last = qBound(mBufferPosition, end, mBufferPosition+mBufferSize-1);
                emit requestItems(first, last);
            }
        }
        else {
            first = qBound(mBufferPosition, start, mBufferPosition+mBufferSize-1);
            last = qBound(mBufferPosition, end, mBufferPosition+mBufferSize-1);

            if (mTotalCount > mBufferSize) {
                // Release from the end
                emit releaseItems(mBufferPosition+mBufferSize,
                                  mBufferPosition+mBufferSize+(last-first+1)-1);
            }
            // If all the items fit in the buffer no need to release items

            emit requestItems(first, last);
        }
    }
}

void HgScrollBufferManager::moveItems(int start, int end, int target, int totalCount)
{
    if (isInsideBuffer(start) && isInsideBuffer(end) && isInsideBuffer(target)) {
        return;
    }

    if (!isInsideBuffer(start, end) && !isInsideBuffer(target)) {
        return;
    }

    if (!isInsideBuffer(target)) {
        if (isInsideBuffer(start) && isInsideBuffer(end)) {
            if (mBufferPosition+mBufferSize == mTotalCount) {
                // Fetch from beginning
                emit requestItems(mBufferPosition, mBufferPosition+end-start);
            }
            else {
                // Fetch from end
                emit requestItems(mBufferPosition+mBufferSize-(end-start+1),
                                  qMin(mBufferPosition+mBufferSize-1, mTotalCount));
            }
        }
        else if (isInsideBuffer(start) && end >= mBufferPosition+mBufferSize-1) {
            emit requestItems(start, mBufferPosition+mBufferSize-1);
        }
        else if (start <= mBufferPosition && isInsideBuffer(end)) {
            emit requestItems(mBufferPosition, end);
        }
        else {
            emit requestItems(mBufferPosition, mBufferPosition+mBufferSize-1);
        }
    }

    if (isInsideBuffer(target)) {
        // start-end may be partially inside buffer
        if (!isInsideBuffer(start, end)) {
            addItems(target, target+end-start, totalCount);
        }
        else if (isInsideBuffer(start)) {
            addItems(target+(mBufferPosition+mBufferSize-start), target+end-start, totalCount);
        }
        else { // end is inside buffer
            addItems(target, target+mBufferPosition-start-1, totalCount);
        }
    }
}

bool HgScrollBufferManager::isInsideBuffer(int pos)
{
    return (pos >= mBufferPosition && pos < mBufferPosition+mBufferSize);
}

bool HgScrollBufferManager::isInsideBuffer(int start, int end)
{
    INFO("Buffer:" << mBufferPosition << "-" << mBufferPosition+mBufferSize-1);
    INFO("Change:" << start << "-" << end);

    if (isInsideBuffer(start)) {
        return true;
    }
    if (isInsideBuffer(end)) {
        return true;
    }
    if (start < mBufferPosition && end >= mBufferPosition+mBufferSize) {
        return true;
    }

    INFO("Buffer not affected");
    return false;
}

