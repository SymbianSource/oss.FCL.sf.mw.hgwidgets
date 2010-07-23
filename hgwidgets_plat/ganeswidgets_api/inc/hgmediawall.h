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

#ifndef HGMEDIAWALL_H
#define HGMEDIAWALL_H

#include <hgwidgets/hgwidgets.h>

class HgMediawallPrivate;

class HG_WIDGETS_EXPORT HgMediawall : public HgWidget
{
    Q_OBJECT
    Q_PROPERTY(HbFontSpec titleFontSpec READ titleFontSpec WRITE setTitleFontSpec)
    Q_PROPERTY(HbFontSpec descriptionFontSpec READ descriptionFontSpec WRITE setDescriptionFontSpec)
    Q_PROPERTY(QPointF frontItemPositionDelta READ frontItemPositionDelta WRITE setFrontItemPositionDelta)
    Q_PROPERTY(bool reflectionsEnabled READ reflectionsEnabled WRITE enableReflections)
    Q_PROPERTY(HgMediawall::TitleAndDescrVisibility titleAndDescriptionVisibility \
                READ titleAndDescriptionVisibility \
                WRITE setTitleAndDescriptionVisibility)
    Q_ENUMS(TitleAndDescrVisibility)

public:

    enum TitleAndDescrVisibility {
        TitleVisibilityBothVisible = 0,
        TitleVisibilityTitleVisible,
        TitleVisibilityDescriptionVisible,
        TitleVisibilityBothInvisible
    };

    explicit HgMediawall(QGraphicsItem *parent = 0 );
    virtual ~HgMediawall();

    void setTitleFontSpec(const HbFontSpec &fontSpec);
    HbFontSpec titleFontSpec() const;
    void setDescriptionFontSpec(const HbFontSpec &fontSpec);
    HbFontSpec descriptionFontSpec() const;

    void setTitleAndDescriptionVisibility(HgMediawall::TitleAndDescrVisibility visibility);
    HgMediawall::TitleAndDescrVisibility titleAndDescriptionVisibility() const;
    
    void setFrontItemPositionDelta(const QPointF& position);
    QPointF frontItemPositionDelta() const;

    void enableReflections(bool enabled);
    bool reflectionsEnabled() const;

signals:

    void animationAboutToEnd(const QModelIndex& targetIndex);

protected:
    HgMediawall(HgMediawallPrivate &dd, QGraphicsItem *parent);
    void polish(HbStyleParameters& params);

private:
    Q_DECLARE_PRIVATE_D(p_ptr, HgMediawall)
    Q_DISABLE_COPY(HgMediawall)
};

#endif  //HGMEDIAWALL_H

