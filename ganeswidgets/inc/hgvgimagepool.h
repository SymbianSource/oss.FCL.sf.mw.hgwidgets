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

#ifndef HGVGIMAGEPOOL_H_
#define HGVGIMAGEPOOL_H_

#include <VG/openvg.h>
#include <qlinkedlist>

class HgVgImagePool
{
public:
    HgVgImagePool(int numImages, int imgWidth, int imgHeight, bool alpha);
    ~HgVgImagePool();
    void releaseImage(VGImage image);
    VGImage getImage();
private:
    QLinkedList<VGImage> mFreeImages;
    QLinkedList<VGImage> mUsedImages;
};


#endif /* HGVGIMAGEPOOL_H_ */
