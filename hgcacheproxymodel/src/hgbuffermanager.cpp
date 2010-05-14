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
*  Version     : %version: 1 %
*/
#include "hgbuffermanager.h"
#include <hgwidgets/hgcacheproxymodel.h>


HgBufferManager::HgBufferManager(
    HgBufferManagerObserver* aObserver,
	int aBufferSize,
	int aBufferTreshold,
	int aInitialPosition,
	int aTotalCount )
:
mObserver(aObserver),
mBufferSize( aBufferSize ),
mBufferTreshold( aBufferTreshold ),
mBufferPosition( aInitialPosition ),
mTotalCount( aTotalCount )
{
    ASSERT( mObserver != 0 );
    mBufferPosition -= (mBufferSize / 2);
    
    if( mBufferPosition + mBufferSize > mTotalCount - 1 ){
        mBufferPosition = (mTotalCount - 1) - mBufferSize;
    }
    
    if(mBufferPosition < 0 ){
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
    
    if (newSize!=mBufferSize){
//        int pos = mBufferPosition + (mBufferSize / 2);
        
        int a = Max(0, mBufferPosition + mBufferSize/2 - newSize/2);
        int b = Min(a + newSize, mTotalCount);
        if ( b == mTotalCount){
            a = mTotalCount - newSize;
        }
        
        int c = Max(0, mBufferPosition);
        int d = Min(c + mBufferSize, mTotalCount);
        if ( d == mTotalCount){
            c = mTotalCount - mBufferSize;
        }
        
        if ( newSize>mBufferSize){
            mObserver->request(a, c-1, HgCacheProxyModel::HgRequestOrderAscending);
            mObserver->request(d, b-1, HgCacheProxyModel::HgRequestOrderAscending);
        }else if ( newSize<mBufferSize){
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
    
    if(mResetOrdered){
        mResetOrdered = false;
    } else {
        if(mDiff < 0){
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
    int end = mReleaseStart + mReleaseCount < mTotalCount ?
        mReleaseStart + mReleaseCount: mTotalCount; 
    end--;
    if(end >= mReleaseStart ){
        mObserver->release(mReleaseStart, end);
    }
    
    mReleaseCount = 0;
    
    // Request
    end = mRequestStart + mRequestCount < mTotalCount ? 
        mRequestStart + mRequestCount : mTotalCount;
    
    end--;
    if(end >= mRequestStart ){
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
    if(mTotalCount <= mBufferSize)
        return;
	
	bool forceUpdate = false;
    aIndex -= mBufferSize / 2; // normalize index to Buffer start
    
    if(aIndex < 0){
        aIndex = 0;
        forceUpdate = true;
    }else if( aIndex > mTotalCount - mBufferSize ){
        aIndex = mTotalCount - mBufferSize;
        forceUpdate = true;
    }

    mDiff = mBufferPosition - aIndex; 

    // Too large change reset whole buffer
    if( mDiff >= mBufferSize || -mDiff >= mBufferSize || mResetOrdered ) {
        resetBuffer(aIndex + (mBufferSize/2), mTotalCount);
    } else if( mDiff >= mBufferTreshold ) { // Move Up
        mRequestCount = mDiff;
        mReleaseCount = mDiff;
        calculate();
    } else if ( -mDiff >= mBufferTreshold ) {// Move Down
        mRequestCount = -mDiff;
        mReleaseCount = -mDiff;
        calculate();
    } else if( forceUpdate && mDiff ) { // Top or bottom has been reached
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
void HgBufferManager::resetBuffer( int aPosition, int aTotalCount)
{
    if( !mResetOrdered ){
        // release Old buffer
        mReleaseStart = mBufferPosition;
        mReleaseCount = mBufferSize;
    }
    
    // set position and count
    mBufferPosition = aPosition - (mBufferSize / 2);
    mTotalCount = aTotalCount;
    mDiff = 0;
    
    if( mBufferPosition + mBufferSize > mTotalCount - 1 ){
        mBufferPosition = mTotalCount - mBufferSize;
    }
    
    if(mBufferPosition < 0 ){
        mBufferPosition = 0;
    }
    
    //request new Buffer
    mRequestStart = mBufferPosition;
    mRequestCount = mBufferSize;
    mResetOrdered = true;
    calculate();
}

void HgBufferManager::itemCountChanged( int aIndex, 
                                      bool aRemoved,
                                      int aNewTotalCount )
{    
    Q_UNUSED(aIndex);    
    Q_UNUSED(aRemoved);
    //release all, to make sure that no old items are skipped
    mObserver->release(0, aNewTotalCount);
    resetBuffer(mBufferPosition + (mBufferSize / 2), aNewTotalCount);
}
//eof