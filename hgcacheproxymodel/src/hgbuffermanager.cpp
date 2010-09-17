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
*  Version     : %version: 10 %
*/
#include "hgbuffermanager.h"
#include "hglogger.h"
#include <hgwidgets/hgcacheproxymodel.h>


HgBufferManager::HgBufferManager(
    HgBufferManagerObserver* aObserver,
	int aBufferSize,
	int aBufferTreshold,
	int aInitialPosition,
	int aTotalCount)
:
mObserver(aObserver),
mBufferSize(aBufferSize),
mBufferTreshold(aBufferTreshold),
mBufferPosition(aInitialPosition),
mDiff(0),
mTotalCount(aTotalCount),
mResetOrdered(false),
mRequestStart(0),
mRequestCount(0),
mReleaseStart(0),
mReleaseCount(0)
{
    ASSERT(mObserver != 0);
    mBufferPosition -= (mBufferSize/2);
    
    if (mBufferPosition + mBufferSize > mTotalCount - 1 ) {
        mBufferPosition = (mTotalCount - 1) - mBufferSize;
    }
    
    if (mBufferPosition < 0 ) {
        mBufferPosition = 0;
    }
    
    mDiff = 0;
	//request Initial Buffer
	mRequestStart = mBufferPosition;
	mRequestCount = mBufferSize;

	calculate();
}


HgBufferManager::~HgBufferManager()
{
}

void HgBufferManager::resizeCache(int newSize, int newTreshold)
{
    if (newTreshold != mBufferTreshold){
        mBufferTreshold = newTreshold;
    }
    
    if (newSize!=mBufferSize) {
        int a = Max(0, mBufferPosition + mBufferSize/2 - newSize/2);
        int b = Min(a + newSize, mTotalCount);
        if (b == mTotalCount) {
            a = mTotalCount - newSize;
        }
        
        int c = Max(0, mBufferPosition);
        int d = Min(c + mBufferSize, mTotalCount);
        if (d == mTotalCount) {
            c = mTotalCount - mBufferSize;
        }
        
        if (newSize>mBufferSize) {
            mObserver->request(a, c-1, HgCacheProxyModel::HgRequestOrderAscending);
            mObserver->request(d, b-1, HgCacheProxyModel::HgRequestOrderAscending);
        }else if (newSize<mBufferSize) {
            mObserver->release(c, a-1);
            mObserver->release(b, d);
        }
        mBufferPosition = a;
        mBufferSize = newSize;
    }
}

void HgBufferManager::calculate()
{  
    HgCacheProxyModel::HgRequestOrder direction = HgCacheProxyModel::HgRequestOrderAscending;
    
    if (mResetOrdered) {
        mResetOrdered = false;
    } else {
        if (mDiff < 0) {
            mReleaseStart = mBufferPosition;
            mRequestStart = mBufferPosition + mBufferSize;
            direction = HgCacheProxyModel::HgRequestOrderAscending; 
        } else if( mDiff > 0) {
            mReleaseStart = mBufferPosition + mBufferSize - mDiff;
            mRequestStart = mBufferPosition - mDiff;
            direction = HgCacheProxyModel::HgRequestOrderDescending;
        }
    }
    
    // Release 
    int end = (mReleaseStart + mReleaseCount < mTotalCount)?
        (mReleaseStart + mReleaseCount): mTotalCount; 
    end--;
    if (end >= mReleaseStart) {
        mObserver->release(mReleaseStart, end);
    }
    
    mReleaseCount = 0;
    
    // Request
    end = (mRequestStart + mRequestCount < mTotalCount)? 
        (mRequestStart + mRequestCount): mTotalCount;
    
    end--;
    if (end >= mRequestStart) {
        mObserver->request(mRequestStart, end, direction);
	}
      
    mRequestCount = 0;
    
    // Move Buffer
    mBufferPosition -= mDiff;
    // Reset Diff
    mDiff = 0;
}

// -----------------------------------------------------------------------------
// BufferManager::SetPosition()
// -----------------------------------------------------------------------------
//
void HgBufferManager::setPosition( int aIndex )
{
    // If all the items fit in the buffer no need to move the buffer
    if (mTotalCount <= mBufferSize)
        return;
	
	bool forceUpdate = false;
	int idx = aIndex - mBufferSize / 2; // normalize index to Buffer start
    
    if (idx < 0) {
        idx = 0;
        forceUpdate = true;
    }else if (idx > mTotalCount - mBufferSize) {
        idx = mTotalCount - mBufferSize;
        forceUpdate = true;
    }

    mDiff = mBufferPosition - idx; 

    // Too large change reset whole buffer
    if (mDiff >= mBufferSize || -mDiff >= mBufferSize || mResetOrdered) {
        resetBuffer(aIndex, mTotalCount);
    } else if (mDiff >= mBufferTreshold) { // Move Up
        mRequestCount = mDiff;
        mReleaseCount = mDiff;
        calculate();
    } else if (-mDiff >= mBufferTreshold) {// Move Down
        mRequestCount = -mDiff;
        mReleaseCount = -mDiff;
        calculate();
    } else if (forceUpdate && mDiff) { // Top or bottom has been reached
        int diff = mDiff < 0 ? -mDiff : mDiff;
        mRequestCount = diff;
        mReleaseCount = diff;
        calculate();
    }
}

