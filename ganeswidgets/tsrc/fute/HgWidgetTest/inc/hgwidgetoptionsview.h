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

#ifndef HGWIDGETOPTIONSVIEW_H_
#define HGWIDGETOPTIONSVIEW_H_

#include <hbview.h>
#include <hgwidgets/hgmediawall.h>
#include "hgtestdefs.h"

class HbDataForm;
class HbDataFormModel;

class HgWidgetOptionsView : public HbView
{
    Q_OBJECT

public:

    HgWidgetOptionsView(QGraphicsItem *parent = 0);
    ~HgWidgetOptionsView();

signals:
    void optionsClosed();
    void widgetTypeChanged(HgTestWidgetType);
    void scrollBarVisibilityChanged(HgWidget::ScrollBarPolicy);
    void scrollBarInteractivityChanged(bool);
    void imageTypeChanged(HgTestImageType);
    void lowResImageUseChanged(bool);
    void titlePositionChanged(HgMediawall::LabelPosition);
    void titleFontChanged(const HbFontSpec &);
    void descriptionPositionChanged(HgMediawall::LabelPosition);
    void descriptionFontChanged(const HbFontSpec &);
    void widgetHeightChanged(int);
    void widgetWidthChanged(int);
    void reflectionsEnabledChanged(bool);
    void itemSizePolicyChanged(HgWidget::ItemSizePolicy);
private slots:
    void updateData(QModelIndex startIn, QModelIndex endIn);

private:
    void setCoverflowEnabled(bool value);
    void storeWidgetSize();
    void setupData();
    void resizeEvent(QGraphicsSceneResizeEvent *event);

private: // data
    HbDataForm *mForm;
    HbDataFormModel *mModel;
    bool mContentReady;
    bool mUpdateWidgetSize;
};


#endif /* HGWIDGETOPTIONSVIEW_H_ */
