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

#ifndef BUFFERMANAGER_H_
#define BUFFERMANAGER_H_

class AbstractDataProvider
{
public:

    enum requestsOrder {
            ascending,
            descending
    };   
    
    virtual void release(int start, int end) = 0;
    virtual void request(int start, int end, requestsOrder order) = 0;    
};


class BufferManager
{
public:    
    
	BufferManager(
	    AbstractDataProvider* aObserver,
		int aBufferSize,
		int aBufferTreshold,
		int aInitialPosition,
		int aTotalCount );

	~BufferManager();

	void setPosition( int aIndex );

	void resetBuffer( int aPosition, int aTotalCount);

	void itemCountChanged( int aIndex, bool aRemoved, int aNewTotalCount );
	void resizeCache(int newSize, int newTreshold);
	
private:
	void calculate();

private:
    AbstractDataProvider* mObserver; 
    
	int mBufferSize;
	int mBufferTreshold;
	int mBufferPosition;
	int mDiff;
	int mTotalCount;

	bool mResetOrdered;

	int mRequestStart;
	int mRequestCount;
	int mReleaseStart;
	int mReleaseCount;
};



#endif /*BUFFERMANAGER_H_*/
