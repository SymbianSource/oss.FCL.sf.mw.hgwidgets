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


#ifndef HGSCROLLBUFFERMANAGER_H_
#define HGSCROLLBUFFERMANAGER_H_

#include <qobject>
#include <QTimer>

class HgScrollBufferManager: public QObject
    {
    Q_OBJECT
public:

    HgScrollBufferManager(
            int bufferSize,
            int bufferTreshold,
            int initialPosition,
            int totalCount);

    virtual ~HgScrollBufferManager();

    void resetBuffer(int aPosition, int aTotalCount);

    void scrollPositionChanged(int newPosition);

    bool positionInsideBuffer(int position);

    void removeItems(int start, int end, int totalCount);
    void addItems(int start, int end, int totalCount);
    void moveItems(int start, int end, int target, int totalCount);

    void currentBuffer(int& bufferStart, int& bufferEnd);

signals:

    void releaseItems(int releaseStart, int releaseEnd);
    void requestItems(int requestStart, int requestEnd);

protected slots:

    void timeout();

protected:

    void init();
    void asyncUpdate();
    bool isInsideBuffer(int pos);
    bool isInsideBuffer(int start, int end);

private:

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
    QTimer mTimer;

private:
    Q_DISABLE_COPY(HgScrollBufferManager)
    };

#endif /*HGSCROLLBUFFERMANAGER_H_*/
