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
#include <QList>
#include "hglogger.h"
#include <hbicon.h>
#include "mydataprovider.h"

//#include "x:\sf\mw\qt\src\openvg\qpixmapdata_vg_p.h"
//typedef VGImage (*pfnVgCreateEGLImageTargetKHR)(VGeglImageKHR);

const int KItemIdRole = Qt::UserRole+1;

const int KThumbnailsPriority = EPriorityLess; //standard priority


MyDataProvider::MyDataProvider(QObject *parent) :
HgDataProviderModel(parent),
mDefaultIcon(new HbIcon(QIcon(QPixmap(":/icons/default.png")))),
mScheduler(new CActiveSchedulerWait()),
mWrapper( new ThumbnailManager() ),
mThumbnailRequestPending(false),
mThumbnailRequestIndex(-1),
mThumbnailRequestID(-1),
mThumbnailsize(ThumbnailManager::ThumbnailMedium),
mMDSLoadInProgress(false)
{
//    TX_ENTRY
    Q_UNUSED(parent);
    mWrapper->setThumbnailSize( mThumbnailsize );
    mWrapper->setQualityPreference( ThumbnailManager::OptimizeForPerformance );

    QObject::connect( mWrapper, SIGNAL(thumbnailReady( QPixmap , void* , int, int ) ),
                      this, SLOT( thumbnailReady( QPixmap , void* , int , int )));
    
    doResetModel(); //will read mds data
}

MyDataProvider::~MyDataProvider()
{
//    TX_ENTRY
    delete mDefaultIcon;
    delete mWrapper;
    delete mScheduler;
//    TX_EXIT
}

void MyDataProvider::changeIconSize(ThumbnailManager::ThumbnailSize aThumbnailsize)
{
//    TX_ENTRY
//	we could  remove all iconsfrom cache and put default ones, but probably that would be waste of time, and it's better just to load new icons with correct sizes.
	mThumbnailsize = aThumbnailsize;
	mWrapper->setThumbnailSize( mThumbnailsize );
//    TX_EXIT
}

void MyDataProvider::doRequestData(QList<int> list, bool silent)
{
    TX_ENTRY
	Q_UNUSED(silent);
    QString items = "Requested items:";
    int i = 0;
	for (int idx=0; idx<list.count(); idx++){
        i = list[idx];
		if (containsRole(i, KItemIdRole) && !containsRole(i, Qt::DecorationRole)) {//if there is icon, don't request new one
            items += QString("%0 ").arg(i);
			mWaitingThumbnails.append(i);
		}
	}
	TX_LOG_ARGS(items);
    getNextThumbnail();
    TX_EXIT
}

void MyDataProvider::getNextThumbnail()
{
//    TX_ENTRY
    if ( !mThumbnailRequestPending && mWaitingThumbnails.count()){
            int i = mWaitingThumbnails.takeFirst();
            if (i >=0 && i < count() && containsRole(i, KItemIdRole)) {
                int id = (data(i, KItemIdRole)).toInt();
                unsigned long int uId = (unsigned long int)id;
//                TX_LOG_ARGS(QString("getThumbnail for index:%0 uID:%1").arg(i).arg(uId));
                void *clientData = reinterpret_cast<void *>(i);
                mThumbnailRequestID = mWrapper->getThumbnail(uId, clientData, KThumbnailsPriority);
                mThumbnailRequestIndex = i;
                mThumbnailRequestPending = true;            
        }
    }
//    TX_EXIT    
}

void MyDataProvider::readMDSData()
{
    mMDSLoadInProgress = true;
}

void MyDataProvider::thumbnailReady( QPixmap pixmap, void* data, int id, int error )
{
//    TX_ENTRY    
    Q_UNUSED(id);
    if (!error && pixmap.rect().height()>0  && pixmap.rect().width()>0 ){
        int idx = reinterpret_cast<int>(data);
//        TX_LOG_ARGS(QString("thumbnailReady idx = %0").arg(idx));
        updateIcon(idx, createIcon(idx, pixmap));
	} else {
        TX_LOG_ARGS(QString("error:%0 id:%0").arg(error).arg(id));
	}
    mThumbnailRequestIndex = -1;
    mThumbnailRequestID = -1;
    mThumbnailRequestPending = false;            
    getNextThumbnail();
//    TX_EXIT
}

