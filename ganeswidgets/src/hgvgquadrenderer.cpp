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


class HgVgQuad
{
public:
    HgVgQuad(HgVgQuadRenderer* renderer);
    ~HgVgQuad();
    
    int index() const;
    bool isPointInside(const QPointF& point) const;
    void transformQuad(int index, const QMatrix4x4& matrix, HgQuad* quad, 
        const QRectF& rect, qreal mirroringPlaneY);
    void draw();

    void getTransformedPoints(QPolygonF& polygon) const;
        
    void computeMirrorMatrix(const QMatrix4x4& tm, const QMatrix4x4& projView, 
        const QRectF& rect, qreal mirroringPlaneY);
    
    bool perspectiveTransformPoints(QVector2D* points, const QMatrix4x4& matrix, 
        const QRectF& rect);
    
    void computeWarpMatrix(VGfloat* matrix, int pxWidth, int pxHeight, const QVector2D* points);
    
    void drawImage(HgVgImage* image, qreal alpha);
    
    int mIndex;
    HgQuad* mQuad;
    QVector2D mTransformedPoints[4];
    VGfloat mMatrix[9];
    VGfloat mMirrorMatrix[9];
    HgVgQuadRenderer* mRenderer;
    bool mDegenerate;
private:
    HgVgQuad(const HgVgQuad&);
    HgVgQuad& operator=(const HgVgQuad&);
};

static bool quadSorter(HgVgQuad* a, HgVgQuad* b)
{
    return a->mQuad->position().z() < b->mQuad->position().z();
}

HgVgQuad::HgVgQuad(HgVgQuadRenderer* renderer) : mRenderer(renderer)
{
    
}

HgVgQuad::~HgVgQuad()
{
    
}

int HgVgQuad::index() const
    {
    return mIndex;
    }

bool HgVgQuad::isPointInside(const QPointF& point) const
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


void HgVgQuad::computeMirrorMatrix(const QMatrix4x4& trans, const QMatrix4x4& projView, 
    const QRectF& rect, qreal mirroringPlaneY)
{
    HgQuad* quad = mQuad;

    QMatrix4x4 mirror = trans;

    qreal distToPlane = qAbs(quad->position().y() - mirroringPlaneY);
    
    mirror.translate(quad->position().x(), mirroringPlaneY - distToPlane/2, quad->position().z());
    mirror.scale(quad->scale().x(), -quad->scale().y()/2);
    mirror.rotate(quad->rotation());
        
    QMatrix4x4 modelViewProjMatrix = projView * mirror;
    
    QVector2D temp[4];
    
    perspectiveTransformPoints(temp, modelViewProjMatrix, rect);
    
    HgVgImage* image = (HgVgImage*)mQuad->image();
    
    if (image == NULL)
    {
        image = mRenderer->defaultImage();
    }
    
    int pxWidth = image->mirrorImageWidth();
    int pxHeight = image->mirrorImageHeight();

    computeWarpMatrix(mMirrorMatrix, pxWidth, pxHeight, temp);
}

void HgVgQuad::transformQuad(int index, const QMatrix4x4& projView, HgQuad* quad, 
    const QRectF& rect, qreal mirroringPlaneY)
{
    mIndex = index;
    mQuad = quad;
        
    QMatrix4x4 tm;
    tm.setToIdentity();
    tm.rotate(quad->outerRotation());

    if (mQuad->mirrorImageEnabled())
    {
        computeMirrorMatrix(tm, projView, rect, mirroringPlaneY);
    }
    
    tm.translate(quad->position());    
    tm.rotate(quad->rotation());    
    tm.scale(quad->scale().x(), quad->scale().y());

    tm = projView * tm;         
    //QMatrix4x4 tmt = tm.transposed();
    
    mDegenerate = false;
    if (!perspectiveTransformPoints(mTransformedPoints, tm, rect))
    {
        mDegenerate = true;
    }

    HgVgImage* image = (HgVgImage*)mQuad->image();
    
    if (image == NULL)
    {
        image = mRenderer->defaultImage();
    }
    
    int pxWidth = image->width();
    int pxHeight = image->height();
    
    computeWarpMatrix(mMatrix, pxWidth, pxHeight, mTransformedPoints);
    
}

