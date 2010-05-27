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

#include "buffermanager.h"
#include <qglobal.h>

BufferManager::BufferManager(
    AbstractDataProvider* aObserver,
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


BufferManager::~BufferManager()
{
}

void BufferManager::resizeCache(int newSize, int newTreshold)
{
    if (newTreshold != mBufferTreshold){
        mBufferTreshold = newTreshold;
    }
    
    if (newSize!=mBufferSize){
//        int pos = mBufferPosition + (mBufferSize / 2);

        int a = qMax(0, mBufferPosition + mBufferSize/2 - newSize/2);
        int b = qMin(a + newSize, mTotalCount);
        if ( b == mTotalCount){
            a = mTotalCount - newSize;
        }
        
        int c = qMax(0, mBufferPosition);
        int d = qMin(c + mBufferSize, mTotalCount);
        if ( d == mTotalCount){
            c = mTotalCount - mBufferSize;
        }
        
        if ( newSize>mBufferSize){
            mObserver->request(a, c-1, AbstractDataProvider::ascending);
            mObserver->request(d, b-1, AbstractDataProvider::ascending);
        }else if ( newSize<mBufferSize){
            mObserver->release(c, a-1);
            mObserver->release(b, d);
        }
        mBufferPosition = a;
        mBufferSize = newSize;
    }
}

void BufferManager::calculate()
{  
    AbstractDataProvider::requestsOrder direction = AbstractDataProvider::ascending;
    
    if(mResetOrdered){
        mResetOrdered = false;
    } else {
        if(mDiff < 0){
            mReleaseStart = mBufferPosition;
            mRequestStart = mBufferPosition + mBufferSize;
            direction = AbstractDataProvider::ascending; 
        } else if( mDiff > 0) {
            mReleaseStart = mBufferPosition + mBufferSize - mDiff;
            mRequestStart = mBufferPosition - mDiff;
            direction = AbstractDataProvider::descending;
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
void BufferManager::setPosition( int aIndex )
{
    // If all the items fit in the buffer no need to move the buffer
    if(mTotalCount <= mBufferSize) return;
	
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
void BufferManager::resetBuffer( int aPosition, int aTotalCount)
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

void BufferManager::itemCountChanged( int aIndex, 
                                      bool aRemoved,
                                      int aNewTotalCount )
{
    mTotalCount = aNewTotalCount;
    AbstractDataProvider::requestsOrder direction = AbstractDataProvider::ascending;    
    if ( aIndex >= mBufferPosition && aIndex < mBufferPosition + mBufferSize ){
        // Change inside the buffer
        if( aRemoved ){
            // Release removed item ( Item deleted already from the owner )
			mObserver->release( aIndex, aIndex );
            if( mTotalCount >= mBufferSize ){
                // There are more items to keep the buffer filled
                if( mBufferPosition + mBufferSize <= mTotalCount ){
                    // Requested from the end
					mObserver->request( mBufferPosition + mBufferSize, 
                                        mBufferPosition + mBufferSize,
                                        direction );
                }else if( mBufferPosition > 0 ){
                    // Move buffer and request from the beginning 
                    mBufferPosition--;
                    mObserver->request( mBufferPosition, mBufferPosition, direction);
                }
            }
        }else{
            if( mTotalCount > mBufferSize ){
                // One item needs to be released
                if( mBufferPosition + mBufferSize < mTotalCount ){
                    // Release from the end of the buffer
					mObserver->release(mBufferPosition + mBufferSize, mBufferPosition + mBufferSize);
                }
            }
            // Request added item
            mObserver->request(aIndex, aIndex, direction);
        }
    }
}

