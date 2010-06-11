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

#include "hgtransformedquadrenderer.h"
#include <QList>
#include <VG/openvg.h>
#include <QImage>
#include <QTransform>

class QPoint;
class QPainter;
class QRectF;
class QMatrix4x4;
class HgVgQuad;
class HgVgImage;
class QSize;
class HgTransformedQuad;
/**
 * OpenVG implementation of the HgQuadRenderer.
 */
class HgVgQuadRenderer: public HgTransformedQuadRenderer
{
public:
    HgVgQuadRenderer(int maxQuads);
    virtual ~HgVgQuadRenderer();    
        
    // From HgQuadRenderer
    virtual void drawQuads(QPainter* painter, const QRectF& rect, 
        const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix,
        Qt::Orientation orientation, 
        const QTransform& sceneTransform);    
    virtual HgImage* createNativeImage();
    virtual void setDefaultImage(QImage defaultImage);

    // new functions
    HgVgImage* defaultImage();
    
protected:

    // From HgTransformedQuadRenderer
    virtual HgTransformedQuad* createNativeQuad();

private:
    
    QTransform qtToVgTransform(QPainter* painter, const QTransform& sceneTransform, 
        const QRectF& rect, Qt::Orientation orientation) const;

private:    
    HgVgImage* mDefaultVgImage;    
};

#endif
