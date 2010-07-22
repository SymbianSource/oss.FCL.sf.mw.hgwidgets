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

#ifndef HGQTQUADRENDERER_H
#define HGQTQUADRENDERER_H

#include "hgtransformedquadrenderer.h"
#include <QList>
#include <QImage>

class QPoint;
class QPainter;
class QRectF;
class QMatrix4x4;
class HgQtImage;
class QSize;
class HgTransformedQuad;

/**
 * Qt implementation of the HgTransformedQuadRenderer.
 */
class HgQtQuadRenderer: public HgTransformedQuadRenderer
{
public:
    HgQtQuadRenderer(int maxQuads);
    virtual ~HgQtQuadRenderer();    

    // From HgQuadRenderer
    virtual void drawQuads(QPainter* painter, const QRectF& rect, 
        const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix,
        Qt::Orientation orientation, 
        const QTransform& sceneTransform);    
    virtual HgImage* createNativeImage();
    virtual void setDefaultImage(QImage defaultImage);

    // new functions
    HgQtImage* defaultImage();
    
    bool isReflection() const;
    
protected:

    // From HgTransformedQuadRenderer
    virtual HgTransformedQuad* createNativeQuad();
    
    void drawFloor(QPainter* painter, const QRectF& rect);
    
private:    
    HgQtImage* mDefaultQtImage;    
    bool mIsReflection;
};

#endif
