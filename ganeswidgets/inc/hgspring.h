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

#ifndef HGSPRING_H
#define HGSPRING_H

#include <QPoint>
#include <QObject>
#include <QTime>

class QTimer;

/**
 * Spring physics class.
 */
class HgSpring : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal mDamping READ damping WRITE setDamping)
    Q_PROPERTY(qreal mK READ k WRITE setK)
    
public:

    explicit HgSpring();    
    virtual ~HgSpring();
    
    qreal k() const;
    qreal damping() const;
    void setK(qreal K);
    void setDamping(qreal damping);
    
    void animateToPos(const QPointF& pos);
    void animateToPosAfterPanning(const QPointF& pos, qreal worldWidth);
    void gotoPos(const QPointF& pos);
    void cancel();
    bool isActive() const;
    void resetVelocity();

    const QPointF& startPos() const;
    const QPointF& pos() const;
    const QPointF& endPos() const;    
    const QPointF& velocity() const;
    
    bool updatePositionIfNeeded();
    
signals:
    void updated();
    void started();
    void ended();
private slots:
    void update();
private:
    Q_DISABLE_COPY(HgSpring)

    QPointF mStartPos;
    QPointF mPos;
    QPointF mEndPos;
    QPointF mEndPosOverListBoundary;
    QPointF mVelocity;
    qreal mK;
    qreal mDamping;
    int mAccumulator;
    QTimer* mTimer;
    QTime mPrevTime;
    bool mDoNotUpdate;
    
    bool mEndPosOverListEdge;
    qreal mWorldWidth;
};

#endif
