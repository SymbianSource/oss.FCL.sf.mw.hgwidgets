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

#include <HbAction>
#include <HbDataform>
#include <HbDataformModel>
#include <HbAbstractViewItem>
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
static const QString TITLE_FONT = "Title font";
static const QString DESCRIPTION_FONT = "Description font";
static const QString REFLECTIONS_ENABLED = "Reflections enabled";
static const QString EFFECT3D_ENABLED = "Grid 3d effects enabled";
static const QString ITEM_SIZE_POLICY = "Item size policy";
static const QString TITLE_DESCR_VISIBILITY = "Title/description visibility";

enum DataFormItems {
    ItemWidgetType = 0,
    ItemScrollBarVisibility,
    ItemScrollBarInteractivity,
    ItemModelImageType,
    ItemWidgetHeight,
    ItemWidgetWidth,
    ItemLowResImages,
    ItemTitleFont,
    ItemDescriptionFont,
    ItemReflectionEnabled,
    ItemEffect3dEnabled,
    ItemItemSizePolicy,
    ItemTitleDescrVisibility
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
    item->setContentWidgetData(QString("items"), QStringList("QImage") << "HbIcon" << "QIcon" << "QPixmap");

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
        HbDataFormModelItem::ComboBoxItem, TITLE_FONT);
    item->setContentWidgetData(QString("items"), QStringList("Primary") << "Secondary" << "Title" << "Primary small" << "Digital");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, DESCRIPTION_FONT);
    item->setContentWidgetData(QString("items"), QStringList("Primary") << "Secondary" << "Title" << "Primary small" << "Digital");

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ToggleValueItem, REFLECTIONS_ENABLED);
    item->setContentWidgetData(QString("text"), QString("no"));
    item->setContentWidgetData(QString("additionalText"), QString("yes"));

    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ToggleValueItem, EFFECT3D_ENABLED);
    item->setContentWidgetData(QString("text"), QString("no"));
    item->setContentWidgetData(QString("additionalText"), QString("yes"));
    
    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ToggleValueItem, ITEM_SIZE_POLICY);
    item->setContentWidgetData(QString("text"), QString("User defined"));
    item->setContentWidgetData(QString("additionalText"), QString("Automatic"));
    
    item = mModel->appendDataFormItem(
        HbDataFormModelItem::ComboBoxItem, TITLE_DESCR_VISIBILITY);
    item->setContentWidgetData(QString("items"), QStringList("Both visible") << "Title visible" << "Description visible" << "Both invisible");
	
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
    if( !item ) return;
    
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
            case 3:
                type = ImageTypeQPixmap;
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
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == EFFECT3D_ENABLED) {
        QVariant data = item->contentWidgetData(QString("text"));
        bool value = data.toString() == "yes";
        settings.setValue(SETT_EFFECT3D_ENABLED, value);
        emit effect3dEnabledChanged(value);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == ITEM_SIZE_POLICY) {
        QVariant data = item->contentWidgetData(QString("text"));
        HgWidget::ItemSizePolicy value = (data.toString() == "Automatic" ? HgWidget::ItemSizeAutomatic : HgWidget::ItemSizeUserDefined);
        settings.setValue(SETT_ITEM_SIZE_POLICY, value);
        emit itemSizePolicyChanged(value);
    }
    else if (item->data(HbDataFormModelItem::LabelRole).toString() == TITLE_DESCR_VISIBILITY) {
                
        int index = item->contentWidgetData(QString("currentIndex")).toInt();
        HgMediawall::TitleAndDescrVisibility visibility = HgMediawall::TitleVisibilityBothVisible;
        switch (index) {
            case 0:
                visibility = HgMediawall::TitleVisibilityBothVisible;
                break;
            case 1:
                visibility = HgMediawall::TitleVisibilityTitleVisible;
                break;
            case 2:
                visibility = HgMediawall::TitleVisibilityDescriptionVisible;
                break;
            case 3:
                visibility = HgMediawall::TitleVisibilityBothInvisible;
                break;
            default: break;
        }
        settings.setValue(SETT_TITLE_DESCR_VISIBILITY, visibility);
        emit titleAndDescrVisibilityChanged(visibility);
    }
}

