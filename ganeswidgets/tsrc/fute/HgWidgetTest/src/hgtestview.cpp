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
#include <qgraphicslinearlayout.h>
#include <qgraphicssceneresizeevent>
#include <hbaction.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <hbmainwindow.h>
#include "hgtestview.h"
#include "trace.h"

HgTestView::HgTestView(const QString &title1, const QString &title2, const QPixmap &pixmap, QGraphicsItem *parent) :
    HbView(parent), mPixmap(pixmap)
{
    FUNC_LOG;

    QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
    if (mainWindows.count() > 0)
    {
        HbMainWindow *primaryWindow = mainWindows[0];

        setTitle(primaryWindow->currentView()->title());

        HbAction *backAction = new HbAction(Hb::BackNaviAction);
        connect(backAction, SIGNAL(triggered()), SIGNAL(closeRequested()));
        setNavigationAction(backAction);

        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        HANDLE_ERROR_NULL(layout);
        if (layout)
        {
            HbLabel *title1Label = new HbLabel(title1);
            title1Label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            layout->addItem(title1Label);

            HbLabel *title2Label = new HbLabel(title2);
            title2Label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            layout->addItem(title2Label);

            mIconLabel = new HbLabel;
            HANDLE_ERROR_NULL(mIconLabel);
            if (mIconLabel)
            {
                mIconLabel->setIcon(HbIcon(mPixmap));
                mIconLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
                layout->addItem(mIconLabel);
            }
            setLayout(layout);
        }
    }
}

void HgTestView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    if (mIconLabel && event)
    {
        QSize iconSize(event->newSize().width()-5, event->newSize().height()-20);
        mIconLabel->setIcon(HbIcon(mPixmap.scaled(iconSize, Qt::KeepAspectRatio)));
    }
}

