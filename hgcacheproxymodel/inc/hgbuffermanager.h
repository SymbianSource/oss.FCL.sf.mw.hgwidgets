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
*  Version     : %version: 3 %
*/
#ifndef HGBUFFERMANAGER_H_
#define HGBUFFERMANAGER_H_

class HgBufferManagerObserver;

class HgBufferManager
{
public:    
    
    HgBufferManager(
                    HgBufferManagerObserver* aObserver,
                    int aBufferSize,
                    int aBufferTreshold,
                    int aInitialPosition,
                    int aTotalCount );
	~HgBufferManager();

	void setPosition( int aIndex );
	void resetBuffer( int aPosition, int aTotalCount);
	void resizeCache(int newSize, int newTreshold);
	
    void aboutToRemoveItem(int pos);
    void removedItem(int pos);
    void aboutToInsertItem(int pos);
    void insertedItem(int pos);
	bool isCached(int idx) const;
	
private:
	void calculate();

private:
	HgBufferManagerObserver* mObserver; 
    
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

#endif // HGBUFFERMANAGER_H_
