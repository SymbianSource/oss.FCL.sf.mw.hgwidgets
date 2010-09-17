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

    int width() const
    {
        return mPixmap.width();
    }

    int height() const
    {
        return mPixmap.height();
    }

    int mirrorImageWidth() const
    {
        return width();
    }

    int mirrorImageHeight() const
    {
        return height();
    }

    void setImage(const QImage& image)
    {
        mPixmap = QPixmap::fromImage(image);
        //mMirrorPixmap = QPixmap();
    }
    
    void setPixmap(const QPixmap& pixmap)
    {
        mPixmap = pixmap;
        //mMirrorPixmap = QPixmap();
    }
    
    void releaseImage()
    {
        //mPixmap = QPixmap();
        //mMirrorPixmap = QPixmap();
    }
    
    QImage getQImage() const
    {
        return mPixmap.toImage();
    }
        
    const QPixmap& pixmap() const
    {
        return mPixmap;
    }
    
    const QPixmap& mirrorPixmap(QPainter* painter)
    {
        Q_UNUSED(painter)

        return mPixmap;
/*        
        if (mPixmap.isNull())
            return mPixmap;

        if (mMirrorPixmap.isNull())
        {
            QImage img = mPixmap.toImage();
            QImage mirrorImage = img.scaled(QSize(img.width()/3,img.height()/3)).convertToFormat(QImage::Format_ARGB32);
            
            // apply gradient to alpha channel so that mirror image looks like
            // it fades under the floor
            for (int i = 0; i < mirrorImage.height(); i++)
            {
                qreal t = qreal(i) / qreal(mirrorImage.height());
                int a = (int)(t * 255.0);
                uchar* scanline = mirrorImage.scanLine(i);
                for (int j = 0; j < mirrorImage.width(); j++)
                {
                    scanline[j*4+0] /= 3;
                    scanline[j*4+1] /= 3;
                    scanline[j*4+2] /= 3;
                    scanline[j*4+3] = 255;
                }        
            }
            
            mMirrorPixmap = QPixmap::fromImage(mirrorImage);
            
            QPaintDevice* device = painter->device();
            painter->end();

            mMirrorPixmap = mPixmap.scaled(100,100);
            int w = mMirrorPixmap.width();
            int h = mMirrorPixmap.height();
            //QPainter p;
            painter->begin(&mMirrorPixmap);
            painter->fillRect(0,0,w, h, QColor::fromRgbF(0, 0, 0, 0.5f));
            painter->end();

            painter->begin(device);
        
        }
        
        
        return mMirrorPixmap;*/
    }

    QPixmap mPixmap;
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
        const QPixmap& pixmap = image->pixmap();
        
        if (pixmap.isNull())            
            return;
        
        const QVector2D* points = mTransformedPoints;
        if (mRenderer->isReflection() && quad()->mirrorImageEnabled())
            points = mMirroredPoints;
        
        QPolygonF poly;
        poly << points[0].toPointF();
        poly << points[1].toPointF();
        poly << points[2].toPointF();
        poly << points[3].toPointF();        
        QRectF bounds = poly.boundingRect();
        if (!(bounds.intersects(rect) || rect.contains(bounds))) {
            return;
        }
        
        computeWarpMatrix(mTransform, image->width(), image->height(), points);
        
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
    QImage image(QSize(200,200), QImage::Format_RGB16);
    image.fill(0xFFFFFFFF);
    setDefaultImage(image);
    
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

void HgQtQuadRenderer::setDefaultImage(QImage defaultImage)
{
    HgQuadRenderer::setDefaultImage(defaultImage);
    
    delete mDefaultQtImage;
    mDefaultQtImage = 0;
    
    mDefaultQtImage = static_cast<HgQtImage*>(createNativeImage());
    mDefaultQtImage->setImage(mDefaultImage);

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
    QRectF floorRect(0, rect.height()/2, rect.width(), rect.height()/2);
    QBrush brush(QColor::fromRgbF(0,0,0,0.5f));
    painter->setBrush(brush);
    painter->drawRect(floorRect);
}

