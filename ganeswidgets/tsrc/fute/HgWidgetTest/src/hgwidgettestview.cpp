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
#include <HbDialog>
#include <HbInstance>
#include <HbLabel>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbMenu>
#include <HbScrollbar>
#include <QActionGroup>
#include <QGraphicsLinearLayout>
#include <QSettings>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include "hgwidgettestview.h"
#include "hgtestview.h"
#include "hgwidgetoptionsview.h"
#include "hgselectiondialog.h"
#include "hgwidgettestdatamodel.h"
#include "hgflipwidget.h"
#include "hgitemsizedialog.h"
#include "trace.h"
#include "hgcoveritem.h"
#include <hgwidgets/hggrid.h>
#include <hgwidgets/hgmediawall.h>
#include <HbStyleLoader>

static const int GRIDBUFFERSIZE(400);
static const int COVERFLOWBUFFERSIZE(100);

HgWidgetTestView::HgWidgetTestView(QGraphicsItem *parent) :
    HbView(parent),
    mWidget(0),
    mModel(0),
    mWidgetType(HgWidgetNone),
    mListWidget(0),
    mToggleOrientation(false),
    mFlipWidget(0),
    mFrontItem(0),
    mOptionsView(0),
    mItemSizeDialog(0),
    mItemPosDialog(0),
    mAnimationGroup(0),
    mDialog(0),
    mCoverItem(0)
{    
    mModel = new HgWidgetTestDataModel(this);
    mSelectionModel = new QItemSelectionModel(mModel, this);

    mAnimationGroup = new QParallelAnimationGroup();
    
    createMenu();

    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mLayout->setContentsMargins(0,0,0,0);
    setupWidget();
    setLayout(mLayout);
}

HgWidgetTestView::~HgWidgetTestView()
{
    delete mAnimationGroup;
    HbStyleLoader::unregisterFilePath(":/test/hgmediawall.css");
    HbStyleLoader::unregisterFilePath(":/test/hgmediawall.widgetml");
}

void HgWidgetTestView::createMenu()
{
    FUNC_LOG;

    menu()->addAction("Options", this, SLOT(showOptions()));
    menu()->addAction("Reset Options", this, SLOT(resetOptions()));
    menu()->addAction("Toggle scrolldirection", this, SLOT(toggleScrollDirection()));
    menu()->addAction("Simulate orientation switch", this, SLOT(orientationChanged()));
    menu()->addAction("Edit item size", this, SLOT(startItemSizeChange()));
    menu()->addAction("Edit item pos", this, SLOT(startItemPosChange()));
    menu()->addAction("use custom layout", this, SLOT(useCustomLayout()));
        
    HbMenu *modelChangeSubMenu = menu()->addMenu("Change model");
    modelChangeSubMenu->addAction("Remove items", this, SLOT(openDeleteItemsDialog()));
    modelChangeSubMenu->addAction("Move items", this, SLOT(openMoveItemsDialog()));
    modelChangeSubMenu->addAction("Add items", this, SLOT(openAddItemsDialog()));
    modelChangeSubMenu->addAction("Reset model", this, SLOT(resetModel()));
}

void HgWidgetTestView::toggleScrollDirection()
{
    FUNC_LOG;

    mToggleOrientation = !mToggleOrientation;
    initWidget(mWidgetType);
}

