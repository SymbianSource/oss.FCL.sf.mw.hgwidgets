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

#include <qlist>

class HgQuad;
class QPointF;
class QPainter;
class QRectF;
class QMatrix4x4;
class QPolygonF;
class HgImage;
class QImage;
class HgImageFader;
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
     * Transform quads with view and projection transformation for rendering.
     * @param view view transformation matrix.
     * @param projection transformation matrix.
     * @rect window rectangle.
     */
    virtual void transformQuads(const QMatrix4x4& view, const QMatrix4x4& proj, 
        const QRectF& rect)=0;
    /**
     * Draw transformed quads using painter or underlying Native graphics API.
     * This should be called inside Widgets draw method.
     * @param rect window rectangle.
     * @param painter QPainter-object.
     */
    virtual void drawQuads(const QRectF& rect, QPainter* painter)=0;
    
    /**
     * 
     */
    virtual bool getQuadTranformedPoints(QPolygonF& points, int index) const=0;
    
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
    virtual void setImageFader(HgImageFader* fader);
    
    /**
     * 
     */
    virtual QList<HgQuad*> getVisibleQuads(const QRectF& rect) const=0;
protected:    
    HgQuadRenderer(int maxQuads);

    QList<HgQuad*> mQuads;
    qreal mMirroringPlaneY;
    HgImageFader* mImageFader;
};

#endif
