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
class QPixmap;
/**
 * Interface class for Images provided to HgQuadRenderer.
 */
class HgImage
{
public:
    virtual ~HgImage() {}
    virtual void setAlpha(qreal alpha) { mAlpha = alpha; }
    virtual qreal alpha() const { return mAlpha; }
    virtual void setPixmap(const QPixmap& pixmap, bool createMirror=false)=0;
    virtual void updateMirror(bool enabled) = 0;
    virtual void releaseImages()=0;
    virtual int width() const = 0;
    virtual int height() const = 0;
    virtual void setMirrorPixmap(const QPixmap& pixmap) = 0;
    virtual QPixmap pixmap() const = 0;
private:
    qreal mAlpha;
};

#endif