void HgWidgetTestView::initWidget(HgTestWidgetType type)
{
    FUNC_LOG;

    if (mWidgetType != type) {
        mWidgetType = type;

        // TODO, disconnecting signals required?

        if( mWidget )
            mLayout->removeItem(mWidget);
        if( mListWidget )
            mLayout->removeItem(mListWidget);

        delete mWidget;
        mWidget = NULL;

        delete mListWidget;
        mListWidget = NULL;

        mWidget = createWidget(type);
        mLayout->addItem(mWidget);

        switch (type) {
            case HgWidgetGrid:
            	setItemVisible(Hb::AllItems, true);
                mModel->enableLowResImages(false);
                // TODO, init grid different model,
//                mModel->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
                break;
            case HgWidgetCoverflow:
//                mModel->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
                setItemVisible(Hb::AllItems, orientation() != Qt::Horizontal);
                break;
            case HgWidgetTBone:
                {
            	setItemVisible(Hb::AllItems, true);
//                mModel->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
                mListWidget = new HbListWidget;
                mListWidget->setMinimumHeight(360);
                mListWidget->setMaximumHeight(360);
                mListWidget->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
                mLayout->addItem(mListWidget);
                /*mListWidget->addItem( "List item 1");
                mListWidget->addItem( "List item 2");
                mListWidget->addItem( "List item 3");*/
                
                HgMediawall* mediawall = qobject_cast<HgMediawall*>( mWidget );
                if (mediawall) {
                    connect(mediawall, SIGNAL(animationAboutToEnd(QModelIndex)),
                            SLOT(animationAboutToEnd(QModelIndex)));
                }
                }
                break;
            default:
                break;
        }

        HANDLE_ERROR_NULL(mWidget);
        connect(mWidget, SIGNAL(activated(QModelIndex)), SLOT(openDialog(QModelIndex)));
        connect(mWidget, SIGNAL(longPressed(QModelIndex, QPointF)), SLOT(openView(QModelIndex)));
        QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
        if (mainWindows.count() > 0) {
            HbMainWindow *primaryWindow = mainWindows[0];
            connect(primaryWindow, SIGNAL(orientationChanged(Qt::Orientation)), mWidget, SLOT(orientationChanged(Qt::Orientation)));
            connect(primaryWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
            primaryWindow->setAutomaticOrientationEffectEnabled(false);
        }
        setupWidgetOptions();
        setupWidgetSize();

        connect(mWidget, SIGNAL(scrollingStarted()), SLOT(onScrollingStarted()));
        connect(mWidget, SIGNAL(scrollingEnded()), SLOT(onScrollingEnded()));
    }
}

void HgWidgetTestView::animationAboutToEnd(const QModelIndex& targetIndex)
{
    FUNC_LOG;
    
    QVariant texts = mModel->silentData(targetIndex, Qt::DisplayRole);
    if (texts.canConvert<QStringList>()) {
        QStringList strList = texts.toStringList();
        if (strList.count() > 0) {
            if( mListWidget->count() == 1 ) {
                mListWidget->setText(0,strList.at(0));
            } else {
                mListWidget->insertItem(0, strList.at(0));
            }
        }
    }
}

void HgWidgetTestView::changeScrollBarVisibility(HgWidget::ScrollBarPolicy policy)
{
    FUNC_LOG;

    if (mWidget->scrollBarPolicy() != policy) {
        mWidget->setScrollBarPolicy(policy);
    }
}

void HgWidgetTestView::changeScrollBarInteractivity(bool value)
{
    FUNC_LOG;

    if (mWidget->scrollBar()->isInteractive() != value) {
        mWidget->scrollBar()->setInteractive(value);

        if (mWidgetType == HgWidgetCoverflow || mWidgetType == HgWidgetTBone) {
            mWidget->setIndexFeedbackPolicy(HgWidget::IndexFeedbackSingleCharacter);
        }
    }
}

void HgWidgetTestView::changeModelImageType(HgTestImageType type)
{
    FUNC_LOG;

    if (mModel->imageDataType() != type) {
        mModel->setImageDataType(type);
    }
}

void HgWidgetTestView::changeLowResImageUse(bool value)
{
    FUNC_LOG;

    if (mWidgetType == HgWidgetCoverflow || mWidgetType == HgWidgetTBone) {
        mModel->enableLowResImages(value);
        initWidget(mWidgetType);
    }
}

void HgWidgetTestView::changeTitleFont(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall && mediawall->titleFontSpec() != fontSpec) {
        mediawall->setTitleFontSpec(fontSpec);
    }
}

void HgWidgetTestView::changeDescriptionFont(const HbFontSpec &fontSpec)
{
    FUNC_LOG;

    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall && mediawall->descriptionFontSpec() != fontSpec) {
        mediawall->setDescriptionFontSpec(fontSpec);
    }
}

void HgWidgetTestView::changeWidgetHeight(int value)
{
    FUNC_LOG;

    int height(rect().height());
    if (value < height) {
        qreal left(0), top(0), right(0), bottom(0);
        mLayout->getContentsMargins(&left, &top, &right, &bottom);

        if (mWidgetType == HgWidgetTBone) {
            mListWidget->setMinimumHeight(height-value);
            mListWidget->setMaximumHeight(height-value);
            mLayout->setContentsMargins(left, 0, right, 0);
        }
        else {
            int margin(0);
            margin = (height-value)/2;
            mLayout->setContentsMargins(left, margin, right, margin);
        }
    }
}

void HgWidgetTestView::changeWidgetWidth(int value)
{
    FUNC_LOG;

    int width(rect().width());
    int margin(0);
    if (value < width) {
        margin = (width-value)/2;
    }

    qreal left(0), top(0), right(0), bottom(0);
    mLayout->getContentsMargins(&left, &top, &right, &bottom);
    mLayout->setContentsMargins(margin, top, margin, bottom);
}

