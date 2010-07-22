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
*  Version     : %version: 2 %
*/
#ifndef BUFFERMANAGERHELPER_H
#define BUFFERMANAGERHELPER_H

#include <hgwidgets/hgcacheproxymodel.h>

class BMHelper: public HgBufferManagerObserver
{
public:
    BMHelper(int totalSize);
    ~BMHelper();
    
    virtual void release(int start, int end);
    virtual void request(int start, int end, HgRequestOrder order = HgRequestOrderAscending);  

    bool isIntergal(int bufferSize);
    int totalSize();
    void resizeCache(int newSize);

    void remove(int pos);
    void insert(int pos);

    
private:    
    QList< bool > mBuffer;
    
};
#endif // BUFFERMANAGERHELPER_H
