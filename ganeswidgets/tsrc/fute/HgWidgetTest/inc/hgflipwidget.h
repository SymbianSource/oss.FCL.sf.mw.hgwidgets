/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Another view for test application.
*
*/
#ifndef HGFLIPWIDGET_H_
#define HGFLIPWIDGET_H_

#include <hbwidget.h>
#include <hbstackedwidget.h>

class HbAction;
class HbLabel;
class QPropertyAnimation;

class HgFlipWidget : public HbWidget
{
    Q_PROPERTY(qreal yRotation READ yRotation WRITE setYRotation)
    Q_OBJECT
public:
    enum State
    {
        StateClosed,
        StateOpening1,
        StateOpening2,
        StateOpened,
        StateClosing1,
        StateClosing2
    };
    
	HgFlipWidget(const QString &title1, const QString &title2, const QPixmap &pixmap, QGraphicsItem *parent=0);

	qreal yRotation();
    void setYRotation(qreal a);

    void close();
signals:
    void closed();
private slots:
    void animationFinished();
    void buttonClicked(bool checked=false);
private:
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void togglePage();
private:
    QPixmap mPixmap;
    HbStackedWidget *mStack;
    qreal mYRotation;
    QPropertyAnimation* mAnimation;
    State mState;
    HbLabel* mIconLabel;
};

#endif /* HGTESTVIEW_H_ */
