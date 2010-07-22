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

#ifndef HGQUAD_H
#define HGQUAD_H

#include <QVector3D>
#include <QVector2D>
#include <QQuaternion>
#include <QVariant>

class HgImage;

/**
 * Quad class to be used by HgQuadRenderer.
 */
class HgQuad
{
public:
    /**
     * Constructor.
     */
    HgQuad();
    /**
     * Destructor.
     */
    virtual ~HgQuad();
    /**
     * Sets the position of the quad.
     * @param position in 3D space.
     */
    void setPosition(const QVector3D& position);
    /**
     * Sets the rotation of the quad.
     * @param rotation as quaternion.
     */
    void setRotation(const QQuaternion& rotation);
    /**
     * Sets the image to use in this quad.
     * @param image to use in this quad.
     */
    void setImage(const HgImage* image);
    /**
     * Toggles quads visible/hidden.
     * @param visible true to make quad visible.
     */
    void setVisible(bool visible);
    /**
     * Sets the pivot point of the quad.
     * @pivot to set for this quad. Quads are considered unit quads so to set pivot for example,
     * to bottom left corner you would use QVector2D(-0.5f, -0.5f).
     */
    void setPivot(const QVector2D& pivot);
    /**
     * Sets the scale of the quad.
     * @param scale scaling 2d vector (appliend before rotation but after pivot is transformed)
     */
    void setScale(const QVector2D& scale);
    /**
     * Sets user data to this quad.
     * @param userdata to be carried in this quad. This can be for example to identify object that is represented by this specific quad instance.
     */
    void setUserData(const QVariant& data);
    /**
     * Gets the image in this quad.
     * @return pointer HgImage-object carried by this quad.
     */
    const HgImage* image() const;
    /**
     * Gets the position of this quad.
     * @return position vector.
     */
    const QVector3D& position() const;
    /**
     * Gets the roation of this quad.
     * @return rotation quaternion.
     */
    const QQuaternion& rotation() const;
    /**
     * Tells whether or not this quad is visible and should be rendered.
     * @return true if this quad is visible and should be rendered.
     */
    bool visible() const;
    /**
     * Gets the pivot point of this quad.
     * @return pivot point applied to this quads trasformation.
     */
    const QVector2D& pivot() const;
    /**
     * Gets the scale of this quad.
     * @return scaling applied to this quads transformation.
     */
    const QVector2D& scale() const;
    /**
     * Gets the user data associated in to this quad.
     * @return user data carriend by this quad.
     */
    const QVariant& userData() const;
    /**
     * 
     */
    HgQuad* copy() const;
    /**
     * 
     */
    void copyFrom(const HgQuad& quad);
    /*
     * 
     */
    void setOuterRotation(const QQuaternion& rot);
    /*
     * 
     */
    const QQuaternion outerRotation() const;
    /**
     * 
     */
    void enableMirrorImage(bool enabled);
    /**
     * 
     */
    bool mirrorImageEnabled() const;
    /**
     * 
     */
    void setAlpha(qreal alpha);
    /**
     * 
     */
    qreal alpha() const;
private:    
    QVector3D mPosition;
    QQuaternion mRotation;
    const HgImage* mImage;
    bool mVisible;
    QVector2D mPivot;
    QVector2D mScale;
    QVariant mUserData;
    QQuaternion mOuterRotation;
    bool mDrawMirror;
    qreal mAlpha;
private:
    Q_DISABLE_COPY(HgQuad)
};


#endif
