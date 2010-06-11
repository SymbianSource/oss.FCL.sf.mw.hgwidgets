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

#include "hgvgimage.h"
#include "HgImageFader.h"
#include "hgvgquadrenderer.h"

const int KMaxMirrorWidth(128);
const int KMaxMirrorHeight(64);

static QSize getMirrorSize(const QImage& image)
{
    return QSize(
        qMin(image.width() / 2, KMaxMirrorWidth), 
        qMin(image.height() / 4, KMaxMirrorHeight) 
        );
}

HgVgImage::HgVgImage(HgVgQuadRenderer* renderer) :
    mVgImage(VG_INVALID_HANDLE),
    mMirrorImage(VG_INVALID_HANDLE),
    mMirrorSize(0,0),
    mRenderer(renderer)
{
    setAlpha(1.0);
}

HgVgImage::~HgVgImage()
{
    releaseImage();
}

int HgVgImage::width() const
{
    return mQImage.width();
}

int HgVgImage::height() const
{
    return mQImage.height();
}

VGImage HgVgImage::image() const
{
    return mVgImage;
}

int HgVgImage::mirrorImageWidth() const
{
    return mMirrorSize.width();
}

int HgVgImage::mirrorImageHeight() const
{
    return mMirrorSize.height();
}
    
VGImage HgVgImage::mirrorImage() const
{
    return mMirrorImage;
}


void HgVgImage::setImage(const QImage& image)
{
    if (image.isNull())
        return;

    // release previous images vg data
    releaseImage();
        
    mQImage = image;

    mMirrorSize = getMirrorSize(mQImage);

}

void HgVgImage::setPixmap(const QPixmap& pixmap)
{
    Q_UNUSED(pixmap)    
}

QImage HgVgImage::getQImage() const
{
    return mQImage;
}


void HgVgImage::releaseImage()
{
    if (mVgImage != VG_INVALID_HANDLE)
    {
        vgDestroyImage(mVgImage);
        mVgImage = VG_INVALID_HANDLE;            
    }
    if (mMirrorImage != VG_INVALID_HANDLE)
    {
        vgDestroyImage(mMirrorImage);
        mMirrorImage = VG_INVALID_HANDLE;            
    }
    
    // TODO, check that this will free all the previously reserved image resources.
    mQImage = QImage();
}

void HgVgImage::upload(bool mirror)
{
    if( mQImage.isNull())
        return;    
    
    if (mVgImage == VG_INVALID_HANDLE)
    {            
                
        VGImageFormat format;

        switch (mQImage.format())
        {
        case QImage::Format_ARGB32_Premultiplied:
            format = VG_sARGB_8888_PRE;
            break;
        case QImage::Format_ARGB32:
            format = VG_sARGB_8888;
            break;
        case QImage::Format_RGB16:
            format = VG_sRGB_565;
            break;
        default:
            mQImage = mQImage.convertToFormat(QImage::Format_RGB16);
            format = VG_sRGB_565;
            break;
        }
        

        mVgImage = vgCreateImage(format, 
            mQImage.width(), mQImage.height(),VG_IMAGE_QUALITY_NONANTIALIASED);
        if (mVgImage == VG_INVALID_HANDLE)
        {
            qWarning("HgMediaWall run out of video memory for images!");
            return;
        }
        
        // bits function performs deep copy if we dont fetch data with const specifier.
        const uchar *bits = mQImage.bits();
        
        vgImageSubData(mVgImage, bits, mQImage.bytesPerLine(), 
            format, 0, 0, mQImage.width(), mQImage.height() );
        
    }
    
    if (mirror && mMirrorImage == VG_INVALID_HANDLE)
    {
        mMirrorImage = vgCreateImage(VG_sARGB_8888_PRE, mMirrorSize.width(), mMirrorSize.height(), VG_IMAGE_QUALITY_NONANTIALIASED);
        if (mMirrorImage == VG_INVALID_HANDLE)
        {
            qWarning("HgMediaWall run out of video memory for images!");
            return;
        }
        
        // cut half of the image, then scale to desired width/height if needed
        QImage mirrorImage = mQImage.copy(0, mQImage.height()/2, mQImage.width(), mQImage.height()/2).scaled(mMirrorSize).convertToFormat(QImage::Format_ARGB32);
        
        // apply gradient to alpha channel so that mirror image looks like
        // it fades under the floor
        for (int i = 0; i < mirrorImage.height(); i++)
        {
            qreal t = qreal(i) / qreal(mirrorImage.height());
            int a = (int)(t * 255.0);
            uchar* scanline = mirrorImage.scanLine(i);
            for (int j = 0; j < mirrorImage.width(); j++)
            {
                scanline[j*4+3] = a;
            }        
        }
        
        // copy data to vg image
        vgImageSubData(mMirrorImage, mirrorImage.bits(), mirrorImage.bytesPerLine(), 
            VG_sARGB_8888, 0, 0, mirrorImage.width(), mirrorImage.height() );

    }
    
}

