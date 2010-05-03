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

void HgScrollBufferManager::addItems(int start, int end)
{
    FUNC_LOG;

    mTotalCount += (end-start+1);
    int lastBufferItem = mBufferPosition+mBufferSize-1;

    if (start < mBufferPosition) {
        simpleAddItems(start, end);
        // New items push the buffer forward, items inside the buffer do not change
    }
    // Check buffer higher limit
    else if (start <= lastBufferItem && end > lastBufferItem) {
        simpleAddItems(start, lastBufferItem);
        // Items added after the buffer are ignored
    }
    else {
        simpleAddItems(start, end);
    }
}

void HgScrollBufferManager::removeItems(int start, int end)
{
    FUNC_LOG;

    int lastBufferItem = mBufferPosition+mBufferSize-1;
    int removedItemCount = end-start+1;

    if (mTotalCount < mBufferSize) {
        // Do nothing
    }
    else if (start > lastBufferItem) {
        // Do nothing
    }
    else if (end < mBufferPosition) {
        mTotalCount = mTotalCount-removedItemCount;
        simpleRemoveItems(start, end);
    }
    else if (start < mBufferPosition && end > lastBufferItem) {
        mTotalCount = mTotalCount-removedItemCount;
        mBufferPosition = qBound(0, mBufferPosition, mTotalCount-mBufferSize);
        resetBuffer(mBufferPosition, mTotalCount);
    }
    // Check buffer higher limit
    else if (start <= lastBufferItem && end > lastBufferItem) {
        mTotalCount = mTotalCount-(end-lastBufferItem);
        simpleRemoveItems(lastBufferItem+1, end);
        mTotalCount = mTotalCount-(lastBufferItem-start+1);
        simpleRemoveItems(start, lastBufferItem);
        // Order does matter
        mTotalCount = mTotalCount-(end-lastBufferItem);
        simpleRemoveItems(lastBufferItem+1, end);
        mTotalCount = mTotalCount-(lastBufferItem-start+1);
        simpleRemoveItems(start, lastBufferItem);
    }
    // Check buffer lower limit
    else if (start < mBufferPosition && end >= mBufferPosition) {
        // Order does matter
        mTotalCount = mTotalCount-(end-mBufferPosition+1);
        simpleRemoveItems(mBufferPosition, end);
        mTotalCount = mTotalCount-(mBufferPosition-start);
        simpleRemoveItems(start, mBufferPosition-1);
    }
    else {
        mTotalCount = mTotalCount-removedItemCount;
        simpleRemoveItems(start, end);
    }
}

void HgScrollBufferManager::moveItems(int start, int end, int target)
{
    int lastBufferItem = mBufferPosition+mBufferSize-1;

    INFO("Move" << start << "-" << end << "to" << target << ",buffer:" << mBufferPosition << "-" << lastBufferItem << "total count:" << mTotalCount);

    if (mTotalCount < mBufferSize) {
        // Do nothing
    }
    else if (start < mBufferPosition && end > lastBufferItem) {
        resetBuffer(mBufferPosition, mTotalCount);
    }
    else if (start > lastBufferItem && target > lastBufferItem) {
        // Do nothing
    }
    else if (start > lastBufferItem && target < mBufferPosition) {
        simpleAddItems(start, end);
    }
    else if (end < mBufferPosition && target < mBufferPosition) {
        // Do nothing
    }
    else if (end < mBufferPosition && target > lastBufferItem) {
        simpleRemoveItems(start, end);
    }
    else if (start >= mBufferPosition && end <= lastBufferItem &&
             target >= mBufferPosition && target <= lastBufferItem) {
        // Do nothing
    }
    else {
        // Rare and complicated use cases: reset the whole buffer
        resetBuffer(mBufferPosition, mTotalCount);
    }
}