void HgWidgetTestView::changeReflectionsEnabled(bool enabled)
{
    HgMediawall* wall = qobject_cast<HgMediawall*>(mWidget);
    if (wall)
        wall->enableReflections(enabled);
}

void HgWidgetTestView::changeEffect3dEnabled(bool enabled)
{
    HgGrid* grid = qobject_cast<HgGrid*>(mWidget);
    if (grid)
        grid->setEffect3dEnabled(enabled);
}

void HgWidgetTestView::changeItemSizePolicy(HgMediawall::ItemSizePolicy policy)
{
    FUNC_LOG;

    if (mWidget && mWidget->itemSizePolicy() != policy) {
        mWidget->setItemSizePolicy(policy);
    }
}

void HgWidgetTestView::activated()
{
    mAnimationGroup->setDirection(QAbstractAnimation::Backward);
    connect(mAnimationGroup, SIGNAL(finished()), this, SLOT(animationFinished()));
    mAnimationGroup->start();
}

void HgWidgetTestView::animationFinished()
{
    delete mCoverItem;
    mCoverItem = 0;
    mAnimationGroup->disconnect(this, SIGNAL(animationFinished()));
    mAnimationGroup->setDirection(QAbstractAnimation::Forward);
    mAnimationGroup->clear();
    delete mDialog;
    mDialog = 0;
    mModel->setData(mFlippedIndex, true, Qt::UserRole+1);    
}

void HgWidgetTestView::openDialog(const QModelIndex& index)
{
    FUNC_LOG;
    
    QVariant image = mModel->silentData(index, Qt::DecorationRole);
    QVariant texts = mModel->silentData(index, Qt::DisplayRole);

    if (mWidgetType == HgWidgetCoverflow )
    {
        startAnimation(index);
        return;
    }
    else if(mWidgetType == HgWidgetTBone)
    {
        if (image.canConvert<QPixmap>() && texts.canConvert<QStringList>())
        {
            QStringList strList = texts.toStringList();
            if (strList.count() > 1)
            {
                if (mFrontItem)
                    mFrontItem->setVisible(false);

                if (mFlipWidget)
                    delete mFlipWidget;

                QPolygonF poly;
                if (!mWidget->getItemOutline(index, poly))
                    return;

                QRectF itemRect = poly.boundingRect();
                itemRect.moveTo(itemRect.topLeft() + mWidget->geometry().topLeft());

                mFlipWidget =
                    new HgFlipWidget(
                        strList.at(0),
                        strList.at(1),
                        image.value<QPixmap>(),
                        this);

                mFlipWidget->setPos(QPointF(
                    itemRect.center().x() - itemRect.width() / 2,
                    itemRect.center().y() - itemRect.height() / 2
                ));

                mFlipWidget->resize(itemRect.width(), itemRect.height());
                mFlipWidget->show();

                QObject::connect(mFlipWidget, SIGNAL(closed()), this, SLOT(flipClosed()));
                mModel->setData(index, false, Qt::UserRole+1);
                mFlippedIndex = index;
            }
        }

        return;
    }

    HbDialog* dlg = new HbDialog();
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setTimeout(HbPopup::NoTimeout);
    dlg->setDismissPolicy(HbPopup::TapAnywhere);
    dlg->setModal(false);
    dlg->setBackgroundFaded(false);
    dlg->addAction(new HbAction("Close",dlg));
    if (texts.canConvert<QStringList>())
    {
        QStringList strList = texts.toStringList();
        if (strList.count() > 0)
        {
            dlg->setHeadingWidget(new HbLabel(strList.at(0)));
        }
    }
    if (image.canConvert<QImage>())
    {
        HbLabel *content = new HbLabel;
        QImage realImage(image.value<QImage>());
        QPixmap pixmap = QPixmap::fromImage( realImage  );
        content->setIcon(HbIcon(pixmap));
        dlg->setContentWidget(content);
    }
    dlg->show();
}

