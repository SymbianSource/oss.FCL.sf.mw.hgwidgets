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

#include "hgqtquadrenderer.h"
#include "hgquad.h"
#include "hgtransformedquad.h"
#include "trace.h"
#include "hgimage.h"

#include <QVector2D>
#include <QPolygon>
#include <QMatrix4x4>
#include <QPainter>
#include <QPixmapCache>


class HgQtImage : public HgImage
{
public:    
    HgQtImage(HgQtQuadRenderer* renderer)
    {
    Q_UNUSED(renderer)
    }

    ~HgQtImage()
    {
    }

    int width() const {
        return mPixmap.width();
    }

    int height() const {
        return mPixmap.height();
    }

    int mirrorPixmapWidth() const {
        return mMirrorPixmap.width();
    }

    int mirrorPixmapHeight() const {
        return mMirrorPixmap.height();
    }
    
    void setPixmap(const QPixmap& pixmap, bool createMirror) {
        mPixmap = pixmap;        
        if (createMirror) {
            createMirrorPixmap(mPixmap);
        } else {
            mMirrorPixmap = QPixmap();
        }
    }
    
    void releaseImages() {
        mPixmap = QPixmap();
        mMirrorPixmap = QPixmap();
    }
            
    QPixmap pixmap() const {
        return mPixmap;
    }
    
    QPixmap mirrorPixmap() {
        return mMirrorPixmap;
    }

    void setMirrorPixmap(const QPixmap& mirrorPixmap) {
        mMirrorPixmap = mirrorPixmap;
    }
    
    void createMirrorPixmap(const QPixmap& source) {
        if (!source.isNull()) {
            mMirrorPixmap = source.copy(QRect(0,source.height()*ReflectionHeight,source.width(),source.height()));
            QPainter painter(&mMirrorPixmap);
            painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
            QLinearGradient gradient(0.5,0.0,0.5,1);
            gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
            gradient.setColorAt(1, QColor::fromRgb(0,0,0,128));
            gradient.setColorAt(0, QColor::fromRgb(0,0,0,0));
            QBrush brush(gradient);
            painter.setBrush(brush);
            painter.setPen(Qt::NoPen);
            painter.drawRect(mMirrorPixmap.rect());
        }    
    }
    
    void updateMirror(bool enabled) {
        if (enabled && !mPixmap.isNull() && mMirrorPixmap.isNull()) {
            createMirrorPixmap(mPixmap);
        } else if(!enabled) {
            mMirrorPixmap = QPixmap();
        }
    }
    
    QPixmap mPixmap;
    QPixmap mMirrorPixmap;
};

class HgQtQuad : public HgTransformedQuad
{
public:
    
    HgQtQuad(HgQtQuadRenderer* renderer) : HgTransformedQuad(-1), mRenderer(renderer)
    {
        
    }

    ~HgQtQuad()
    {
        
    }

    void draw(QPainter* painter, const QRectF& rect, const QTransform& transform)    
    {
        if (!quad()->visible())
            return;
                
        HgQtImage* image = (HgQtImage*)quad()->image();        
        if (image == NULL)
            image = mRenderer->defaultImage();
        
        if (image == NULL)
            return;
        
        if (image->pixmap().isNull())
            image = mRenderer->defaultImage();
        
        drawImage(painter, image, rect, transform);                      
    }

    
private:

    void computeWarpMatrix(QTransform& tm, int pxWidth, int pxHeight, const QVector2D* points)
    {
        QPolygonF poly;

        poly << points[0].toPointF();
        poly << points[1].toPointF();
        poly << points[2].toPointF();
        poly << points[3].toPointF();

        QPolygonF img;

        img.append(QPointF(0,pxHeight));
        img.append(QPointF(pxWidth,pxHeight));
        img.append(QPointF(pxWidth,0));
        img.append(QPointF(0,0));

        QTransform::quadToQuad(img, poly, tm);
    }
    
    void drawImage(QPainter* painter, HgQtImage* image, const QRectF& rect, const QTransform& transform)
    {
        QPixmap pixmap = image->pixmap();
        
        if (pixmap.isNull())            
            return;
        
                
        const QVector2D* points = mTransformedPoints;
        if (mRenderer->isReflection() && quad()->mirrorImageEnabled()) {
            points = mMirroredPoints;
            pixmap = image->mirrorPixmap();            
        }
        
        QPolygonF poly;
        poly << points[0].toPointF();
        poly << points[1].toPointF();
        poly << points[2].toPointF();
        poly << points[3].toPointF();        
        QRectF bounds = poly.boundingRect();
        if (!(bounds.intersects(rect) || rect.contains(bounds))) {
            return;
        }
        
        computeWarpMatrix(mTransform, pixmap.width(), pixmap.height(), points);
        
        painter->setTransform(mTransform * transform);    

        painter->drawPixmap(QPointF(0,0), pixmap);
    }

    HgQtQuadRenderer* mRenderer;
    QTransform mTransform;
    QTransform mMirrorTransform;
};


HgQtQuadRenderer::HgQtQuadRenderer(int maxQuads) : 
    HgTransformedQuadRenderer(maxQuads),
    mDefaultQtImage(NULL)
{
    // initialize base class to the end.
    init(maxQuads);
    QImage image(QSize(250,250), QImage::QImage::Format_ARGB32_Premultiplied);
    image.fill(0xFFFFFFFF);
    setDefaultImage(QPixmap::fromImage(image));
    
    QPixmapCache::setCacheLimit(2048);
}

HgQtQuadRenderer::~HgQtQuadRenderer()
{
    delete mDefaultQtImage;
}

void HgQtQuadRenderer::drawQuads(QPainter* painter, const QRectF& rect, 
    const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix,
    Qt::Orientation orientation, 
    const QTransform& sceneTransform)
{
    Q_UNUSED(orientation)
    Q_UNUSED(sceneTransform)

    transformQuads(viewMatrix, projectionMatrix, 
        QPointF(rect.width()/2, rect.height()/2), QSizeF(rect.width(), rect.height()));
    
    // save old transform
    QTransform temp = painter->transform();

    if (mReflectionsEnabled) 
    {
        mIsReflection = true;

        drawTransformedQuads(painter, rect, temp);    

        painter->setTransform(temp);        
        drawFloor(painter, rect);
    }
    
    mIsReflection = false;
    
    drawTransformedQuads(painter, rect, temp);
        
    painter->setTransform(temp);
    
}

HgImage* HgQtQuadRenderer::createNativeImage()
{    
    return new HgQtImage(this);   
}

HgQtImage* HgQtQuadRenderer::defaultImage()
{
    return mDefaultQtImage;
}

void HgQtQuadRenderer::setDefaultImage(QPixmap defaultImage)
{
    HgQuadRenderer::setDefaultImage(defaultImage);
    
    delete mDefaultQtImage;
    mDefaultQtImage = 0;
    
    mDefaultQtImage = static_cast<HgQtImage*>(createNativeImage());
    mDefaultQtImage->setPixmap(mDefaultImage, true);
}

HgTransformedQuad* HgQtQuadRenderer::createNativeQuad()
{
    return new HgQtQuad(this);
}

bool HgQtQuadRenderer::isReflection() const
{
    return mIsReflection;
}

void HgQtQuadRenderer::drawFloor(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
}