void HgScrollBufferManager::flushRequestBuffers()
{
    FUNC_LOG;

    qSort(mReleaseBuffer);
    int releaseCount = mReleaseBuffer.count();
    int lastReleased = -1;
    for (int i = 0; i < releaseCount; i++) {
        UpdatePair update = mReleaseBuffer.at(i);
        emit releaseItems(qMax(lastReleased+1, update.start()), update.end());
        lastReleased = update.end();
    }
    mReleaseBuffer.clear();

    qSort(mRequestBuffer);
    int requestCount = mRequestBuffer.count();
    int lastRequested = -1;
    for (int i = 0; i < requestCount; i++) {
        UpdatePair update = mRequestBuffer.at(i);
        emit requestItems(qMax(lastRequested+1, update.start()), update.end());
        lastRequested = update.end();
    }
    mRequestBuffer.clear();
}

int HgScrollBufferManager::changeBufferPosition(int newPos)
{
    FUNC_LOG;
    INFO("Change buffer position to" << newPos << "total count:" << mTotalCount);
    HANDLE_ERROR_BOOL((newPos >= 0));
    HANDLE_ERROR_BOOL((newPos+mBufferSize <= mTotalCount));

    int bufferShift = newPos-mBufferPosition;
    if (bufferShift > 0) {
        mRequestBuffer.shiftRight(mBufferPosition, bufferShift);
        mReleaseBuffer.shiftRight(mBufferPosition, bufferShift);
    }
    else if (bufferShift < 0) {
        mRequestBuffer.shiftLeft(mBufferPosition, -bufferShift);
        mReleaseBuffer.shiftLeft(mBufferPosition, -bufferShift);
    }
    mBufferPosition = newPos;
    return bufferShift;
}

/**
    This function manages only simple item additions: all items are either
    outside the buffer or inside it.
    Firs call prepare, then update model, then call fecth.
*/
void HgScrollBufferManager::simpleAddItems(int start, int end)
{
    FUNC_LOG;

    int lastBufferItem = mBufferPosition+mBufferSize-1;
    int numAddedItems = end-start+1; // [start, end] inclusive

    if (mTotalCount < mBufferSize) {
        appendRequestBuffer(start, numAddedItems);
    }
    else if (start > lastBufferItem) {
        // Do nothing
    }
    else if (start <= mBufferPosition) {
        changeBufferPosition(mBufferPosition+numAddedItems);
        // No need to fetch items, the indexes just change
    }
    else {
        // free from end
        appendReleaseBuffer(lastBufferItem+1-numAddedItems, numAddedItems);
        mReleaseBuffer.shiftRight(start, numAddedItems);
        mRequestBuffer.shiftRight(start, numAddedItems);
        appendRequestBuffer(start, numAddedItems);
    }
}

/**
    This function manages only simple item removals: all items are either
    outside the buffer or inside it.
    Firs call prepare, then update model, then call fecth.
*/
void HgScrollBufferManager::simpleRemoveItems(int start, int end)
{
    FUNC_LOG;

    int lastBufferItem = mBufferPosition+mBufferSize-1;
    int numRemovedItems = end-start+1; // [start, end] inclusive

    if (start > lastBufferItem) {
        // Do nothing
    }
    else if (end < mBufferPosition) {
        changeBufferPosition(qMax(0, mBufferPosition-numRemovedItems));
        // No need to fetch items, the indexes just change
    }
    else {
        if (mTotalCount < mBufferPosition+mBufferSize) {
            // Buffer is at the end of items
            int bufferShift = changeBufferPosition(qMax(0, mTotalCount-mBufferSize));
            // Fetch from beginning
            // Releasing removed items has been done outside this class
            appendRequestBuffer(mBufferPosition, qAbs(bufferShift));
        }
        else {
            // Fetch from end
            appendRequestBuffer(lastBufferItem+1-numRemovedItems, numRemovedItems);
        }
    }
}

