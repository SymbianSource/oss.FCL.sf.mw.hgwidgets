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
* Description:
*
*/

#ifndef HGMEDIAWALL_P_H
#define HGMEDIAWALL_P_H

#include <hgwidgets/hgmediawall.h>
#include "hgwidgets_p.h"

class HgCoverflowContainer;
class HgCenterItemArea;
class HbTextItem;

class HgMediawallPrivate : public HgWidgetPrivate
{
    Q_DECLARE_PUBLIC(HgMediawall)

public:

    HgMediawallPrivate();
    virtual ~HgMediawallPrivate();

    void init(Qt::Orientation orientation);

    void setTitleFontSpec(const HbFontSpec &fontSpec);
    HbFontSpec titleFontSpec() const;
    void setDescriptionFontSpec(const HbFontSpec &fontSpec);
    HbFontSpec descriptionFontSpec() const;

    HgCoverflowContainer *container();
    const HgCoverflowContainer *container() const;

private: // From HgWidgetPrivate
    void updateCurrentItem(const QModelIndex &currentItem);
    void handleThemeChanged();
    
private:
    HbTextItem *mTitleItem;
    HbTextItem *mDescriptionItem;
    HgCenterItemArea *mCenterItemArea; // Dummy item for calculating correct position for center item
};

#endif  //HGMEDIAWALL_P_H