void HgWidgetTestView::startAnimation(const QModelIndex& index)
{
    if( mAnimationGroup->state() == QAbstractAnimation::Running ||
        mDialog || mCoverItem ) return;

    QPolygonF poly;
    if (!mWidget->getItemOutline(index, poly))
        return;
    
    
    mFlippedIndex = index;
    
    QRectF itemRect = poly.boundingRect();
    itemRect.moveTo(itemRect.topLeft() + mWidget->geometry().topLeft());
    
    QVariant image = mModel->silentData(index, Qt::DecorationRole);
    QPixmap pixmap;
    QImage temp = image.value<QImage>();
    QImage scaled = temp.scaled(itemRect.size().toSize());
    if (scaled.isNull()) return;
    QPixmap pixmap2 = pixmap.fromImage(scaled);    
    if(pixmap2.isNull()) return;
    
    mCoverItem = new HgCoverItem(pixmap2, this);
    mCoverItem->setPos(itemRect.topLeft());
    mCoverItem->setZValue(mWidget->zValue()+2);
    mCoverItem->show();
    
    QPropertyAnimation* animation = new QPropertyAnimation(mCoverItem, "geometry");
    animation->setDuration(400);        
    animation->setStartValue(itemRect);

    QRectF endRect = itemRect;
    int topLeftX = (mWidget->size().width() - itemRect.size().width()*2)/2;
    endRect.moveTo(topLeftX,endRect.topLeft().y());
    animation->setEndValue(endRect);
    
    mAnimationGroup->addAnimation(animation);
        
    HgListWidget* list = new HgListWidget(this);
    mDialog = new HbDialog(this);
    mDialog->setZValue(mWidget->zValue()+1);
    mDialog->setContentWidget(list);
    mDialog->setDismissPolicy(HbPopup::TapOutside);
    list->addItem( "List item 1");
    list->addItem( "List item 2");
    list->addItem( "List item 3");
    list->addItem( "List item 4");
    list->addItem( "List item 5");
    list->addItem( "List item 6");
    list->addItem( "List item 7");
    
    connect(list,SIGNAL(activated(HbListWidgetItem*)),this, SLOT(activated()));
    connect(list,SIGNAL(close()),this, SLOT(activated()));
    connect(mCoverItem,SIGNAL(close()),this, SLOT(activated()));
    
    animation = new QPropertyAnimation(mDialog, "geometry");
    animation->setDuration(400);
    animation->setStartValue(itemRect);
    endRect.moveTo(endRect.topRight());
    animation->setEndValue(endRect);

    mModel->setData(mFlippedIndex, false, Qt::UserRole+1);    
    
    mAnimationGroup->addAnimation(animation);
    mAnimationGroup->start();    
}

void HgWidgetTestView::openView(const QModelIndex& index)
{
    FUNC_LOG;

    QVariant image = mModel->silentData(index, Qt::DecorationRole);
    QVariant texts = mModel->silentData(index, Qt::DisplayRole);

    if (image.canConvert<QImage>() && texts.canConvert<QStringList>())
    {
        QStringList strList = texts.toStringList();
        if (strList.count() > 1)
        {
            HgTestView* view =
                new HgTestView(
                    strList.at(0),
                    strList.at(1),
                    QPixmap::fromImage(image.value<QImage>()));
            connect(view, SIGNAL(closeRequested()), SLOT(closeCurrentView()));
            QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
            if (mainWindows.count() > 0)
            {
                HbMainWindow *primaryWindow = mainWindows[0];
                connect(primaryWindow, SIGNAL(orientationChanged(Qt::Orientation)), view, SLOT(orientationChanged(Qt::Orientation)));
                primaryWindow->addView(view);
                primaryWindow->setCurrentView(view);
				// For photos simulation
//                primaryWindow->setOrientation(Qt::Horizontal, false);
            }
        }
    }
}

void HgWidgetTestView::closeCurrentView()
{
    FUNC_LOG;

    QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
    if (mainWindows.count() > 0) {
        HbMainWindow *primaryWindow = mainWindows[0];
        HbView *currentView = primaryWindow->currentView();
        primaryWindow->setCurrentView(this);
        primaryWindow->removeView(currentView);
		// For photos simulation
//        primaryWindow->unsetOrientation(false);
    }
}

void HgWidgetTestView::openDeleteItemsDialog()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mWidget);

//    HgWidget *widget = copyWidget();
//    HANDLE_ERROR_NULL(widget);

    mLayout->removeItem(mWidget);
    
    HgSelectionDialog *dlg =
        new HgSelectionDialog("Remove items", "Remove", mWidget); // Takes ownership of widget
    HANDLE_ERROR_NULL(dlg);

    mWidget->setPreferredSize(mWidget->size());
    connect(dlg, SIGNAL(finished(HbAction*)),
            SLOT(dialogFinished(HbAction*)));
    
