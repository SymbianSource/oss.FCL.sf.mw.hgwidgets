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

#include <qpoint>
#include <qobject>
#include <qtime>

class QTimer;

/**
 * Spring physics class.
 */
class HgSpring : public QObject
{
    Q_OBJECT
public:

    explicit HgSpring();    
    virtual ~HgSpring();
    
    void setK(qreal K);
    void setDamping(qreal damping);
    
    void animateToPos(const QPointF& pos);
    void gotoPos(const QPointF& pos);
    void cancel();
    bool isActive() const;

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
    QPointF mVelocity;
    qreal mK;
    qreal mDamping;
    int mAccumulator;
    QTimer* mTimer;
    QTime mPrevTime;
    bool mDoNotUpdate;
};

#endif
