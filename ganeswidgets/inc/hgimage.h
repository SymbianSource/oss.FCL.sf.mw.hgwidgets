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

#ifndef HGIMAGE_H
#define HGIMAGE_H

class QImage;

/**
 * Interface class for Images provided to HgQuadRenderer.
 */
class HgImage
{
public:
    virtual ~HgImage() {}
    virtual void setAlpha(qreal alpha) { mAlpha = alpha; }
    virtual qreal alpha() const { return mAlpha; }
    virtual void setImage(QImage& image)=0;
    virtual void releaseImage()=0;
private:
    qreal mAlpha;
};

#endif