//    mWidget->hide();
    mWidget->setSelectionMode(HgWidget::MultiSelection);
    dlg->open();
/*    bool removeItems = (dlg->exec() == dlg->primaryAction());
    QItemSelection selection = mSelectionModel->selection();
    widget->setSelectionMode(HgWidget::NoSelection); // Clears the selection
    delete dlg;

    if (removeItems) {
        mModel->remove(selection);
    }
*/
//    mWidget->show();
}

void HgWidgetTestView::dialogFinished(HbAction* action)
{
    mWidget->setSelectionMode(HgWidget::NoSelection); // Clears the selection
    if (action->text() == "Remove" ) {
        QItemSelection selection = mSelectionModel->selection();    
        mModel->remove(selection);
    }    
    mLayout->addItem(mWidget);
}

void HgWidgetTestView::openMoveItemsDialog()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mWidget);

 /*   HgWidget *widget = copyWidget();
    HANDLE_ERROR_NULL(widget);
    HgSelectionDialog *dlg =
        new HgSelectionDialog("Select items to move", "Move to...", widget); // Takes ownership of widget
        HANDLE_ERROR_NULL(dlg);
    HANDLE_ERROR_NULL(dlg);

    mWidget->hide();
    widget->setSelectionMode(HgWidget::ContiguousSelection);
    bool moveItems = (dlg->exec() == dlg->primaryAction());
    QItemSelection selection = mSelectionModel->selection();
    widget->setSelectionMode(HgWidget::NoSelection); // Clears the selection
    delete dlg;
    dlg = 0;
    widget = 0;

    if (moveItems) {
        widget = copyWidget();
        HANDLE_ERROR_NULL(widget);
        widget->setPreferredSize(size().width(), 320);
        dlg = new HgSelectionDialog("Select target location", "Move", widget); // Takes ownership of widget
        HANDLE_ERROR_NULL(dlg);
        widget->setSelectionMode(HgWidget::SingleSelection);
        moveItems = (dlg->exec() == dlg->primaryAction());
        QItemSelection target = mSelectionModel->selection();
        widget->setSelectionMode(HgWidget::NoSelection); // Clears the selection
        delete dlg;

        if (moveItems) {
            mModel->move(selection, target.indexes().at(0));
        }
    }
    mWidget->show();
    */
}

void HgWidgetTestView::openAddItemsDialog()
{
    FUNC_LOG;
/*    HANDLE_ERROR_NULL(mWidget);

    HgWidget *widget = copyWidget();
    HANDLE_ERROR_NULL(widget);
    HgSelectionDialog *dlg =
        new HgSelectionDialog("Select position to add items to", "Add", widget); // Takes ownership of widget
    HANDLE_ERROR_NULL(dlg);

    mWidget->hide();
    widget->setSelectionMode(HgWidget::SingleSelection);
    bool addItems = (dlg->exec() == dlg->primaryAction());
    QItemSelection target = mSelectionModel->selection();
    widget->setSelectionMode(HgWidget::NoSelection); // Clears the selection
    delete dlg;

    if (addItems) {
        mModel->add(target.indexes().at(0), 5);
    }

    mWidget->show();
*/
}

void HgWidgetTestView::showOptions()
{
    FUNC_LOG;

    QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
    if (mainWindows.count() > 0) {
        HbMainWindow *primaryWindow = mainWindows[0];
        if (!mOptionsView) {
            HgWidgetOptionsView* view = new HgWidgetOptionsView;
            connect(view, SIGNAL(optionsClosed()), SLOT(hideOptions()));
            connect(view, SIGNAL(widgetTypeChanged(HgTestWidgetType)),
                SLOT(initWidget(HgTestWidgetType)));
            connect(view, SIGNAL(scrollBarVisibilityChanged(HgWidget::ScrollBarPolicy)),
                SLOT(changeScrollBarVisibility(HgWidget::ScrollBarPolicy)));
            connect(view, SIGNAL(scrollBarInteractivityChanged(bool)),
                SLOT(changeScrollBarInteractivity(bool)));
            connect(view, SIGNAL(imageTypeChanged(HgTestImageType)),
                SLOT(changeModelImageType(HgTestImageType)));
            connect(view, SIGNAL(lowResImageUseChanged(bool)), SLOT(changeLowResImageUse(bool)));
            connect(view, SIGNAL(widgetHeightChanged(int)), SLOT(changeWidgetHeight(int)));
            connect(view, SIGNAL(widgetWidthChanged(int)), SLOT(changeWidgetWidth(int)));
            connect(view, SIGNAL(titleFontChanged(HbFontSpec)),
                SLOT(changeTitleFont(HbFontSpec)));
            connect(view, SIGNAL(descriptionFontChanged(HbFontSpec)),
                SLOT(changeDescriptionFont(HbFontSpec)));
            connect(view, SIGNAL(reflectionsEnabledChanged(bool)), 
                SLOT(changeReflectionsEnabled(bool)));
            connect(view, SIGNAL(effect3dEnabledChanged(bool)), 
                SLOT(changeEffect3dEnabled(bool)));
            connect(view, SIGNAL(itemSizePolicyChanged(HgWidget::ItemSizePolicy)),
                SLOT(changeItemSizePolicy(HgWidget::ItemSizePolicy)));

            mOptionsView = view;
            primaryWindow->addView(mOptionsView);
        }
        primaryWindow->setCurrentView(mOptionsView);
    }
}

