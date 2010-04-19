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

#include "HgQuad.h"

HgQuad::HgQuad() : 
mRotation(QQuaternion(1,0,0,0)), 
mImage(NULL), 
mVisible(false), 
mPivot(0, 0), 
mScale(1, 1), 
mOuterRotation(QQuaternion(1,0,0,0)),
mDrawMirror(false),
mAlpha(1)
{
}

HgQuad::~HgQuad()
{
}

void HgQuad::setPosition(const QVector3D& position)
{
    mPosition = position;
}
    
void HgQuad::setRotation(const QQuaternion& rotation)
{
    mRotation = rotation;
}
    
const QVector3D& HgQuad::position() const
{
    return mPosition;
}
    
const QQuaternion& HgQuad::rotation() const
{
    return mRotation;
}

void HgQuad::setImage(const HgImage* image)
{
    mImage = image;
}

const HgImage* HgQuad::image() const
{
    return mImage;
}

void HgQuad::setVisible(bool visible)
{
    mVisible = visible;
}

bool HgQuad::visible() const
{
    return mVisible;
}

void HgQuad::setPivot(const QVector2D& pivot)
{
    mPivot = pivot;
}

void HgQuad::setScale(const QVector2D& scale)
{
    mScale = scale;
}

void HgQuad::setUserData(const QVariant& userData)
{
    mUserData = userData;
}

const QVector2D& HgQuad::pivot() const
    {
    return mPivot;
    }

const QVector2D& HgQuad::scale() const
    {
    return mScale;
    }

const QVariant& HgQuad::userData() const
    {
    return mUserData;
    }

HgQuad* HgQuad::copy() const
    {
    HgQuad* q = new HgQuad();
    q->copyFrom(*this);
    return q;
    }


void HgQuad::copyFrom(const HgQuad& quad)
    {
    mPosition = quad.mPosition;
    mRotation = quad.mRotation;
    mScale = quad.mScale;
    mPivot = quad.mPivot;
    mImage = quad.mImage;
    mUserData = quad.mUserData;
    mVisible = quad.mVisible;
    }

void HgQuad::setOuterRotation(const QQuaternion& rot)
{
    mOuterRotation = rot;
}

const QQuaternion HgQuad::outerRotation() const
{
    return mOuterRotation;
}

void HgQuad::enableMirrorImage(bool enabled)
{
    mDrawMirror = enabled;
}

bool HgQuad::mirrorImageEnabled() const
{
    return mDrawMirror;
}

void HgQuad::setAlpha(qreal alpha)
{
    mAlpha = alpha;
}

qreal HgQuad::alpha() const
{
    return mAlpha;
}