// -----------------------------------------------------------------------------
// BufferManager::ResetBuffer()
// -----------------------------------------------------------------------------
//
void HgBufferManager::resetBuffer(int aPosition, int aTotalCount)
{
    int oldPos = mBufferPosition;
    if (!mResetOrdered) {
        // release Old buffer
        mReleaseStart = mBufferPosition;
        mReleaseCount = mBufferSize;
    }

    mTotalCount = aTotalCount;
    mDiff = 0;
    
    // set position and count
    mBufferPosition = aPosition - (mBufferSize / 2);

    if (aPosition < 0) {
        aPosition = 0;
    } else if (aPosition >= mTotalCount) {
        aPosition = mTotalCount - 1;    
    }
    
    if (mBufferPosition + mBufferSize > mTotalCount - 1) {
        mBufferPosition = mTotalCount - mBufferSize;
    }
    if (mBufferPosition < 0) {
        mBufferPosition = 0;
    }
    
    mObserver->release(0, mTotalCount);
    
//                  size      size       size   
//  -------------|---------|---------|---------|------------------
//             begin    middle1   middle2     end                          
    int size = mBufferSize/3;
    int begin = mBufferPosition;
    int middle1 = begin + size;
    int middle2 = middle1 + size;
    int end = mBufferPosition + mBufferSize -1;  //Can not be middle2 + size, mBufferSize/3 can be not equal size
    
    TX_LOG_ARGS(QString("aPosition:%0 begin:%1 middle1:%2 c:%3 end:%4").arg(aPosition).arg(begin).arg(middle1).arg(c).arg(end) );
    
    if (aPosition >=begin && aPosition < middle1) { //aPosition is in begining, let's load from top
        mObserver->request(begin, end, HgBufferManagerObserver::HgRequestOrderAscending);
    } else if (aPosition >= middle1 && aPosition < middle2) {//aPosition is in the middle, let's load from middle
        HgBufferManagerObserver::HgRequestOrder order = HgBufferManagerObserver::HgRequestOrderAscending;
        if (oldPos > mBufferPosition) {
            order = HgBufferManagerObserver::HgRequestOrderDescending;
        }
        mObserver->request(middle1, middle2, order);
        if (order == HgBufferManagerObserver::HgRequestOrderAscending) {
            mObserver->request(middle2, end, order);
            mObserver->request(begin, middle1 -1, order);
        } else {
            mObserver->request(begin, middle1 -1, order);
            mObserver->request(middle2, end, order);
        }
    } else if (aPosition >= middle2 && aPosition <= end) { //aPosition is in end, let's load from bottom
        mObserver->request(begin, end, HgBufferManagerObserver::HgRequestOrderDescending);    
    }
    
    mDiff = 0;
    mResetOrdered = false;
    mRequestStart = 0;
    mRequestCount = 0;
    mReleaseStart = 0;
    mReleaseCount = 0;
    
}

void HgBufferManager::aboutToRemoveItem(int pos)
{
    if (pos < 0 || pos >= mTotalCount) {
        return;
    }
    
    if (pos >= mBufferPosition && pos < mBufferPosition + mBufferSize) {
        mObserver->release(pos, pos);
    }
}

void HgBufferManager::removedItem(int pos)
{
    if (pos < 0 || pos >= mTotalCount) {
        return;
    }
    
    mTotalCount--;
    if (mTotalCount >= mBufferSize) {
        if (pos < mBufferPosition) { //before buffer pos is >=0
            mBufferPosition--;
        } else if (pos >= mBufferPosition && pos < mBufferPosition + mBufferSize) {
            if (mBufferPosition + mBufferSize <= mTotalCount) {
                // Requested from the end
                mObserver->request(mBufferPosition + mBufferSize - 1,
                                   mBufferPosition + mBufferSize - 1);
            }else if (mBufferPosition > 0) {
                // Move buffer and request from the beginning 
                mBufferPosition--;
                mObserver->request(mBufferPosition, mBufferPosition);
            }
        }
    }
}

void HgBufferManager::aboutToInsertItem(int pos)
{
    if (pos < 0 || pos > mTotalCount) {
        return;
    }

    if (pos >= mBufferPosition && pos < mBufferPosition + mBufferSize) {
        if (mBufferPosition + mBufferSize < mTotalCount) {
            // Release from the end of the buffer
            mObserver->release(mBufferPosition + mBufferSize - 1, mBufferPosition + mBufferSize - 1);
        }
    }
}

void HgBufferManager::insertedItem(int pos)
{
    if ( pos < 0 || pos > mTotalCount) {
        return;
    }

    mTotalCount++;
    if (pos >= mBufferPosition && pos < mBufferPosition + mBufferSize) {
        mObserver->request(pos, pos);
    }else if (pos<mBufferPosition) { //if we have inserted item before buffer, we should move buffer.
        mBufferPosition++;
    }
}

//eof
