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

#ifndef HGQUADRENDERER_H
#define HGQUADRENDERER_H

#include <QList>
#include <QImage>
#include <QVector2D>

class HgQuad;
class QPointF;
class QPainter;
class QRectF;
class QMatrix4x4;
class QPolygonF;
class HgImage;

/**
 *
 * Abstract class to be inherited by differend quad renderer implementations.
 * One could for example implement this in OpenGL instead of OpenVG.
 */
class HgQuadRenderer
{
public:
    /**
     * Destructor.
     */
    virtual ~HgQuadRenderer();
    /**
     * Gets the quad at given index.
     * @return HgQuad-object at index.        
     */
    virtual HgQuad* quad(int index);
    /**
     * Gets the max amount of quads this renderer can draw.
     * @return max amount of quads.
     */
    virtual int quadCount() const;
    /**
     * Gets quads index at pointer position.
     * @param pointer position.
     * @return quad under the pointer position or -1 if nothing is there.
     */
    virtual HgQuad* getQuadAt(const QPointF& point) const = 0;

    /**
     * Rasterize 3D quads to 2D screen using painter or underlying Native graphics API.
     * This should be called inside Widgets draw method.
     * @param painter QPainter-object.
     * @param rect window rectangle.
     * @param viewMatrix view matrix
     * @param projectionMatrix projection matrix
     * @param orientation current screen orientation
     */
    virtual void drawQuads(QPainter* painter, const QRectF& rect, 
        const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, 
        const Qt::Orientation orientation, 
        const QTransform& sceneTransform)=0;
    
    /**
     * 
     */
    virtual bool getQuadTranformedPointsByUserData(QPolygonF& points, const QVariant& userData) const=0;
    
    /**
     * 
     */
    virtual void setMirroringPlaneY(qreal mirroringPlaneY);

    /**
     * 
     */
    virtual HgImage* createNativeImage()=0;
        
    /**
     * 
     */
    virtual QList<HgQuad*> getVisibleQuads(const QRectF& rect) const=0;
    
    /**
     * 
     */
    virtual void setDefaultImage(QImage defaultImage);

    /**
     * 
     */
    virtual void setTranslation(const QVector2D& translate);
    
    /**
     * 
     */
    virtual HgQuad* getQuadByUserData(const QVariant& userData) const;
    
    /**
     * 
     */
    virtual QImage getDefaultImage() const;
    
    /**
     * 
     */
    virtual void enableReflections(bool enabled);
    
    /**
     * 
     */
    virtual bool reflectionsEnabled() const;
    
protected:    
    HgQuadRenderer(int maxQuads);

    QList<HgQuad*> mQuads;
    qreal mMirroringPlaneY;
    QImage mDefaultImage;
    QVector2D mTranslation;
    bool mReflectionsEnabled;
};

#endif
