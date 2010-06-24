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
#ifndef HGITEMSIZEDIALOG_H_
#define HGITEMSIZEDIALOG_H_

#include <HbDialog>
#include <HbLabel>
#include <HbWidget>
#include <HbSlider>

class HgItemSizeDialog : public HbDialog
{
    Q_OBJECT
public:
    
	HgItemSizeDialog(QSizeF size, QSizeF spacing, HbWidget *parent=0);
    
    QSizeF itemSize() const;
    QSizeF itemSpacing() const;
        
    void setSliderLimits(int min, int max);
    
signals:
    void updateItemSizeAndSpacing();
    void closed();
private slots:
    void itemSizeXChanged(int);
    void itemSizeYChanged(int);
    void itemSpacingXChanged(int);
    void itemSpacingYChanged(int);    
    void buttonClosed(bool);
private:
    HbLabel*                mSizeXLabel;
    HbLabel*                mSizeYLabel;
    HbLabel*                mSpacingXLabel;
    HbLabel*                mSpacingYLabel;
    QSizeF                  mSize;
    QSizeF                  mSpacing;
    HbSlider*               mSizeXSlider;
    HbSlider*               mSizeYSlider;
    HbSlider*               mSpacingXSlider;
    HbSlider*               mSpacingYSlider;
};

#endif /* HGTESTVIEW_H_ */
