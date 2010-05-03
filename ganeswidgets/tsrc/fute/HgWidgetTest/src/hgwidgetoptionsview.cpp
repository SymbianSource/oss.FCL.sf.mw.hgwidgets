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

#include <hbaction.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbabstractviewitem.h>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <QSettings>
#include "hgwidgetoptionsview.h"
#include "trace.h"

static const QString WIDGET_TYPE = "Widget type";
static const QString SCROLLBAR_VISIBILITY = "Scrollbar visibility";
static const QString INTERACTIVE_SCROLLBAR = "Interactive scrollbar";
static const QString MODEL_IMAGE_TYPE = "Datamodel image type";
static const QString WIDGET_HEIGHT = "Widget height";
static const QString WIDGET_WIDTH = "Widget width";
static const QString LOW_RES_IMAGES = "Use low res images";
static const QString TITLE_POSITION = "Title position";
static const QString TITLE_FONT = "Title font";
static const QString DESCRIPTION_POSITION = "Description position";
static const QString DESCRIPTION_FONT = "Description font";
static const QString REFLECTIONS_ENABLED = "Reflections enabled";

enum DataFormItems {
    ItemWidgetType = 0,
    ItemScrollBarVisibility,
    ItemScrollBarInteractivity,
    ItemModelImageType,
    ItemWidgetHeight,
    ItemWidgetWidth,
    ItemLowResImages,
    ItemTitlePosition,
    ItemTitleFont,
    ItemDescriptionPosition,
    ItemDescriptionFont,
    ItemReflectionEnabled,
};

HgWidgetOptionsView::HgWidgetOptionsView(QGraphicsItem *parent) :
    HbView(parent),
    mForm(new HbDataForm(this)),
    mModel(new HbDataFormModel(this)),
    mContentReady(false),
    mUpdateWidgetSize(true)
{
    HbAction *backAction = new HbAction(Hb::BackNaviAction);
    connect(backAction, SIGNAL(triggered()), SIGNAL(optionsClosed()));
    setNavigationAction(backAction);

    HbDataFormModelItem *item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, WIDGET_TYPE);
    item->setContentWidgetData(QString("items"), QStringList("Grid") << "Coverflow" << "T-Bone");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, SCROLLBAR_VISIBILITY);
    item->setContentWidgetData(QString("items"), QStringList("Autohide") << "Always on" << "Always off");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ToggleValueItem, INTERACTIVE_SCROLLBAR);
    item->setContentWidgetData(QString("text"), QString("no"));
    item->setContentWidgetData(QString("additionalText"), QString("yes"));

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, MODEL_IMAGE_TYPE);
    item->setContentWidgetData(QString("items"), QStringList("QImage") << "HbIcon" << "QIcon");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, WIDGET_HEIGHT);
    item->setContentWidgetData(QString("text"), QString("0"));
    item->setContentWidgetData(QString("inputMethodHints"), Qt::ImhDigitsOnly);

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::TextItem, WIDGET_WIDTH);
    item->setContentWidgetData(QString("text"), QString("0"));
    item->setContentWidgetData(QString("inputMethodHints"), Qt::ImhDigitsOnly);

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ToggleValueItem, LOW_RES_IMAGES);
    item->setContentWidgetData(QString("text"), QString("no"));
    item->setContentWidgetData(QString("additionalText"), QString("yes"));

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, TITLE_POSITION);
    item->setContentWidgetData(QString("items"), QStringList("Hidden") << "Above" << "Below");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, TITLE_FONT);
    item->setContentWidgetData(QString("items"), QStringList("Primary") << "Secondary" << "Title" << "Primary small" << "Digital");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, DESCRIPTION_POSITION);
    item->setContentWidgetData(QString("items"), QStringList("Hidden") << "Above" << "Below");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, DESCRIPTION_FONT);
    item->setContentWidgetData(QString("items"), QStringList("Primary") << "Secondary" << "Title" << "Primary small" << "Digital");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ToggleValueItem, REFLECTIONS_ENABLED);
    item->setContentWidgetData(QString("text"), QString("no"));
    item->setContentWidgetData(QString("additionalText"), QString("yes"));

    connect(mModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), SLOT(updateData(QModelIndex, QModelIndex)));
    mForm->setModel(mModel);

    setupData();
    mContentReady = true;

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->addItem(mForm);
    setLayout(layout);
}