void HgWidgetTestView::hideOptions()
{
    FUNC_LOG;

    QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
    if (mainWindows.count() > 0) {
        HbMainWindow *primaryWindow = mainWindows[0];
        primaryWindow->setCurrentView(this);
    }
}

void HgWidgetTestView::setupWidget()
{
    FUNC_LOG;

    QSettings settings(SETT_ORGANIZATION, SETT_APPLICATION);

    QVariant value = settings.value(SETT_WIDGET_TYPE);
    if (value.isValid()) {
        initWidget(static_cast<HgTestWidgetType>(value.toInt()));
    }
    else {
        initWidget(HgWidgetGrid);
    }

    value = settings.value(SETT_LOW_RES_IMAGES);
    if (value.isValid()) {
        changeLowResImageUse(value.toBool());
    }
}

void HgWidgetTestView::setupWidgetOptions()
{
    FUNC_LOG;

    QSettings settings(SETT_ORGANIZATION, SETT_APPLICATION);

    QVariant value = settings.value(SETT_SCROLLBAR_VISIBILITY);
    if (value.isValid()) {
        changeScrollBarVisibility(static_cast<HgWidget::ScrollBarPolicy>(value.toInt()));
    }

    value = settings.value(SETT_SCROLLBAR_INTERACTIVITY);
    if (value.isValid()) {
        changeScrollBarInteractivity(value.toBool());
    }

    value = settings.value(SETT_MODEL_IMAGE_TYPE);
    if (value.isValid()) {
        changeModelImageType(static_cast<HgTestImageType>(value.toInt()));
    }
	else {
        changeModelImageType(ImageTypeQImage);
	}

    value = settings.value(SETT_TITLE_FONT);
    if (value.isValid()) {
        changeTitleFont(HbFontSpec(static_cast<HbFontSpec::Role>(value.toInt())));
    }

    value = settings.value(SETT_DESCRIPTION_FONT);
    if (value.isValid()) {
        changeDescriptionFont(HbFontSpec(static_cast<HbFontSpec::Role>(value.toInt())));
    }

    value = settings.value(SETT_REFLECTIONS_ENABLED);
    if (value.isValid()) {
        changeReflectionsEnabled(value.toBool());
    }

    value = settings.value(SETT_ITEM_SIZE_POLICY);
    if (value.isValid()) {
        changeItemSizePolicy(static_cast<HgMediawall::ItemSizePolicy>(value.toInt()));
    }
}

void HgWidgetTestView::setupWidgetSize()
{
    QSettings settings(SETT_ORGANIZATION, SETT_APPLICATION);

    QVariant value = settings.value(SETT_WIDGET_HEIGHT);
    if (value.isValid()) {
        changeWidgetHeight(value.toInt());
    }

    value = settings.value(SETT_WIDGET_WIDTH);
    if (value.isValid()) {
        changeWidgetWidth(value.toInt());
    }
}

