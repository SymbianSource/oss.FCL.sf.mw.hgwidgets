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

#ifndef HGTRANSFORMEDQUADRENDERER_H
#define HGTRANSFORMEDQUADRENDERER_H

#include "hgquadrenderer.h"
#include <qlist>
#include <qimage>

class QPoint;
class QPainter;
class QRectF;
class QMatrix4x4;
class QSize;
class HgTransformedQuad;

/**
 * Abstract base class for quad renderers, that need to tranform quads manually. (using CPU)
 */
class HgTransformedQuadRenderer: public HgQuadRenderer
{
public:
    
    HgTransformedQuadRenderer(int maxQuads);
    virtual ~HgTransformedQuadRenderer();    
    
    virtual HgQuad* getQuadAt(const QPointF& point) const;
    virtual bool getQuadTranformedPointsByUserData(QPolygonF& points, const QVariant& userData) const;
    virtual QList<HgQuad*> getVisibleQuads(const QRectF& rect) const;
            
protected:

    // this must be called from inheriting class
    void init(int maxQuads);
    
    virtual void transformQuads(const QMatrix4x4& view, const QMatrix4x4& proj, 
        const QPointF& center, const QSizeF& windowSize);

    virtual void drawTransformedQuads(QPainter* painter, const QRectF& rect);
    
    virtual HgTransformedQuad* createNativeQuad()=0;
        
private:
    QList<HgTransformedQuad*> mTransformedQuads;    
    QList<HgTransformedQuad*> mSortedQuads;    
};

#endif
