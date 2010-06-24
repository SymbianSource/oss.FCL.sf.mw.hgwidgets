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
#include "hgitemsizedialog.h"

#include <qgraphicslinearlayout.h>
#include <qgraphicssceneresizeevent>
#include <HbSlider>
#include <HbPushButton>

#include "trace.h"



HgItemSizeDialog::HgItemSizeDialog(QSizeF size, QSizeF spacing, HbWidget *parent) : HbDialog(parent),
mSize(size),
mSpacing(spacing)
{    

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFlag(QGraphicsItem::ItemIsMovable);
    this->setModal(false);
    this->setBackgroundFaded(false);
    this->setDismissPolicy(HbPopup::TapOutside);
    this->setTimeout(5000);
    this->setMinimumSize(QSizeF(200, 200));
    
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    
    QSizeF itemSize = mSize;
    QSizeF itemSpacing = mSpacing;

    int maxVal = 500;
    if (parent)
        maxVal = qMax(parent->size().width(), parent->size().height());
            
    
    HbSlider* slider1 = new HbSlider(Qt::Horizontal);
    slider1->setMinimum(1);
    slider1->setMaximum(maxVal);
    slider1->setValue(itemSize.width());
    mSizeXSlider = slider1;
    
    HbSlider* slider2 = new HbSlider(Qt::Horizontal);
    slider2->setMinimum(1);
    slider2->setMaximum(maxVal);
    slider2->setValue(itemSize.height());
    mSizeYSlider = slider2;

    HbSlider* slider3 = new HbSlider(Qt::Horizontal);
    slider3->setMinimum(0);
    slider3->setMaximum(maxVal);
    slider3->setValue(itemSpacing.width());
    mSpacingXSlider = slider3;

    HbSlider* slider4 = new HbSlider(Qt::Horizontal);
    slider4->setMinimum(0);
    slider4->setMaximum(maxVal);
    slider4->setValue(itemSpacing.height());
    mSpacingYSlider = slider4;

    mSizeXLabel = new HbLabel();
    mSizeYLabel = new HbLabel();
    mSpacingXLabel = new HbLabel();
    mSpacingYLabel = new HbLabel();
    
    layout->addItem(mSizeXLabel);
    layout->addItem(slider1);
    layout->addItem(mSizeYLabel);
    layout->addItem(slider2);
    layout->addItem(mSpacingXLabel);
    layout->addItem(slider3);
    layout->addItem(mSpacingYLabel);
    layout->addItem(slider4);
    
    mSizeXLabel->setNumber(itemSize.width());
    mSizeYLabel->setNumber(itemSize.height());
    mSpacingXLabel->setNumber(itemSpacing.width());
    mSpacingYLabel->setNumber(itemSpacing.height());
    
    QObject::connect(slider1, SIGNAL(valueChanged(int)), this, SLOT(itemSizeXChanged(int)));
    QObject::connect(slider2, SIGNAL(valueChanged(int)), this, SLOT(itemSizeYChanged(int)));
    QObject::connect(slider3, SIGNAL(valueChanged(int)), this, SLOT(itemSpacingXChanged(int)));
    QObject::connect(slider4, SIGNAL(valueChanged(int)), this, SLOT(itemSpacingYChanged(int)));

    HbPushButton* button = new HbPushButton("Close");
    QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(buttonClosed(bool)));
    layout->addItem(button);

    this->setLayout(layout);
        
}

void HgItemSizeDialog::itemSizeXChanged(int value)
{
    QSizeF s = mSize;
    mSize = QSizeF(value, s.height());
    mSizeXLabel->setNumber(value);
    emit updateItemSizeAndSpacing();
}

void HgItemSizeDialog::itemSizeYChanged(int value)
{
    QSizeF s = mSize;
    mSize = QSizeF(s.width(), value);
    mSizeYLabel->setNumber(value);
    emit updateItemSizeAndSpacing();
}

void HgItemSizeDialog::itemSpacingXChanged(int value)
{
    QSizeF s = mSpacing;
    mSpacing = QSizeF(value, s.height());
    mSpacingXLabel->setNumber(value);
    emit updateItemSizeAndSpacing();
}

void HgItemSizeDialog::itemSpacingYChanged(int value)
{
    QSizeF s = mSpacing;
    mSpacing = QSizeF(s.width(), value);
    mSpacingYLabel->setNumber(value);
    emit updateItemSizeAndSpacing();
}

void HgItemSizeDialog::buttonClosed(bool)
{
    close();
    emit closed();
}

QSizeF HgItemSizeDialog::itemSize() const
{
    return mSize;
}

QSizeF HgItemSizeDialog::itemSpacing() const
{
    return mSpacing;
}

static void setSliderScale(HbSlider* slider, int min, int max)
{
    slider->setMinimum(min);
    slider->setMaximum(max);
}


void HgItemSizeDialog::setSliderLimits(int min, int max)
{
    setSliderScale(mSizeXSlider, min, max);
    setSliderScale(mSizeYSlider, min, max);
    setSliderScale(mSpacingXSlider, min, max);
    setSliderScale(mSpacingYSlider, min, max);
}
