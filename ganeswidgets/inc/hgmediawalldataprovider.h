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

#ifndef HGMEDIAWALLDATAPROVIDER_H
#define HGMEDIAWALLDATAPROVIDER_H

class HgImage;

/**
 * Data Provider interface for HgMediWallRenderer.
 */
class HgMediaWallDataProvider
{
public:
    virtual ~HgMediaWallDataProvider() {}
    /**
     * Gets the number of images the model contains.
     * @return number of images the model contains.
     */
    virtual int imageCount() const=0;
    /**
     * Gets the image at given index.
     * @param index of the image in the model
     * @return pointer to HgImage-object.
     */
    virtual const HgImage* image(int index) const=0;
    /**
     * Gets the the flags associated to the item at index
     * @param index of the item
     * @return flags of the item
     */
    virtual int flags(int index) const=0;
    /**
     * Gets the image associated to given flags.
     * @param flags.
     * @return pointer HgImage-object.
     */
    virtual const HgImage* indicator(int flags)=0;
};

#endif