HgWidget *HgWidgetTestView::createWidget(HgTestWidgetType type) const
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mModel);
    HANDLE_ERROR_NULL(mSelectionModel);

    Qt::Orientation scrollDirection = Qt::Vertical;
    QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
    if (mainWindows.count() > 0)
    {
        HbMainWindow *primaryWindow = mainWindows[0];
        primaryWindow->scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
        primaryWindow->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
        if (primaryWindow->orientation() == Qt::Horizontal) {
            scrollDirection = Qt::Horizontal;
        }
    }

    HgWidget* widget = 0;

    HbStyleLoader::unregisterFilePath(":/test/hgmediawall.css");
    HbStyleLoader::unregisterFilePath(":/test/hgmediawall.widgetml");

    switch (type) {
        case HgWidgetGrid:
            mModel->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
            mModel->setBuffer(GRIDBUFFERSIZE, GRIDBUFFERSIZE/3);
            widget = new HgGrid(scrollDirection);
            break;
        case HgWidgetCoverflow:
            {
            mModel->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
            mModel->setBuffer(COVERFLOWBUFFERSIZE, COVERFLOWBUFFERSIZE/3);
            HgMediawall *mediaWall = new HgMediawall;
//            mediaWall->setItemSize(QSizeF(4, 3)); // Sets aspect ratio
            HbStyleLoader::registerFilePath(":/test/hgmediawall.css");
            HbStyleLoader::registerFilePath(":/test/hgmediawall.widgetml");
            widget = mediaWall;
            }
            break;
        case HgWidgetTBone:
            mModel->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
            mModel->setBuffer(COVERFLOWBUFFERSIZE, COVERFLOWBUFFERSIZE/3);
            widget = new HgMediawall;
            HbStyleLoader::registerFilePath(":/test/hgmediawall.css");
            HbStyleLoader::registerFilePath(":/test/hgmediawall.widgetml");
            break;
        default:
            break;
    }

    HANDLE_ERROR_NULL(widget);
    
    widget->setModel(mModel);
    widget->setSelectionModel(mSelectionModel);
    widget->setLongPressEnabled(true);

    QImage defaultImage(":/images/default.svg");
    widget->setDefaultImage(defaultImage);

    return widget;
}

HgWidget *HgWidgetTestView::copyWidget() const
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mWidget);

    HgWidget* widget = createWidget(mWidgetType);
    widget->setPreferredSize(mWidget->size());

    HgMediawall *original = qobject_cast<HgMediawall *>(mWidget);
    HgMediawall *copy = qobject_cast<HgMediawall *>(widget);
    if (original && copy) {
        copy->setTitleFontSpec(original->titleFontSpec());
        copy->setDescriptionFontSpec(original->descriptionFontSpec());
    }

    return widget;
}

void HgWidgetTestView::flipClosed()
{
    delete mFlipWidget;
    mFlipWidget = 0;
    mModel->setData(mFlippedIndex, true, Qt::UserRole+1);

    if (mFrontItem) {
        mFrontItem->setVisible(true);
    }

}

void HgWidgetTestView::onScrollingStarted()
{
    FUNC_LOG;

    qDebug() << "SCROLLING onScrollingStarted";
    
    // scrolling started, need to hide
    // label displaying full resolution image
    if (mFrontItem)
        mFrontItem->setVisible(false);

}

void HgWidgetTestView::onScrollingEnded()
{
    FUNC_LOG;

    qDebug() << "SCROLLING onScrollingEnded";

    if (mModel->lowResImagesEnabled()) {

        if (!mWidget)
            return;

        // get index to current item
        QModelIndex index = mWidget->currentIndex();
        if (!index.isValid())
            return;

        // get outlines of the item so we know where to render
        QPolygonF poly;
        if (!mWidget->getItemOutline(index, poly))
            return;

        // fetch highresolution image from the model
        QVariant imgVariant = mModel->silentData(index, Qt::UserRole+2);
        if (imgVariant.isNull())
            return;

        QRectF itemRect = poly.boundingRect();
        itemRect.moveTo(itemRect.topLeft() + mWidget->geometry().topLeft());

        // show it using HbLabel
        QPixmap pixmap = imgVariant.value<QPixmap>().scaled(itemRect.width(), itemRect.height());

        if (!mFrontItem) {
            mFrontItem = new HbLabel(this);
        }


        mFrontItem->setVisible(false);
        mFrontItem->setIcon(HbIcon(pixmap));
        mFrontItem->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        mFrontItem->setPos(itemRect.center() - QPointF(itemRect.width()/2, itemRect.height()/2));
        mFrontItem->resize(itemRect.width(), itemRect.height());
        mFrontItem->setVisible(true);
    }
}

