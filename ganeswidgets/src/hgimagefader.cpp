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
#include "hgimagefader.h"
#include "hgimage.h"

HgImageFader::HgImageFader()
{
    QObject::connect(&mTimer, SIGNAL(timeout()), this, SLOT(doFading()));
}

HgImageFader::~HgImageFader()
{
    if (mTimer.isActive())
        mTimer.stop();
}

void HgImageFader::addImage(HgImage* image)
{
    mImages.push_back(image);
}

void HgImageFader::removeImage(HgImage* image)
{
    mImages.removeOne(image);
}

void HgImageFader::fadeImages()
{
    if (!mImages.empty() && !mTimer.isActive())
    {
        mTimer.start(10);
    }
}

void HgImageFader::doFading()
{
    QLinkedList<HgImage*>::iterator i = mImages.begin();
    while (i != mImages.end())
    {
        HgImage* image = (*i);
        image->setAlpha(image->alpha() + 0.1f);
        if (image->alpha() >= 1.0f)
        {
            image->setAlpha(1.0f);
            i = mImages.erase(i);
        }
        else
        {
            i++;
        }
    }
    
    //emit doUpdate();
    
    if (mImages.empty())
    {
        mTimer.stop();
    }
}