HgWidgetOptionsView::~HgWidgetOptionsView()
{
}

void HgWidgetOptionsView::updateData(QModelIndex startIn, QModelIndex endIn)
{
    Q_UNUSED(endIn);

    if (!mContentReady) return;

    QSettings settings(SETT_ORGANIZATION, SETT_APPLICATION);

    HbDataFormModelItem *item = mModel->itemFromIndex(startIn);
    if (item->data(HbDataFormModelItem::LabelRole).toString() == WIDGET_TYPE) {
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HgTestWidgetType type = HgWidgetNone;
        switch (index) {
            case 0:
                type = HgWidgetGrid;
                break;
            case 1:
                type = HgWidgetCoverflow;
                break;
            case 2:
                type = HgWidgetTBone;
                break;
            default: break;
        }

        setCoverflowEnabled(index == 1 || index  == 2);
        settings.setValue(SETT_WIDGET_TYPE, type);
        emit widgetTypeChanged(type);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == SCROLLBAR_VISIBILITY) {
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HgWidget::ScrollBarPolicy visibility = HgWidget::ScrollBarAutoHide;
        switch (index) {
            case 0:
                visibility = HgWidget::ScrollBarAutoHide;
                break;
            case 1:
                visibility = HgWidget::ScrollBarAlwaysOn;
                break;
            case 2:
                visibility = HgWidget::ScrollBarAlwaysOff;
                break;
            default: break;
        }

        settings.setValue(SETT_SCROLLBAR_VISIBILITY, visibility);
        emit scrollBarVisibilityChanged(visibility);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == INTERACTIVE_SCROLLBAR) {
        QVariant data = item->contentWidgetData(QString("text"));
        bool value = data.toString() == "yes";
        settings.setValue(SETT_SCROLLBAR_INTERACTIVITY, value);
        if (value) {
            item = mModel->itemFromIndex(mModel->index(ItemScrollBarVisibility, 0));
            if (item) {
                item->setContentWidgetData(QString("currentIndex"), 1); // Set scroll bar always on
            }
        }
        emit scrollBarInteractivityChanged(value);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == MODEL_IMAGE_TYPE) {
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HgTestImageType type = ImageTypeNone;
        switch (index) {
            case 0:
                type = ImageTypeQImage;
                break;
            case 1:
                type = ImageTypeHbIcon;
                break;
            case 2:
                type = ImageTypeQIcon;
                break;
            default: break;
        }

        settings.setValue(SETT_MODEL_IMAGE_TYPE, type);
        emit imageTypeChanged(type);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == WIDGET_HEIGHT) {
        emit widgetHeightChanged(item->contentWidgetData(QString("text")).toInt());
        storeWidgetSize();
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == WIDGET_WIDTH) {
        emit widgetWidthChanged(item->contentWidgetData(QString("text")).toInt());
        storeWidgetSize();

    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == LOW_RES_IMAGES) {
        QVariant data = item->contentWidgetData(QString("text"));
        bool value = data.toString() == "yes";
        settings.setValue(SETT_LOW_RES_IMAGES, value);
        emit lowResImageUseChanged(value);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == TITLE_POSITION) {
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HgMediawall::LabelPosition position = HgMediawall::PositionNone;
        switch (index) {
            case 0:
                position = HgMediawall::PositionNone;
                break;
            case 1:
                position = HgMediawall::PositionAboveImage;
                break;
            case 2:
                position = HgMediawall::PositionBelowImage;
                break;
            default: break;
        }

        settings.setValue(SETT_TITLE_POSITION, position);
        emit titlePositionChanged(position);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == TITLE_FONT) {
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HbFontSpec::Role role = HbFontSpec::Undefined;
        switch (index) {
            case 0:
                role = HbFontSpec::Primary;
                break;
            case 1:
                role = HbFontSpec::Secondary;
                break;
            case 2:
                role = HbFontSpec::Title;
                break;
            case 3:
                role = HbFontSpec::PrimarySmall;
                break;
            case 4:
                role = HbFontSpec::Digital;
                break;
            default: break;
        }
        settings.setValue(SETT_TITLE_FONT, role);
        emit titleFontChanged(HbFontSpec(role));
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == DESCRIPTION_POSITION) {
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HgMediawall::LabelPosition position = HgMediawall::PositionNone;
        switch (index) {
            case 0:
                position = HgMediawall::PositionNone;
                break;
            case 1:
                position = HgMediawall::PositionAboveImage;
                break;
            case 2:
                position = HgMediawall::PositionBelowImage;
                break;
            default: break;
        }

        settings.setValue(SETT_DESCRIPTION_POSITION, position);
        emit descriptionPositionChanged(position);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == DESCRIPTION_FONT) {
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HbFontSpec::Role role = HbFontSpec::Undefined;
        switch (index) {
            case 0:
                role = HbFontSpec::Primary;
                break;
            case 1:
                role = HbFontSpec::Secondary;
                break;
            case 2:
                role = HbFontSpec::Title;
                break;
            case 3:
                role = HbFontSpec::PrimarySmall;
                break;
            case 4:
                role = HbFontSpec::Digital;
                break;
            default: break;
        }
        settings.setValue(SETT_DESCRIPTION_FONT, role);
        emit descriptionFontChanged(HbFontSpec(role));
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == REFLECTIONS_ENABLED) {
        QVariant data = item->contentWidgetData(QString("text"));
        bool value = data.toString() == "yes";
        settings.setValue(SETT_REFLECTIONS_ENABLED, value);
        emit reflectionsEnabledChanged(value);
    }
}

