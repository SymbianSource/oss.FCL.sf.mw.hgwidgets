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

class UpdatePair
{
public:
    UpdatePair(int start, int count);

    int start() const;
    int end() const;

    bool adjacent(int start, int count) const;
    bool contains(const UpdatePair &other) const;
    void extend(int start, int count);
    void subtract(int start, int count);

    void shiftRight(int count);
    void shiftLeft(int count);

    bool operator== (const UpdatePair &other) const;
    bool operator< (const UpdatePair &other) const;

private:
    int mStart;
    int mCount;
};

class UpdateBuffer : public QList<UpdatePair>
{
public:
    UpdateBuffer();

    void add(int start, int count);
    void remove(int start, int count);
    void shiftRight(int startingFrom, int amount);
    void shiftLeft(int startingFrom, int amount);
};

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

    void addItems(int start, int count);
    void removeItems(int start, int end);
    void moveItems(int start, int end, int target);

    void flushRequestBuffers();

    void currentBuffer(int& bufferStart, int& bufferEnd);

signals:

    void releaseItems(int releaseStart, int releaseEnd);
    void requestItems(int requestStart, int requestEnd);

protected slots:

    void timeout();

protected:

    void init();
    void asyncUpdate();

    int changeBufferPosition(int newPos);

    void simpleAddItems(int start, int end);
    void simpleRemoveItems(int start, int end);

    void appendRequestBuffer(int start, int end);
    void appendReleaseBuffer(int start, int end);

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

    UpdateBuffer mRequestBuffer;
    UpdateBuffer mReleaseBuffer;

private:
    Q_DISABLE_COPY(HgScrollBufferManager)
    };

#endif /*HGSCROLLBUFFERMANAGER_H_*/
