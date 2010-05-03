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

#ifndef HGVGQUADRENDERER_H
#define HGVGQUADRENDERER_H

#include "hgquadrenderer.h"
#include <qlist>
#include <VG/openvg.h>
#include <qimage>

class QPoint;
class QPainter;
class QRectF;
class QMatrix4x4;
class HgVgQuad;
class HgVgImage;
class HgImageFader;
class QSize;
/**
 * OpenVG implementation of the HgQuadRenderer.
 */
class HgVgQuadRenderer: public HgQuadRenderer
{
public:
    HgVgQuadRenderer(int maxQuads);
    virtual ~HgVgQuadRenderer();    
    virtual HgQuad* getQuadAt(const QPointF& point) const;
    virtual void transformQuads(const QMatrix4x4& view, const QMatrix4x4& proj, 
        const QRectF& rect);
    virtual void drawQuads(const QRectF& rect, QPainter* painter);
    virtual bool getQuadTranformedPoints(QPolygonF& points, int index) const;
    virtual HgImage* createNativeImage();
    virtual QList<HgQuad*> getVisibleQuads(const QRectF& rect) const;
    virtual void setDefaultImage(QImage defaultImage);
    
    // new functions
    HgVgImage* defaultImage();
    QSize imageSize(bool mirror=false) const;
    HgImageFader* imageFader();
    
private:
    
    QList<HgVgQuad*> mTransformedQuads;
    
    QList<HgVgQuad*> mSortedQuads;

    HgVgImage* mDefaultVgImage;

};

#endif
