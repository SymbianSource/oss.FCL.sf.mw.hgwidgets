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

#ifndef HGTESTDEFS_H_
#define HGTESTDEFS_H_

enum HgTestWidgetType {
    HgWidgetNone = 0,
    HgWidgetGrid,
    HgWidgetCoverflow,
    HgWidgetTBone
};

enum HgTestImageType {
    ImageTypeNone = 0,
    ImageTypeQIcon,
    ImageTypeHbIcon,
    ImageTypeQImage,
    ImageTypeQPixmap
};

static const QString SETT_ORGANIZATION = "Nokia";
static const QString SETT_APPLICATION = "HgWidgetTestApp";
static const QString SETT_WIDGET_TYPE = "WidgetType";
static const QString SETT_SCROLLBAR_VISIBILITY = "ScrollBarVisibility";
static const QString SETT_SCROLLBAR_INTERACTIVITY = "ScrollBarInteractivity";
static const QString SETT_MODEL_IMAGE_TYPE = "ModelImageType";
static const QString SETT_WIDGET_HEIGHT = "WidgetHeight";
static const QString SETT_WIDGET_WIDTH = "WidgetWidth";
static const QString SETT_LOW_RES_IMAGES = "LowResImages";
static const QString SETT_TITLE_POSITION = "TitlePosition";
static const QString SETT_TITLE_FONT = "TitleFont";
static const QString SETT_DESCRIPTION_POSITION = "DescriptionPosition";
static const QString SETT_DESCRIPTION_FONT = "DescriptionFont";
static const QString SETT_REFLECTIONS_ENABLED = "Reflections enabled";
static const QString SETT_ITEM_SIZE_POLICY = "ItemSizePolicy";

#endif /* HGTESTDEFS_H_ */
