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
*  Version     : %version: 1 %
*/
#include "bmhelper.h"
#include <QDebug>

BMHelper::BMHelper(int totalSize)
{
    for ( int i(0); i < totalSize; i++)
        mBuffer.append(false);
}

BMHelper::~BMHelper()
{
}

void BMHelper::release(int start, int end)
{
    if ( start<0)
        start = 0;
    if (end>mBuffer.size() - 1)
        end = mBuffer.size() - 1;
    
    for ( int i = start; i <= end; i++){
        if (mBuffer.value(i) == true){
            mBuffer.replace(i, false);
        } else {
//            qWarning()<<QString("release released item %1").arg(i);
        }
    }
}

void BMHelper::request(int start, int end, HgRequestOrder order)
{
    Q_UNUSED(order);
    if ( start<0)
        start = 0;
    if (end>mBuffer.size() - 1)
        end = mBuffer.size() - 1;
    
    for ( int i = start; i <= end; i++){
        if (mBuffer.value(i) == false){
            mBuffer.replace(i, true);
        } else {
//            qWarning()<<QString("request requested item %1").arg(i);
        }
    }
}

bool BMHelper::isIntergal(int bufferSize)
{
    int c = mBuffer.count(true);
    bool res = (bufferSize == c);
    if (res){ ///check integrity ( if all items from first true, to size are true;
        int f = mBuffer.indexOf(true);
        for ( int i =0; i < mBuffer.count(); i++){
            if (mBuffer[i] != (i>=f && i < f+bufferSize) ){
                res = false;
                break;
            }
        }
    } else {
        qWarning()<<QString("isIntergal mBuffer.count(true)=%1 bufferSize=%2").arg(c).arg(bufferSize);
    }
    
    return res;
}

void BMHelper::itemCountChanged( int aIndex, bool aRemoved, int aNewTotalCount )
{
    Q_UNUSED(aRemoved);
    
    if ( aIndex < 0)
        aIndex = 0;
    if ( aIndex > mBuffer.count())
        aIndex = mBuffer.count()-1;
    
    if ((mBuffer.count() - aNewTotalCount)>0){
        while (mBuffer.count()!=aNewTotalCount){
            if (aIndex > mBuffer.count() )
                aIndex = mBuffer.count() -1;
            mBuffer.removeAt(aIndex);
        }
    } else if ((mBuffer.count() - aNewTotalCount)<0){
        while (mBuffer.count()!=aNewTotalCount){
            mBuffer.insert(aIndex, false);
        }
    }
}