void HgScrollBufferManager::appendRequestBuffer(int start, int count)
{
    FUNC_LOG;
    INFO("Request items" << start << ":" << count)

    mRequestBuffer.add(start, count);
    mReleaseBuffer.remove(start, count);
}

void HgScrollBufferManager::appendReleaseBuffer(int start, int count)
{
    FUNC_LOG;
    INFO("Release items" << start << ":" << count)

    mReleaseBuffer.add(start, count);
    mRequestBuffer.remove(start, count);
}

UpdatePair::UpdatePair(int start, int count) : mStart(start), mCount(count)
{
    HANDLE_ERROR_BOOL(mCount > 0);
}

int UpdatePair::start() const
{
    return mStart;
}

int UpdatePair::end() const
{
    return mStart+mCount-1;
}

bool UpdatePair::adjacent(int start, int count) const
{
    if (start+count < mStart) return false;
    if (start > mStart+mCount) return false;
    return true;
}

bool UpdatePair::contains(const UpdatePair &other) const
{
    if (other.mStart+other.mCount-1 < mStart) return false;
    if (other.mStart > mStart+mCount-1) return false;
    return true;
}

void UpdatePair::extend(int start, int count)
{
    int end = qMax(mStart+mCount, start+count);
    mStart = qMin(mStart, start);
    mCount = end-mStart;
    INFO("Pair extended to:" << mStart << ":" << mCount);
}

void UpdatePair::subtract(int start, int count)
{
    int end = qMin(mStart+mCount, start+count);
    mStart = qMax(mStart, start);
    mCount = end-mStart;
    INFO("Pair reduced to:" << mStart << ":" << mCount);
}

void UpdatePair::shiftRight(int count)
{
    mStart += count;
    INFO("Pair shifted to:" << mStart << ":" << mCount);
}

void UpdatePair::shiftLeft(int count)
{
    mStart -= count;
    HANDLE_ERROR_BOOL((mStart >= 0));
    INFO("Pair shifted to:" << mStart << ":" << mCount);
}

bool UpdatePair::operator== (const UpdatePair &other) const
{
    return mStart == other.mStart && mCount == other.mCount;
}

bool UpdatePair::operator< (const UpdatePair &other) const
{
    return (mStart < other.mStart || mStart == other.mStart && mCount < other.mCount);
}

UpdateBuffer::UpdateBuffer()
{
    FUNC_LOG;
}

void UpdateBuffer::add(int start, int count)
{
    FUNC_LOG;

    int itemCount = this->count();
    for (int i = 0; i < itemCount; i++) {
        if (at(i).contains(UpdatePair(start, count))) {
            // It is already there
            return;
        }
        if (at(i).adjacent(start, count)) {
            (*this)[i].extend(start, count);
            return;
        }
    }
    append(UpdatePair(start, count));
}

void UpdateBuffer::remove(int start, int count)
{
    FUNC_LOG;

    int itemCount = this->count();
    for (int i = itemCount-1; i >= 0; i--) {
        UpdatePair pair = at(i);
        UpdatePair comp = UpdatePair(start, count);
        if (comp.contains(pair)) {
            INFO("Removing pair" << pair.start() << "-" << pair.end());
            removeAt(i);
        }
        else if (pair.contains(comp)) {
            // Subtraction from middle is not applicable in mediawall use cases
            (*this)[i].subtract(start, count);
        }
        // Item may be present in multiple pairs.
    }
}

void UpdateBuffer::shiftRight(int startingFrom, int amount)
{
    FUNC_LOG;

    int itemCount = this->count();
    for (int i = 0; i < itemCount; i++) {
        if (at(i).start() >= startingFrom) {
            (*this)[i].shiftRight(amount);
        }
    }
}

void UpdateBuffer::shiftLeft(int startingFrom, int amount)
{
    FUNC_LOG;

    int itemCount = this->count();
    for (int i = 0; i < itemCount; i++) {
        if (at(i).start() >= startingFrom) {
            (*this)[i].shiftLeft(amount);
        }
    }
}
