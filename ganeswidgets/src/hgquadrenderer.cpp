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

#include "hgquadrenderer.h"
#include "hgquad.h"

HgQuadRenderer::HgQuadRenderer(int maxQuads)
{
    for (int i = 0; i < maxQuads; i++)
    {
        mQuads.append(new HgQuad());
    }
}

HgQuadRenderer::~HgQuadRenderer()
{
    for (int i = 0; i < mQuads.size(); i++)
    {
        delete mQuads[i];
    }
}

int HgQuadRenderer::quadCount() const
    {
    return mQuads.size();
    }

HgQuad* HgQuadRenderer::quad(int index)
{
    return mQuads[index];
}

void HgQuadRenderer::setMirroringPlaneY(qreal mirroringPlaneY)
{
    mMirroringPlaneY = mirroringPlaneY;
}

void HgQuadRenderer::setDefaultImage(QPixmap defaultImage)
{    
    mDefaultImage = defaultImage;
}

void HgQuadRenderer::setTranslation(const QVector2D& translate)
{
    mTranslation = translate;
}

HgQuad* HgQuadRenderer::getQuadByUserData(const QVariant& userData) const
{
    for (int i = 0; i < mQuads.count(); i++)
    {
        HgQuad* quad = mQuads[i];
        if (quad->userData() == userData)
            return quad;
    }
    return NULL;
}

QPixmap HgQuadRenderer::defaultImage() const
{
    return mDefaultImage;
}

void HgQuadRenderer::enableReflections(bool enabled)
{
    mReflectionsEnabled = enabled;
}

bool HgQuadRenderer::reflectionsEnabled() const
{
    return mReflectionsEnabled;
}

