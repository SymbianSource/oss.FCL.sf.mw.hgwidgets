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
#ifndef MYDATAPROVIDER2_H
#define MYDATAPROVIDER2_H

#include <QtGui>

#include <thumbnailmanager_qt.h>
#include <hgwidgets/hgdataprovidermodel.h>
#include <QList>
#include <QPair>
#include <mdesession.h>
#include <mdequery.h>
#include "flogger.h"

class HbIcon;
class CMdESession;
class CActiveSchedulerWait;
class QEventLoop;

class MyDataProvider : public HgDataProviderModel, public MMdESessionObserver, public MMdEQueryObserver
{
    Q_OBJECT

public:
    MyDataProvider(QObject *parent = 0);
    ~MyDataProvider();
    void changeIconSize(ThumbnailManager::ThumbnailSize aThumbnailsize);
	
//from MMdESessionObserver and MMdEQueryObserver
    virtual void HandleSessionOpened(CMdESession& aSession, TInt aError);
    virtual void HandleSessionError(CMdESession& , TInt ){};
    virtual void HandleQueryNewResults(CMdEQuery& , TInt ,TInt ){};
    virtual void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);
	
//from HgDataProviderModel
    virtual void doReleaseData(QList<int> list, bool silent);
    virtual void doRequestData(QList<int> list, bool silent);  
    virtual QVariant defaultIcon() const;    

protected:
    virtual void doResetModel();
    
public slots:
    void thumbnailReady( QPixmap , void* , int, int ); 

private:
    void getNextThumbnail();
    void readMDSData();
	
private:
    HbIcon* mDefaultIcon;
    CActiveSchedulerWait* mScheduler;
    ThumbnailManager* mWrapper;
    QList<int> mWaitingThumbnails;
    bool mThumbnailRequestPending;
    int mThumbnailRequestIndex;
    int mThumbnailRequestID;
	ThumbnailManager::ThumbnailSize mThumbnailsize;
	bool mMDSLoadInProgress;
};

#endif // MYDATAPROVIDER2_H

