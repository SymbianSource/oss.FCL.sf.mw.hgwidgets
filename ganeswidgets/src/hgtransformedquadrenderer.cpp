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

#include "hgtransformedquadrenderer.h"
#include "hgquad.h"
#include "hgtransformedquad.h"
#include "trace.h"
#include "hgimage.h"

#include <QVector2D>
#include <QPolygon>
#include <QMatrix4x4>
#include <QPainter>


HgTransformedQuadRenderer::HgTransformedQuadRenderer(int maxQuads) : 
    HgQuadRenderer(maxQuads)
{
}

HgTransformedQuadRenderer::~HgTransformedQuadRenderer()
{
    qDeleteAll(mTransformedQuads.begin(), mTransformedQuads.end());
}

void HgTransformedQuadRenderer::init(int maxQuads)
{
    for (int i = 0; i < maxQuads; i++)
    {
        mTransformedQuads.append(createNativeQuad());
    }    
}

HgQuad* HgTransformedQuadRenderer::getQuadAt(const QPointF& point) const
{
    QList<HgTransformedQuad*>::const_iterator i = mSortedQuads.begin();
    while(i != mSortedQuads.end())
    {
        HgTransformedQuad* q = (*i);
        if (q->isPointInside(point))
        {
            return q->quad();
        }
        i++;
    }
    
    return NULL;
}


void HgTransformedQuadRenderer::transformQuads(const QMatrix4x4& view, const QMatrix4x4& proj, 
    const QPointF& center, const QSizeF& windowSize)
{
    QMatrix4x4 pv = proj * view;
    
    mSortedQuads.clear();
    
    for (int i = 0; i < mQuads.size(); i++)
    {
        HgQuad* q = mQuads[i];
            
        HgTransformedQuad* tq = mTransformedQuads[i];
        
        if (q->visible())
        {
            tq->transformQuad(i, pv, q, mMirroringPlaneY, mTranslation, center, windowSize);   
            mSortedQuads.append(tq);
        }
    }
        
    qSort(mSortedQuads.begin(), mSortedQuads.end(), HgTransformedQuad::quadSorter);
}

bool HgTransformedQuadRenderer::getQuadTranformedPointsByUserData(QPolygonF& points, const QVariant& userData) const
{    
    for (int i = 0; i < mSortedQuads.count(); i++)
    {
        HgTransformedQuad* quad = mSortedQuads[i];
        if (quad->quad())
        {
            if (quad->quad()->userData() == userData)
            {
                quad->getTransformedPoints(points);
                return true;                
            }                
        }
    }
    
    return false;
}

QList<HgQuad*> HgTransformedQuadRenderer::getVisibleQuads(const QRectF& rect) const
{
    // this implementation isn't 100% precise
    QList<HgQuad*> result;
    for (int i = 0; i < mSortedQuads.count(); i++) {
       QPolygonF poly;
       mSortedQuads[i]->getTransformedPoints(poly);
       QRectF bounds = poly.boundingRect();       
       if (bounds.intersects(rect) || rect.contains(bounds)) {
           result.append(mSortedQuads[i]->quad());
       }           
    }
    
    return result;    
}

void HgTransformedQuadRenderer::drawTransformedQuads(QPainter* painter, const QRectF& rect, const QTransform& transform)
{
    // draw quads    
    for (int i = 0; i < mSortedQuads.size(); i++)
    {
        mSortedQuads[i]->draw(painter, rect, transform);
    }    
}
