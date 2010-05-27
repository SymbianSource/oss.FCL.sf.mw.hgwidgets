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
*  Version     : %version: 5 %
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
    if ((start <0 && end <0) || (start >=mBuffer.size() && end >=mBuffer.size() ) )
        return;
    
    if ( start<0)
        start = 0;
    if (end>mBuffer.size())
        end = mBuffer.size();
    
    if ( start > end){
        int t = start;
        start = end;
        end = t;
    }
    
    if ( start<0)
        start = 0;
    if (end>=mBuffer.size())
        end = mBuffer.size()-1;
    
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
    
    if ((start <0 && end <0) || (start >=mBuffer.size() && end >=mBuffer.size() ) )
        return;
    
    if ( start<0)
        start = 0;
    if (end>=mBuffer.size())
        end = mBuffer.size()-1;
    
    if ( start > end){
        int t = start;
        start = end;
        end = t;
    }
    if ( start<0)
        start = 0;
    if (end>=mBuffer.size())
        end = mBuffer.size()-1;
    
    
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
    bool res = (c == mBuffer.count())||(bufferSize == c);
    
    
    if (res){ ///check integrity ( if all items from first true, to size are true;
        int f = mBuffer.indexOf(true);
        for ( int i =0; i < mBuffer.count(); i++){
            if (mBuffer[i] != (i>=f && i < f+c) ){
                res = false;
                break;
            }
        }
    } else {
        QString arr;
        QString item = "%1,";        
        for ( int i =0; i < mBuffer.count(); i++){
            if(mBuffer[i]){
                arr+=item.arg(i);
            }
        }
        qWarning()<<QString("isIntergal mBuffer.count(true)=%1 bufferSize=%2 visible:%3").arg(c).arg(bufferSize).arg(arr);
    }
    
    return res;
}
int BMHelper::totalSize()
{
    return mBuffer.count();
}

void BMHelper::remove(int pos)
{
    if ( pos <0 || pos > mBuffer.count()){
        return;
    }
    mBuffer.removeAt(pos);    
}

void BMHelper::insert(int pos)
{
    if ( pos <0 || pos > mBuffer.count()){
        return;
    }    
    mBuffer.insert(pos, false);
}