void MyDataProvider::doReleaseData(QList<int> list, bool silent)
{
//    TX_ENTRY    
    Q_UNUSED(silent);
    int i = 0;
    QString items = "Released items:";
    for (int idx=0;idx<list.count(); idx++){
        i = list[idx];
        if (mThumbnailRequestPending && mThumbnailRequestIndex==i ){
            TX_LOG_ARGS( QString("Cancel also thumbnail request for item %0").arg(i));
            mWrapper->cancelRequest(mThumbnailRequestID);
            mThumbnailRequestIndex = -1;
            mThumbnailRequestID = -1;
            mThumbnailRequestPending = false;
        }
        items += QString("%0 ").arg(i);
        mWaitingThumbnails.removeAll(i);
        releasePixmap(i);
    }
    TX_LOG_ARGS(items);

    getNextThumbnail();
//    TX_EXIT        
}

QVariant MyDataProvider::defaultIcon() const
{
    return *mDefaultIcon;
}

void MyDataProvider::HandleSessionOpened(CMdESession& aSession, TInt aError)
{
    Q_UNUSED(aSession);
    Q_UNUSED(aError);	
    mScheduler->AsyncStop();
}

// ----------------------------------------------------------------------------
// HandleQueryCompleted
// ----------------------------------------------------------------------------
//
void MyDataProvider::HandleQueryCompleted(CMdEQuery& aQuery, TInt aError)
{
    Q_UNUSED(aError);	
    clearCache();
    mMDSLoadInProgress = false;
    int size = aQuery.ResultIds().Count();
    
    for(int i = 0; i < size ; i++){
        QList< QPair< QVariant, int > > list;
        unsigned long int id = aQuery.ResultId(i);
        QVariant v;
        v.setValue(id);
        list.append( QPair< QVariant, int >(v, KItemIdRole) );
        int a = count();
        int b = -count();
		int c = a/4 + b/10 + i%30 + i/2;
		QString s = QString("ITEM%1 %2 %3").arg(c).arg(a).arg(b);
		if (i%2){
            s = s.toLower();
		}
        list.append( QPair< QVariant, int >(s, Qt::DisplayRole) );
        list.append( QPair< QVariant, int >(QVariant(a), Qt::UserRole+2) );
        list.append( QPair< QVariant, int >(QVariant(b), Qt::UserRole+3) );
        list.append( QPair< QVariant, int >(QVariant(c), Qt::UserRole+4) );
        
        newItem(&list);
    }
    
    TX_LOG_ARGS( QString("size=%1").arg(count()) );
    
    mScheduler->AsyncStop();
}

void MyDataProvider::doResetModel()
{

    if (mThumbnailRequestPending && mThumbnailRequestID!=-1){
        mWrapper->cancelRequest(mThumbnailRequestID);
    }
    mThumbnailRequestIndex = -1;
    mThumbnailRequestID = -1;
    mThumbnailRequestPending = false;
    mWaitingThumbnails.clear();
    
    //read MDS data once again
    TRAP_IGNORE(
        CMdESession* session = CMdESession::NewL( *this ); 
        mScheduler->Start();
        
        CMdENamespaceDef& namespaceDef = session->GetDefaultNamespaceDefL();
                
        CMdEObjectDef& objectDef = namespaceDef.GetObjectDefL(_L("Image"));
    
        CMdEObjectQuery* imageQuery = session->NewObjectQueryL( namespaceDef, objectDef, this );
        CleanupStack::PushL( imageQuery );
        imageQuery->SetResultMode( EQueryResultModeId );
        imageQuery->FindL( );
        mScheduler->Start();
        CleanupStack::PopAndDestroy( 1 );
        delete session;        
    );    
    
    
//    TX_EXIT    
}

