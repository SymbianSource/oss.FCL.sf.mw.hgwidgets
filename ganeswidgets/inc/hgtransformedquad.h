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

#ifndef HGTRANSFORMEDQUAD_H
#define HGTRANSFORMEDQUAD_H

#include <QVector2D>

class HgQuad;
class QMatrix4x4;
class QPointF;
class QSizeF;
class QPolygonF;
class QPainter;

/**
 * Base class for transformed quads, to be used by renderers.
 */
class HgTransformedQuad
{
public:
    HgTransformedQuad(qreal yDir=1);
    virtual ~HgTransformedQuad();

    int index() const;
    
    bool isPointInside(const QPointF& point) const;

    void transformQuad(int index, const QMatrix4x4& projView, HgQuad* quad, 
        qreal mirroringPlaneY, const QVector2D& translate, const QPointF& center, 
        const QSizeF& windowSize);
    
    void getTransformedPoints(QPolygonF& poly) const;
    
    HgQuad* quad() const;

    virtual void draw(QPainter* painter, const QRectF& rect, const QTransform& transform)=0;

    static bool quadSorter(HgTransformedQuad* a, HgTransformedQuad* b);
    
protected:
    
    void computeMirroredPoints(const QMatrix4x4& trans, const QMatrix4x4& projView, 
        qreal mirroringPlaneY, const QVector2D& translate, const QPointF& center, 
        const QSizeF& windowSize);

    void perspectiveTransformPoints(QVector2D* outPoints, const QMatrix4x4& matrix, 
        const QPointF& center, const QSizeF& windowSize);
    
protected:
    int mIndex;
    HgQuad* mQuad;
    QVector2D mTransformedPoints[4];
    QVector2D mMirroredPoints[4];
    qreal mYDir;
};

#endif
