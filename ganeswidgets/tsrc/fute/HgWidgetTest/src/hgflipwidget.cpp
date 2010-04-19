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
#include "hgflipwidget.h"

#include <qgraphicslinearlayout.h>
#include <qgraphicssceneresizeevent>
#include <hbaction.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <hbmainwindow.h>
#include <hblistwidget.h>
#include <hbpushbutton.h>
#include <qpropertyanimation>
#include <qstate.h>
#include <qabstracttransition>
#include <qstatemachine>
#include <qsignaltransition>
#include "trace.h"

HgFlipWidget::HgFlipWidget(const QString &title1, const QString &title2, const QPixmap &pixmap, QGraphicsItem *parent) :
    HbWidget(parent), mPixmap(pixmap), mYRotation(0)
{
    FUNC_LOG;
    
    Q_UNUSED(title1)
    Q_UNUSED(title2)
    
    mStack = new HbStackedWidget();
    
    HbWidget* front = new HbWidget();       
    {
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        mIconLabel = new HbLabel;
        HANDLE_ERROR_NULL(mIconLabel);
        if (mIconLabel)
        {
            mIconLabel->setIcon(HbIcon(mPixmap));
            mIconLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            layout->setContentsMargins(0,0,0,0);
            layout->addItem(mIconLabel);
        }    
        front->setLayout(layout);
    }
        
    HbWidget* back = new HbWidget();
    {
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
        HbListWidget* widget = new HbListWidget;
        widget->addItem(QString("1. First item"));
        widget->addItem(QString("2. Second item"));
        widget->addItem(QString("3. Third item"));
        widget->addItem(QString("4. Fourth item"));
        HbPushButton* button = new HbPushButton("close");        
        layout->addItem(widget);
        layout->addItem(button);
        QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
        layout->setContentsMargins(0,0,0,0);
        back->setLayout(layout);
    }
    
    mStack->addWidget(front);
    mStack->addWidget(back);
    
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(this);
    layout->addItem(mStack);
    layout->setAlignment(mStack, Qt::AlignHCenter|Qt::AlignVCenter);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);


    mState = StateOpening1;
    mAnimation = new QPropertyAnimation(this, "yRotation");
    mAnimation->setDuration(500);
    mAnimation->setStartValue(qreal(0));
    mAnimation->setEndValue(qreal(90));
    QObject::connect(mAnimation, SIGNAL(finished()), this, SLOT(animationFinished()));
    
    mAnimation->start();
        
}

void HgFlipWidget::togglePage()
{
    FUNC_LOG;
    
    mStack->setCurrentIndex(mStack->currentIndex() ^ 1);
}

void HgFlipWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    if (mIconLabel && event)
    {
        QSize iconSize(event->newSize().width(), event->newSize().height());
        mIconLabel->setIcon(HbIcon(mPixmap.scaled(iconSize, Qt::IgnoreAspectRatio)));
    }
    
}

void HgFlipWidget::setYRotation(qreal a)
{
    mYRotation = a;
    qreal s = 1.0f + (qAbs(a) / 90.0f * 0.5f);
    QTransform t;
    t.translate(rect().width()/2, rect().height()/2);
    t.scale(s, s);
    t.rotate(a, Qt::YAxis);
    t.translate(-rect().width()/2, -rect().height()/2);
    setTransform(t);
    mStack->setTransform(t);
}

qreal HgFlipWidget::yRotation()
{
    return mYRotation;
}

void HgFlipWidget::animationFinished()
{
    if (mState == StateOpening1)
    {
        mAnimation->setStartValue(qreal(-90));
        mAnimation->setEndValue(qreal(0));
        mAnimation->start();
        mState = StateOpening2;
        togglePage();
    }
    else if (mState == StateOpening2)
    {
        mState = StateOpened;
    }
    else if (mState == StateClosing1)
    {
        mAnimation->setStartValue(-90);
        mAnimation->setEndValue(0);
        mAnimation->start();
        mState = StateClosing2;
        togglePage();
    }
    else if (mState == StateClosing2)
    {
        mState = StateClosed;
        emit closed();
    }
}

void HgFlipWidget::close()
{
    if (mState == StateOpened)
    {
        mState = StateClosing1;
        mAnimation->setStartValue(0);
        mAnimation->setEndValue(90);
        mAnimation->start();
    }
}

void HgFlipWidget::buttonClicked(bool checked)
{
    Q_UNUSED(checked)
    
    close();
}
