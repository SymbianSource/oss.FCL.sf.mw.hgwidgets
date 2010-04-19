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

#ifndef HGVGIMAGE_H
#define HGVGIMAGE_H

#include <qstring>
#include <qimage>
#include <VG/openvg.h>
#include "HgImage.h"

class HgVgImagePool;
class HgImageFader;
class HgVgQuadRenderer;
/**
 * Implements HgImage for HgVgQuadRenderer implementation.
 */
class HgVgImage : public HgImage
{
public:
    HgVgImage(HgVgQuadRenderer* renderer);
    virtual ~HgVgImage();

    int width() const;
    int height() const;
    
    VGImage image() const;
    
    int mirrorImageWidth() const;
    int mirrorImageHeight() const;
    
    VGImage mirrorImage() const;
    
    void setImage(QImage& image);
    void releaseImage();
    
    void upload(bool mirror=false);
  
private:    
    VGImage mVgImage;
    VGImage mMirrorImage;
    QSize mMirrorSize;
    QImage mQImage;
    HgVgQuadRenderer* mRenderer;
private:
    Q_DISABLE_COPY(HgVgImage)
};

#endif