bool HgVgQuad::perspectiveTransformPoints(QVector2D* outPoints, const QMatrix4x4& matrix, 
    const QRectF& rect)
{
    const QVector4D points[] = 
    {
        QVector4D(-0.5f, -0.5f, 0.0f, 1.0f),
        QVector4D( 0.5f, -0.5f, 0.0f, 1.0f),
        QVector4D( 0.5f,  0.5f, 0.0f, 1.0f),
        QVector4D(-0.5f,  0.5f, 0.0f, 1.0f)
    };

    qreal hw = rect.width() * 0.5f;
    qreal hh = rect.height() * 0.5f;

    for (int i = 0; i < 4; i++)
    {
        QVector4D temp = matrix * points[i];
                            
        outPoints[i] = QVector2D(
            hw + temp.x() / temp.w() * hw, 
            hh + temp.y() / temp.w() * hh);

    }
    
    return true;
}

void HgVgQuad::computeWarpMatrix(VGfloat* matrix, int pxWidth, int pxHeight, const QVector2D* points)
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
/*    
    INFO("P0 x:" << points[0].x() << " y:" << points[0].y());
    INFO("P1 x:" << points[1].x() << " y:" << points[1].y());
    INFO("P2 x:" << points[2].x() << " y:" << points[2].y());
    INFO("P3 x:" << points[3].x() << " y:" << points[3].y());*/
}


void HgVgQuad::draw()
{
    if (!mQuad->visible())
        return;
    
    if (mDegenerate)
        return;
    
    HgVgImage* image = (HgVgImage*)mQuad->image();
    
    
    if (image == NULL  || image->alpha() == 0)
    {
        return;
        //drawImage(mRenderer->defaultImage(), 1.0f);
    }
    else
    {
        image->upload(mQuad->mirrorImageEnabled());
        
        if (image->image() == VG_INVALID_HANDLE)
        {
            drawImage(mRenderer->defaultImage(), 1.0f);
        }
        else
        {

            if ( mQuad->alpha() < 1.0f )
            {
                drawImage(mRenderer->defaultImage(), 1.0f - mQuad->alpha());            
            }
            
            drawImage(image, mQuad->alpha());
        }
    }
    
    
}

void HgVgQuad::drawImage(HgVgImage* image, qreal alpha)
{
    Q_UNUSED(alpha)

    //VGfloat values[] = { 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
    //values[3] = alpha;
    
    //vgSetfv(VG_COLOR_TRANSFORM_VALUES, 8, values);
        
    VGImage vgImage = image->image();
    
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

    VGfloat m[9];
    vgGetMatrix(m);
    
    vgMultMatrix(mMatrix);
    vgDrawImage(vgImage);    
    
    vgLoadMatrix(m);

    if (mQuad->mirrorImageEnabled())
    {
        VGImage mirrorImage = image->mirrorImage();
        if (mirrorImage == VG_INVALID_HANDLE)
            return;
        
        vgMultMatrix(mMirrorMatrix);
            
        vgDrawImage(mirrorImage);    
        vgLoadMatrix(m);    
    }
    
}


void HgVgQuad::getTransformedPoints(QPolygonF& poly) const
{
    poly.append(mTransformedPoints[0].toPointF());
    poly.append(mTransformedPoints[1].toPointF());
    poly.append(mTransformedPoints[2].toPointF());
    poly.append(mTransformedPoints[3].toPointF());    
}


HgVgQuadRenderer::HgVgQuadRenderer(int maxQuads) : 
    HgQuadRenderer(maxQuads),
    mDefaultVgImage(NULL)
{
    for (int i = 0; i < maxQuads; i++)
    {
        mTransformedQuads.append(new HgVgQuad(this));
    }
    mImageFader = new HgImageFader();
}

HgVgQuadRenderer::~HgVgQuadRenderer()
{
    delete mDefaultVgImage;
}

HgQuad* HgVgQuadRenderer::getQuadAt(const QPointF& point) const
{
    FUNC_LOG
        
    // TODO: need to use sorted quads here, in reversed order.
    QList<HgVgQuad*>::const_iterator i = mSortedQuads.begin();
    while(i != mSortedQuads.end())
    {
        HgVgQuad* q = (*i);
        if (q->isPointInside(point))
        {
            return q->mQuad;
        }
        i++;
    }
    
    return NULL;
}


