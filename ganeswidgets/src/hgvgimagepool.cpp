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

#include "hgvgimagepool.h"
#include "trace.h"

HgVgImagePool::HgVgImagePool(int numImages, int imgWidth, int imgHeight, bool alpha)
{
    for (int i = 0; i < numImages; i++)
    {
        VGImage vgimage = vgCreateImage(alpha ? VG_sARGB_8888 : VG_sRGB_565,
            imgWidth, imgHeight,VG_IMAGE_QUALITY_NONANTIALIASED);

        mFreeImages.push_back(vgimage);
    }
}
HgVgImagePool::~HgVgImagePool()
{
    QLinkedList<VGImage>::const_iterator it = mUsedImages.begin();
    while (it != mUsedImages.end())
    {
        vgDestroyImage((*it));
        it++;
    }

    QLinkedList<VGImage>::const_iterator it2 = mFreeImages.begin();
    while (it2 != mFreeImages.end())
    {
        vgDestroyImage((*it2));
        it2++;
    }

}
void HgVgImagePool::releaseImage(VGImage image)
{
    if (image == VG_INVALID_HANDLE)
        return;

    for (QLinkedList<VGImage>::iterator it = mUsedImages.begin();
        it != mUsedImages.end(); ++it)
    {
        if ((*it) == image)
        {
            mUsedImages.erase(it);
            break;
        }
    }
    vgClearImage(image, 0, 0, vgGetParameteri(image, VG_IMAGE_WIDTH), vgGetParameteri(image, VG_IMAGE_HEIGHT));
    mFreeImages.push_back(image);
}

VGImage HgVgImagePool::getImage()
{
    FUNC_LOG

    if (mFreeImages.empty())
    {
        INFO("Hg: Run out of images!");

        // if no free images left, use the one which was allocated
        // longer ago
        QLinkedList<VGImage>::iterator i = mUsedImages.begin();
        VGImage img = *i;
        mUsedImages.erase(i);
        mUsedImages.push_back(img);
        return img;
    }

    QLinkedList<VGImage>::iterator i = mFreeImages.begin();
    VGImage img = *i;
    mUsedImages.push_back(img);
    mFreeImages.erase(i);

    return img;
}
