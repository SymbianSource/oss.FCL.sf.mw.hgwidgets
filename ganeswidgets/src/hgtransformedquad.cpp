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

#include "hgtransformedquad.h"
#include "hgquad.h"
#include "trace.h"
#include <qvector2d>
#include <qpolygon>
#include <qmatrix4x4>
#include <qpainter>

    HgTransformedQuad::HgTransformedQuad(qreal yDir) : mYDir(yDir)
    {
        
    }

    HgTransformedQuad::~HgTransformedQuad()
    {
        
    }

    int HgTransformedQuad::index() const
        {
        return mIndex;
        }

    bool HgTransformedQuad::isPointInside(const QPointF& point) const
    {    
        QPolygonF poly;
        getTransformedPoints(poly);
        QRectF rect = poly.boundingRect();
        if (rect.contains(point))
        {
            return true;
        }
        return false;
    }

    void HgTransformedQuad::transformQuad(int index, const QMatrix4x4& projView, HgQuad* quad, 
        qreal mirroringPlaneY, const QVector2D& translate, const QPointF& center, 
        const QSizeF& windowSize)
    {
        mIndex = index;
        mQuad = quad;
            
        QMatrix4x4 tm;
        tm.setToIdentity();
        tm.rotate(quad->outerRotation());

        if (mQuad->mirrorImageEnabled())
        {
            computeMirroredPoints(tm, projView, mirroringPlaneY, translate, center, windowSize);
        }
        
        tm.translate(quad->position());    
        tm.rotate(quad->rotation());    
        tm.scale(quad->scale().x(), quad->scale().y());

        tm = projView * tm;         
        
        perspectiveTransformPoints(mTransformedPoints, tm, center, windowSize);
        
        for (int i = 0; i < 4; i++)
            mTransformedPoints[i] += translate;
        
    }
    
    void HgTransformedQuad::getTransformedPoints(QPolygonF& poly) const
    {
        poly.clear();
        poly.append(mTransformedPoints[0].toPointF());
        poly.append(mTransformedPoints[1].toPointF());
        poly.append(mTransformedPoints[2].toPointF());
        poly.append(mTransformedPoints[3].toPointF());    
    }

    bool HgTransformedQuad::quadSorter(HgTransformedQuad* a, HgTransformedQuad* b)
    {
        return a->mQuad->position().z() < b->mQuad->position().z();
    }

    HgQuad* HgTransformedQuad::quad() const
    {
        return mQuad;
    }
    
    void HgTransformedQuad::computeMirroredPoints(const QMatrix4x4& trans, const QMatrix4x4& projView, 
        qreal mirroringPlaneY, const QVector2D& translate, const QPointF& center, 
        const QSizeF& windowSize)
    {
        HgQuad* quad = mQuad;

        QMatrix4x4 mirror = trans;

        qreal distToPlane = qAbs(quad->position().y() - mirroringPlaneY);
        
        mirror.translate(quad->position().x(), mirroringPlaneY - distToPlane/1, quad->position().z());
        mirror.scale(quad->scale().x(), -quad->scale().y()/1);
        mirror.rotate(quad->rotation());
            
        QMatrix4x4 modelViewProjMatrix = projView * mirror;
        
        perspectiveTransformPoints(mMirroredPoints, modelViewProjMatrix, center, windowSize);    
        
        for (int i = 0; i < 4; i++)
            mMirroredPoints[i] += translate;

    }

    void HgTransformedQuad::perspectiveTransformPoints(QVector2D* outPoints, const QMatrix4x4& matrix, 
        const QPointF& center, const QSizeF& windowSize)
    {
        const QVector4D points[] = 
        {
            QVector4D(-0.5f, -0.5f, 0.0f, 1.0f),
            QVector4D( 0.5f, -0.5f, 0.0f, 1.0f),
            QVector4D( 0.5f,  0.5f, 0.0f, 1.0f),
            QVector4D(-0.5f,  0.5f, 0.0f, 1.0f)
        };

        qreal hw = windowSize.width() * 0.5f;
        qreal hh = windowSize.height() * 0.5f;
        
        for (int i = 0; i < 4; i++)
        {
            QVector4D temp = matrix * points[i];
                                
            outPoints[i] = QVector2D(
                center.x() + temp.x() / temp.w() * hw, 
                center.y() + (temp.y() / temp.w() * hh) * mYDir);

        }
        
    }