void HgVgQuadRenderer::transformQuads(const QMatrix4x4& view, const QMatrix4x4& proj, 
    const QRectF& rect)
{
    QMatrix4x4 pv = proj * view;
    
    mSortedQuads.clear();
    
    for (int i = 0; i < mQuads.size(); i++)
    {
        HgQuad* q = mQuads[i];
            
        HgVgQuad* tq = mTransformedQuads[i];
        
        if (q->visible())
        {
            tq->transformQuad(i, pv, q, rect, mMirroringPlaneY);   
            mSortedQuads.append(tq);
        }
    }
        
    qSort(mSortedQuads.begin(), mSortedQuads.end(), quadSorter);
}

void HgVgQuadRenderer::drawQuads(const QRectF& rect, QPainter* painter)
{
    Q_UNUSED(rect)
            
    
    painter->beginNativePainting();

    // need to save old scissoring rects, otherwise hb
    // screws up with rendering
/*    VGint oldScissoring = vgGeti(VG_SCISSORING);
    VGint numRects = 32;//vgGeti(VG_MAX_SCISSOR_RECTS);
    VGint oldRects[32*4];
    vgGetiv(VG_SCISSOR_RECTS, numRects, oldRects);

    // setup our new scissoring rects
    VGint sRect[4];
    sRect[0] = rect.left();
    sRect[1] = rect.top();
    sRect[2] = rect.width();
    sRect[3] = rect.height();
    vgSeti(VG_SCISSORING, VG_TRUE);
    vgSetiv(VG_SCISSOR_RECTS, 4, sRect);
 */
    // setup root transform
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    vgLoadIdentity();    
    vgTranslate(rect.left(), rect.top());
    
    vgSeti(VG_COLOR_TRANSFORM, VG_FALSE);
    vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);
    vgSeti(VG_IMAGE_MODE, VG_DRAW_IMAGE_NORMAL);
    
    // draw quads
    for (int i = 0; i < mSortedQuads.size(); i++)
    {
        mSortedQuads[i]->draw();
    }
    
    // set back old scissor rects
  //  vgSeti(VG_SCISSORING, oldScissoring);
  //  vgSetiv(VG_SCISSOR_RECTS, numRects, oldRects);

    painter->endNativePainting();
    
}

bool HgVgQuadRenderer::getQuadTranformedPoints(QPolygonF& points, int index) const
{
    for (int i = 0; i < mSortedQuads.count(); i++)
    {
        HgVgQuad* quad = mSortedQuads[i];
        if (quad->mQuad)
        {
            bool bOk;
            if (quad->mQuad->userData().toInt(&bOk) == index)
            {
                quad->getTransformedPoints(points);
                return true;
            }
        }
    }
    
    return false;
}

HgImage* HgVgQuadRenderer::createNativeImage()
{    
    return new HgVgImage(this);   
}

HgVgImage* HgVgQuadRenderer::defaultImage()
{
    if (mDefaultVgImage == NULL)
    {
        QImage defaultImage(64,64,QImage::Format_RGB16);
        defaultImage.fill(qRgb(255,0,0));
        mDefaultVgImage = static_cast<HgVgImage*>(createNativeImage());
        mDefaultVgImage->setImage(defaultImage);
        mDefaultVgImage->upload(true);        
    }
    return mDefaultVgImage;
}

HgImageFader* HgVgQuadRenderer::imageFader()
{
    return mImageFader;
}

QList<HgQuad*> HgVgQuadRenderer::getVisibleQuads(const QRectF& rect) const
{
    FUNC_LOG;
    
    // this implementation isn't 100% precise
    QList<HgQuad*> result;
    for (int i = 0; i < mSortedQuads.count(); i++) {
       QPolygonF poly;
       mSortedQuads[i]->getTransformedPoints(poly);
       QRectF bounds = poly.boundingRect();       
       if (bounds.intersects(rect) || rect.contains(bounds)) {
           result.append(mSortedQuads[i]->mQuad);
       }           
    }
    
    return result;    
}