void HgWidgetOptionsView::setCoverflowEnabled(bool value)
{
    HbAbstractViewItem *item = mForm->itemByIndex(mModel->index(ItemLowResImages, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemTitleFont, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemDescriptionFont, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemReflectionEnabled, 0));
    if (item) item->setEnabled(value);
    item = mForm->itemByIndex(mModel->index(ItemItemSizePolicy, 0));
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
        item->setContentWidgetData(QString("additionalText"), value.toBool() ? "no" : "yes");
    }

    item = mModel->itemFromIndex(mModel->index(ItemModelImageType, 0));
    value = settings.value(SETT_MODEL_IMAGE_TYPE);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case ImageTypeQImage:
                index = 0;
                break;
            case ImageTypeHbIcon:
                index = 1;
                break;
            case ImageTypeQIcon:
                index = 2;
                break;
            case ImageTypeQPixmap:
                index = 3;
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
        item->setContentWidgetData(QString("additionalText"), value.toBool() ? "no" : "yes");
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
        item->setContentWidgetData(QString("additionalText"), value.toBool() ? "no" : "yes");
    }

    item = mModel->itemFromIndex(mModel->index(ItemEffect3dEnabled, 0));
    value = settings.value(SETT_EFFECT3D_ENABLED);
    if (item && value.isValid()) {
        item->setContentWidgetData(QString("text"), value.toBool() ? "yes" : "no");
        item->setContentWidgetData(QString("additionalText"), value.toBool() ? "no" : "yes");
    }
    
    item = mModel->itemFromIndex(mModel->index(ItemItemSizePolicy, 0));
    value = settings.value(SETT_ITEM_SIZE_POLICY);
    if (item && value.isValid()) {
        if (value.toInt() == HgWidget::ItemSizeAutomatic) {
            item->setContentWidgetData(QString("text"), "Automatic");
            item->setContentWidgetData(QString("additionalText"), "User defined");
        }
        else {
            item->setContentWidgetData(QString("text"), "User defined");
            item->setContentWidgetData(QString("additionalText"), "Automatic");
        }
    }
    
    item = mModel->itemFromIndex(mModel->index(ItemTitleDescrVisibility, 0));
    value = settings.value(SETT_TITLE_DESCR_VISIBILITY);
    if (item && value.isValid()) {
        int index(0);
        switch (value.toInt()) {
            case HgMediawall::TitleVisibilityBothVisible:
                qDebug() << "JARI_DEBUG, HgWidgetOptionsView::setupData(): SETT_TITLE_DESCR_VISIBILITY: TitleVisibilityBothVisible";
                index = 0;
                break;
            case HgMediawall::TitleVisibilityTitleVisible:
                qDebug() << "JARI_DEBUG, HgWidgetOptionsView::setupData(): SETT_TITLE_DESCR_VISIBILITY: TitleVisibilityTitleVisible";
                index = 1;
                break;
            case HgMediawall::TitleVisibilityDescriptionVisible:
                qDebug() << "JARI_DEBUG, HgWidgetOptionsView::setupData(): SETT_TITLE_DESCR_VISIBILITY: TitleVisibilityDescriptionVisible";
                index = 2;
                break;
            case HgMediawall::TitleVisibilityBothInvisible:
                qDebug() << "JARI_DEBUG, HgWidgetOptionsView::setupData(): SETT_TITLE_DESCR_VISIBILITY: TitleVisibilityBothInvisible";
                index = 3;
                break;
            default: break;
        }
        item->setContentWidgetData(QString("currentIndex"), index);
    }
}

void HgWidgetOptionsView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    FUNC_LOG;

    if (mUpdateWidgetSize) {
        mContentReady = false;
        HbDataFormModelItem *item = mModel->itemFromIndex(mModel->index(ItemWidgetHeight, 0));
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