void HgWidgetTestView::orientationChanged(Qt::Orientation orientation)
{
    if (mDialog) {
        mModel->setData(mFlippedIndex, true, Qt::UserRole+1);
    }
    mAnimationGroup->stop();            
    delete mDialog;
    mDialog = 0;
    delete mCoverItem;
    mCoverItem = 0;
    mAnimationGroup->clear();
    
    if (orientation == Qt::Horizontal && mWidgetType == HgWidgetCoverflow) {
        setItemVisible(Hb::AllItems, false);
    }
    else if (orientation == Qt::Horizontal
                && mWidgetType == HgWidgetGrid
                && mainWindow()->currentView() == this ) {
        setItemVisible(Hb::AllItems, false);
    }
    else if (orientation == Qt::Horizontal && mWidgetType == HgWidgetTBone) {
        initWidget(HgWidgetCoverflow);
        setItemVisible(Hb::AllItems, false);
    }
    else if (orientation == Qt::Vertical && mWidgetType == HgWidgetCoverflow) {
        initWidget(HgWidgetTBone);
        setItemVisible(Hb::AllItems, true);
    }
    else if (orientation == Qt::Vertical && mWidgetType == HgWidgetGrid) {
        setItemVisible(Hb::AllItems, true);
    }
}
void HgWidgetTestView::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    Q_UNUSED(event);
    FUNC_LOG;

    setupWidgetSize();

/*    if (mWidgetType == HgWidgetCoverflow ||
        mWidgetType == HgWidgetTBone)
    {
        HgMediawall* wall = qobject_cast<HgMediawall*>(mWidget);
        wall->setFrontItemPosition(QPointF(mWidget->size().width()/2, 
            mWidget->size().height()/2));
    }*/
}

Qt::Orientation HgWidgetTestView::orientation() const
{
    FUNC_LOG;

    if (mainWindow()) {
        return mainWindow()->orientation();
    }

    return Qt::Horizontal;
}

void HgWidgetTestView::startItemSizeChange()
{
    if (mItemSizeDialog)
        return;
    
    if (!mWidget)
        return;

    QSizeF itemSize = mWidget->itemSize();
    QSizeF itemSpacing = mWidget->itemSpacing();
    mItemSizeDialog = new HgItemSizeDialog(itemSize, itemSpacing, this);
    
    QObject::connect(mItemSizeDialog, SIGNAL(updateItemSizeAndSpacing()), this, SLOT(updateItemSizeAndSpacing()));
    QObject::connect(mItemSizeDialog, SIGNAL(closed()), this, SLOT(itemSizeDialogClosed()));
}

void HgWidgetTestView::useCustomLayout()
{
    HgMediawall* wall = qobject_cast<HgMediawall*>(mWidget);
    if (wall) {
        mWidgetType = HgWidgetNone;
        initWidget(HgWidgetCoverflow);
        HbStyleLoader::registerFilePath(":/test/hgmediawall.css");
        HbStyleLoader::registerFilePath(":/test/hgmediawall_color.css");
        HbStyleLoader::registerFilePath(":/test/hgmediawall.widgetml");
    }        
}

void HgWidgetTestView::startItemPosChange()
{
    if (mItemPosDialog)
        return;
    
    if (!mWidget)
        return;
    
    HgMediawall* wall = qobject_cast<HgMediawall*>(mWidget);
    if (!wall)
        return;
    
    QPointF itemPos = wall->frontItemPositionDelta();
    QSizeF s(itemPos.x(), itemPos.y());
    mItemPosDialog = new HgItemSizeDialog(s, s, this);
    
    mItemPosDialog->setSliderLimits(-500, 500);
    
    QObject::connect(mItemPosDialog, SIGNAL(updateItemSizeAndSpacing()), this, SLOT(updateItemPos()));
    QObject::connect(mItemPosDialog, SIGNAL(closed()), this, SLOT(itemPosDialogClosed()));
}



void HgWidgetTestView::updateItemSizeAndSpacing()
{
    if (!mItemSizeDialog)
        return;
    
    mWidget->setItemSize(mItemSizeDialog->itemSize());
    mWidget->setItemSpacing(mItemSizeDialog->itemSpacing());
}

void HgWidgetTestView::updateItemPos()
{
    if (!mItemPosDialog)
        return;
    
    HgMediawall* wall = qobject_cast<HgMediawall*>(mWidget);
    if (!wall)
        return;

    QSizeF s = mItemPosDialog->itemSize();
    wall->setFrontItemPositionDelta(QPointF(s.width(), s.height()));
}

void HgWidgetTestView::itemSizeDialogClosed()
{
    // dialog deletes it self at close
    mItemSizeDialog = NULL;
}

void HgWidgetTestView::itemPosDialogClosed()
{
    mItemPosDialog = NULL;
}

void HgWidgetTestView::resetOptions()
{
    QSettings settings(SETT_ORGANIZATION, SETT_APPLICATION);
    settings.clear();
    setupWidget();
}

void HgWidgetTestView::resetModel()
{
    mModel->reset();
}
