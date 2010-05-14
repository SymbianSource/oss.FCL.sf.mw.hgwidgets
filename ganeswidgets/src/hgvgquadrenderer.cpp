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

#include "hgvgquadrenderer.h"
#include "hgtransformedquad.h"
#include "hgquad.h"
#include "hgvgimage.h"
#include "trace.h"
#include "HgImageFader.h"

#include <VG/openvg.h>
#include <VG/vgu.h>
#include <qvector2d>
#include <qpolygon>
#include <qmatrix4x4>
#include <qpainter>


static void matrixFromTransform(VGfloat* matrix, const QTransform& tm)
{
    matrix[0] = tm.m11();
    matrix[1] = tm.m12();
    matrix[2] = tm.m13();

    matrix[3] = tm.m21();
    matrix[4] = tm.m22();
    matrix[5] = tm.m23();

    matrix[6] = tm.m31();
    matrix[7] = tm.m32();
    matrix[8] = tm.m33();
}

class HgVgQuad : public HgTransformedQuad
{
public:
    
    HgVgQuad(HgVgQuadRenderer* renderer) : HgTransformedQuad(-1), mRenderer(renderer)
    {
        
    }

    ~HgVgQuad()
    {
        
    }

    void draw(QPainter* painter, const QRectF& rect)    
    {
        Q_UNUSED(painter)
        Q_UNUSED(rect)
        
        if (!mQuad->visible())
            return;
                
        HgVgImage* image = (HgVgImage*)mQuad->image();
                
        if (image == NULL  || image->alpha() == 0)
        {
            if (mRenderer->defaultImage()) {
                drawImage(mRenderer->defaultImage(), 1.0f);
            }
        }
        else
        {
            image->upload(mQuad->mirrorImageEnabled());
            
            if (image->image() == VG_INVALID_HANDLE)
            {
                if (mRenderer->defaultImage()) {
                    drawImage(mRenderer->defaultImage(), 1.0f);
                }
            }
            else
            {

                if ( mQuad->alpha() < 1.0f )
                {
                    if (mRenderer->defaultImage()) {
                        drawImage(mRenderer->defaultImage(), 1.0f - mQuad->alpha());            
                    }
                }
                
                drawImage(image, mQuad->alpha());
            }
        }
        
        
    }

    void drawImage(HgVgImage* image, qreal alpha)
    {
        Q_UNUSED(alpha)
            
        VGImage vgImage = image->image();
        
        vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

        VGfloat m[9];
        vgGetMatrix(m);
        
        computeWarpMatrix(mMatrix, image->width(), image->height(), mTransformedPoints, QVector2D());

        vgMultMatrix(mMatrix);
        vgDrawImage(vgImage);    
        
        vgLoadMatrix(m);

        if (mQuad->mirrorImageEnabled())
        {
            VGImage mirrorImage = image->mirrorImage();
            if (mirrorImage == VG_INVALID_HANDLE)
                return;
            
            computeWarpMatrix(mMirrorMatrix, image->mirrorImageWidth(), image->mirrorImageHeight(), mMirroredPoints, QVector2D());
            vgMultMatrix(mMirrorMatrix);
                
            vgDrawImage(mirrorImage);    
            vgLoadMatrix(m);    
        }
        
    }
    
private:

    void computeWarpMatrix(VGfloat* matrix, int pxWidth, int pxHeight, const QVector2D* points, 
        const QVector2D& translate)
    {        

        vguComputeWarpQuadToQuad(
            points[0].x(), points[0].y(), 
            points[1].x(), points[1].y(),
            points[2].x(), points[2].y(),
            points[3].x(), points[3].y(),
            0, pxHeight,
            pxWidth, pxHeight,
            pxWidth, 0,
            0, 0,
            matrix);
    
    }

    HgVgQuadRenderer* mRenderer;
    VGfloat mMatrix[9];
    VGfloat mMirrorMatrix[9];
};


HgVgQuadRenderer::HgVgQuadRenderer(int maxQuads) : 
    HgTransformedQuadRenderer(maxQuads),
    mDefaultVgImage(NULL)
{
    init(maxQuads);
}

HgVgQuadRenderer::~HgVgQuadRenderer()
{
    delete mDefaultVgImage;
}

void HgVgQuadRenderer::drawQuads(QPainter* painter, const QRectF& rect, 
    const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix,
    Qt::Orientation orientation, 
    const QTransform& sceneTransform)
{
    // start direct vg
    painter->beginNativePainting();

    // setup default vg states
    vgSeti(VG_COLOR_TRANSFORM, VG_FALSE);
    vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
    vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);

    // setup root transform
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    VGfloat toVgMatrix[9];
    matrixFromTransform(toVgMatrix, qtToVgTransform(painter, sceneTransform, rect, orientation));
    vgLoadMatrix(toVgMatrix);   

    // transform quads to screen
    transformQuads(viewMatrix, projectionMatrix, QPointF(rect.width()/2, rect.height()/2), 
        QSizeF(rect.width(), rect.height()));

    drawTransformedQuads(painter, rect);

    // end directt vg
    painter->endNativePainting();
    
}

HgImage* HgVgQuadRenderer::createNativeImage()
{    
    return new HgVgImage(this);   
}

HgTransformedQuad* HgVgQuadRenderer::createNativeQuad()
{
    return new HgVgQuad(this);
}

HgVgImage* HgVgQuadRenderer::defaultImage()
{
    if (mDefaultVgImage && mDefaultVgImage->image() == VG_INVALID_HANDLE) {
        mDefaultVgImage->upload(true);
    }

    return mDefaultVgImage;
}

void HgVgQuadRenderer::setDefaultImage(QImage defaultImage)
{
    HgQuadRenderer::setDefaultImage(defaultImage);
    delete mDefaultVgImage;
    mDefaultVgImage = 0;
    mDefaultVgImage = static_cast<HgVgImage*>(createNativeImage());
    mDefaultVgImage->setImage(mDefaultImage);
    mDefaultVgImage->upload(true);    
}

QTransform HgVgQuadRenderer::qtToVgTransform(QPainter* painter, const QTransform& sceneTransform, 
    const QRectF& rect, Qt::Orientation orientation) const
{
    QTransform t;
    if (orientation == Qt::Vertical)
    {
        t.translate(sceneTransform.dx(), painter->viewport().height() - sceneTransform.dy());
        t.scale(1, -1);    
    }
    else
    {
        // TODO: this will need reviewing later :)
        t.translate(rect.bottom(), 0);
        t.scale(-1, 1);
        t.translate(0, rect.right());
        t.rotate(-90, Qt::ZAxis);        
    }

    return t;
}
