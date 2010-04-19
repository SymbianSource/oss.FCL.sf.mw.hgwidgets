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

#ifndef HGIMAGEFADER_H_
#define HGIMAGEFADER_H_

#include <qobject>
#include <qtimer>
#include <qlinkedlist>

class HgImage;

class HgImageFader : public QObject
{
    Q_OBJECT
public:
    HgImageFader();
    virtual ~HgImageFader();
    void addImage(HgImage* image);
    void removeImage(HgImage* image);
    void fadeImages();
signals:
    void doUpdate();
private slots:
    void doFading();
private:
    QTimer mTimer;
    QLinkedList<HgImage*> mImages;
};

#endif /* HGIMAGEFADER_H_ */