void HgWidgetOptionsView::setCoverflowEnabled(bool value)
{
    HbAbstractViewItem *item = mForm->itemByIndex(mModel->index(ItemLowResImages, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemTitlePosition, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemTitleFont, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemDescriptionPosition, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemDescriptionFont, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemReflectionEnabled, 0));
    if (item) item->setEnabled(value);    
}

void HgWidgetOptionsView::storeWidgetSize()
{
    QSettings settings(SETT_ORGANIZATION, SETT_APPLICATION);

    HbDataFormModelItem *item = item = mModel->itemFromIndex(mModel->index(ItemWidgetHeight, 0));
    if (item) {
        int value = item->contentWidgetData(QString("text")).toInt();
        settings.setValue(SETT_WIDGET_HEIGHT, value);
    }
    item = mModel->itemFromIndex(mModel->index(ItemWidgetWidth, 0));
    if (item) {
        int value = item->contentWidgetData(QString("text")).toInt();
        settings.setValue(SETT_WIDGET_WIDTH, value);
    }

    mUpdateWidgetSize = false;
}

void HgWidgetOptionsView::setupData()
{
    FUNC_LOG;

    QSettings settings(SETT_ORGANIZATION, SETT_APPLICATION);

    HbDataFormModelItem *item = mModel->itemFromIndex(mModel->index(ItemWidgetType, 0));
    QVariant value = settings.value(SETT_WIDGET_TYPE);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case HgWidgetGrid:
                index = 0;
                break;
            case HgWidgetCoverflow:
                index = 1;
                break;
            case HgWidgetTBone:
                index = 2;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
        setCoverflowEnabled(index == 1 || index  == 2);
    }

    item = mModel->itemFromIndex(mModel->index(ItemScrollBarVisibility, 0));
    value = settings.value(SETT_SCROLLBAR_VISIBILITY);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case HgWidget::ScrollBarAutoHide:
                index = 0;
                break;
            case HgWidget::ScrollBarAlwaysOn:
                index = 1;
                break;
            case HgWidget::ScrollBarAlwaysOff:
                index = 2;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
    }

    item = mModel->itemFromIndex(mModel->index(ItemScrollBarInteractivity, 0));
    value = settings.value(SETT_SCROLLBAR_INTERACTIVITY);
    if (item && value.isValid()) {
        item->setContentWidgetData(QString("text"), value.toBool() ? "yes" : "no");
        item->setContentWidgetData(QString("alternateText"), value.toBool() ? "no" : "yes");
    }

    item = mModel->itemFromIndex(mModel->index(ItemModelImageType, 0));
    value = settings.value(SETT_MODEL_IMAGE_TYPE);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case ImageTypeQImage:
                value = 0;
                break;
            case ImageTypeHbIcon:
                value = 1;
                break;
            case ImageTypeQIcon:
                value = 2;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
    }

    item = mModel->itemFromIndex(mModel->index(ItemWidgetHeight, 0));
    value = settings.value(SETT_WIDGET_HEIGHT);
    if (item && value.isValid()) {
        item->setContentWidgetData(QString("text"), value.toInt());
        mUpdateWidgetSize = false;
    }

    item = mModel->itemFromIndex(mModel->index(ItemWidgetWidth, 0));
    value = settings.value(SETT_WIDGET_WIDTH);
    if (item && value.isValid()) {
        item->setContentWidgetData(QString("text"), value.toInt());
        mUpdateWidgetSize = false;
    }

    item = mModel->itemFromIndex(mModel->index(ItemLowResImages, 0));
    value = settings.value(SETT_LOW_RES_IMAGES);
    if (item && value.isValid()) {
        item->setContentWidgetData(QString("text"), value.toBool() ? "yes" : "no");
        item->setContentWidgetData(QString("alternateText"), value.toBool() ? "no" : "yes");
    }

    item = mModel->itemFromIndex(mModel->index(ItemTitlePosition, 0));
    value = settings.value(SETT_TITLE_POSITION);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case HgMediawall::PositionNone:
                index = 0;
                break;
            case HgMediawall::PositionAboveImage:
                index = 1;
                break;
            case HgMediawall::PositionBelowImage:
                index = 2;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
    }
    else if (item) {
        item->setContentWidgetData(QString("currentIndex"), 1);
    }

    item = mModel->itemFromIndex(mModel->index(ItemTitleFont, 0));
    value = settings.value(SETT_TITLE_FONT);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case HbFontSpec::Primary:
                index = 0;
                break;
            case HbFontSpec::Secondary:
                index = 1;
                break;
            case HbFontSpec::Title:
                index = 2;
                break;
            case HbFontSpec::PrimarySmall:
                index = 3;
                break;
            case HbFontSpec::Digital:
                index = 4;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
    }

    item = mModel->itemFromIndex(mModel->index(ItemDescriptionPosition, 0));
    value = settings.value(SETT_DESCRIPTION_POSITION);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case HgMediawall::PositionNone:
                index = 0;
                break;
            case HgMediawall::PositionAboveImage:
                index = 1;
                break;
            case HgMediawall::PositionBelowImage:
                index = 2;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
    }

    item = mModel->itemFromIndex(mModel->index(ItemDescriptionFont, 0));
    value = settings.value(SETT_DESCRIPTION_FONT);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case HbFontSpec::Primary:
                index = 0;
                break;
            case HbFontSpec::Secondary:
                index = 1;
                break;
            case HbFontSpec::Title:
                index = 2;
                break;
            case HbFontSpec::PrimarySmall:
                index = 3;
                break;
            case HbFontSpec::Digital:
                index = 4;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
    }

    item = mModel->itemFromIndex(mModel->index(ItemReflectionEnabled, 0));
    value = settings.value(SETT_REFLECTIONS_ENABLED);
    if (item && value.isValid()) {
        item->setContentWidgetData(QString("text"), value.toBool() ? "yes" : "no");
        item->setContentWidgetData(QString("alternateText"), value.toBool() ? "no" : "yes");
    }

}

void HgWidgetOptionsView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    if (mUpdateWidgetSize) {
        mContentReady = false;
        HbDataFormModelItem *item = item = mModel->itemFromIndex(mModel->index(ItemWidgetHeight, 0));
        if (item) {
            item->setContentWidgetData(QString("text"), event->newSize().height());
        }

        item = mModel->itemFromIndex(mModel->index(ItemWidgetWidth, 0));
        if (item) {
            item->setContentWidgetData(QString("text"), event->newSize().width());
        }
        mContentReady = true;
    }
}
