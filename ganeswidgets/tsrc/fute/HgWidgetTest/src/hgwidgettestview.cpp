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
#include <hbdialog.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <hblistwidget.h>
#include <hbmenu.h>
#include <hbscrollbar>
#include <QActionGroup>
#include <QGraphicsLinearLayout>
#include "hgwidgettestview.h"
#include "hgtestview.h"
#include "hgselectiondialog.h"
#include "hgwidgettestdatamodel.h"
#include "hgflipwidget.h"
#include "trace.h"
#include <hgwidgets/hggrid.h>
#include <hgwidgets/hgmediawall.h>

HgWidgetTestView::HgWidgetTestView(QGraphicsItem *parent) :
    HbView(parent),
    mWidget(NULL),
    mModel(NULL),
    mListWidget(NULL),
    mToggleOrientation(false),
    mTBone(false),
    mFlipWidget(NULL),
    mFrontItem(NULL)
{
    mModel = new HgWidgetTestDataModel(this);
    mModel->setImageDataType(HgWidgetTestDataModel::TypeQImage);
    mSelectionModel = new QItemSelectionModel(mModel, this);

    createMenu();

    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mLayout->setContentsMargins(0,0,0,0);
    initWidget( HgWidgetGrid );
    setLayout( mLayout );
}

HgWidgetTestView::~HgWidgetTestView()
{
}

void HgWidgetTestView::createMenu()
{
    FUNC_LOG;

    HbMenu* modeMenu = new HbMenu("Change widget type");
    menu()->addMenu(modeMenu);

    HbMenu* scrollBarMenu = new HbMenu("ScrollBar settings");
    menu()->addMenu( scrollBarMenu );

    HbMenu* imageTypeMenu = new HbMenu("Datamodel image type");
    menu()->addMenu( imageTypeMenu );

    HbAction* gridAction = modeMenu->addAction( "Use grid" );
    HbAction* coverFlowAction = modeMenu->addAction( "Use coverFlow" );
    HbAction* TBone = modeMenu->addAction( "Use TBone" );
    connect( modeMenu, SIGNAL(triggered(HbAction*)), this, SLOT(switchWidget(HbAction*)) );

    QActionGroup* ac1 = new QActionGroup( this );
    gridAction->setCheckable( true );
    coverFlowAction->setCheckable( true );
    TBone->setCheckable(true);
    gridAction->setChecked( true );
    ac1->addAction( gridAction );
    ac1->addAction( coverFlowAction );
    ac1->addAction( TBone );

    HbAction* scrollBarAutoHideAction = scrollBarMenu->addAction( "Autohide ScrollBar" );
    HbAction* scrollBarAlwaysOnAction = scrollBarMenu->addAction( "ScrollBar always on" );
    HbAction* scrollBarAlwaysOffAction = scrollBarMenu->addAction( "ScrollBar always off" );
    scrollBarMenu->addSeparator();
    HbAction* interactiveScrollBarAction = scrollBarMenu->addAction( "Interactive scrollbar" );
    HbAction* uninteractiveScrollBarAction = scrollBarMenu->addAction( "Uninteractive scrollbar" );
    connect( scrollBarAutoHideAction, SIGNAL(triggered()), this, SLOT(autoHideScrollBar()) );
    connect( scrollBarAlwaysOnAction, SIGNAL(triggered()), this, SLOT(scrollBarAlwaysOn()) );
    connect( scrollBarAlwaysOffAction, SIGNAL(triggered()), this, SLOT(scrollBarAlwaysOff()) );
    connect( interactiveScrollBarAction, SIGNAL(triggered()), this, SLOT(interactiveScrollBar()) );
    connect( uninteractiveScrollBarAction, SIGNAL(triggered()), this, SLOT(unInteractiveScrollBar()) );

    QActionGroup* ac2 = new QActionGroup( this );
    scrollBarAutoHideAction->setCheckable( true );
    scrollBarAlwaysOnAction->setCheckable( true );
    scrollBarAlwaysOffAction->setCheckable(true);
    scrollBarAutoHideAction->setChecked( true );
    ac2->addAction( scrollBarAutoHideAction );
    ac2->addAction( scrollBarAlwaysOnAction );
    ac2->addAction( scrollBarAlwaysOffAction );

    QActionGroup* ac3 = new QActionGroup( this );
    interactiveScrollBarAction->setCheckable( true );
    uninteractiveScrollBarAction->setCheckable( true );
    uninteractiveScrollBarAction->setChecked( true );
    ac3->addAction( interactiveScrollBarAction );
    ac3->addAction( uninteractiveScrollBarAction );

    HbAction* qimageAction = imageTypeMenu->addAction( "feed QImages" );
    HbAction* hbiconAction = imageTypeMenu->addAction( "feed HbIcons" );
    HbAction* qiconAction = imageTypeMenu->addAction( "feed QIcons" );
    connect( qimageAction, SIGNAL(triggered()), this, SLOT(feedqimages()) );
    connect( hbiconAction, SIGNAL(triggered()), this, SLOT(feedhbicons()) );
    connect( qiconAction, SIGNAL(triggered()), this, SLOT(feedqicons()) );

    QActionGroup* ac4 = new QActionGroup( this );
    qimageAction->setCheckable( true );
    hbiconAction->setCheckable( true );
    qiconAction->setCheckable( true );
    qimageAction->setChecked( true );
    ac4->addAction( qimageAction );
    ac4->addAction( hbiconAction );
    ac4->addAction( qiconAction );

    menu()->addAction("Toggle scrolldirection", this, SLOT(toggleScrollDirection()));
    menu()->addAction("Simulate orientation switch", this, SLOT(orientationChanged()));

    mUseLowResAction = menu()->addAction( "Use low res images for coverflow" );
    mUseLowResAction->setCheckable(true);
    mUseLowResAction->setChecked(false);
    mUseLowResAction->setEnabled(false);
    connect( mUseLowResAction, SIGNAL(triggered()), this, SLOT(toggleLowResForCoverflow()) );
    
    HbMenu *modelChangeSubMenu = menu()->addMenu("Change model");
    modelChangeSubMenu->addAction("Remove items", this, SLOT(openDeleteItemsDialog()));
    modelChangeSubMenu->addAction("Move items", this, SLOT(openMoveItemsDialog()));
    modelChangeSubMenu->addAction("Add items", this, SLOT(openAddItemsDialog()));

    HbMenu *labelChangeSubMenu = menu()->addMenu("Change labels");
    HbMenu *titleSubMenu = labelChangeSubMenu->addMenu("Title");
    HbAction *aboveAction1 = titleSubMenu->addAction("Above", this, SLOT(setTitleAboveImage()));
    HbAction *belowAction1 = titleSubMenu->addAction("Below", this, SLOT(setTitleBelowImage()));
    HbAction *hiddenAction1 = titleSubMenu->addAction("Hide", this, SLOT(setTitleHidden()));
    QActionGroup* ac5 = new QActionGroup(this);
    aboveAction1->setCheckable(true);
    belowAction1->setCheckable(true);
    hiddenAction1->setCheckable(true);
    hiddenAction1->setChecked(true);
    ac5->addAction(aboveAction1);
    ac5->addAction(belowAction1);
    ac5->addAction(hiddenAction1);

    HbMenu *descriptionSubMenu = labelChangeSubMenu->addMenu("Description");
    HbAction *aboveAction2 = descriptionSubMenu->addAction("Above", this, SLOT(setDescriptionAboveImage()));
    HbAction *belowAction2 = descriptionSubMenu->addAction("Below", this, SLOT(setDescriptionBelowImage()));
    HbAction *hiddenAction2 = descriptionSubMenu->addAction("Hide", this, SLOT(setDescriptionHidden()));
    QActionGroup* ac6 = new QActionGroup(this);
    aboveAction2->setCheckable(true);
    belowAction2->setCheckable(true);
    hiddenAction2->setCheckable(true);
    hiddenAction2->setChecked(true);
    ac6->addAction(aboveAction2);
    ac6->addAction(belowAction2);
    ac6->addAction(hiddenAction2);
}

void HgWidgetTestView::switchWidget(HbAction* action)
{
    FUNC_LOG;

    mTBone = false;
    if( action->text() == "Use grid"){
        initWidget( HgWidgetGrid );
    }
    else if( action->text() == "Use coverFlow"){
        initWidget( HgWidgetCoverflow );
    }
    else if( action->text() == "Use TBone" ){
        mTBone = true;
        initWidget( HgWidgetCoverflow );
    }
}

void HgWidgetTestView::toggleScrollDirection()
{
    FUNC_LOG;

    mToggleOrientation = !mToggleOrientation;
    initWidget( mWidgetType );
}

void HgWidgetTestView::initWidget( WidgetType type )
{
    FUNC_LOG;

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

    switch (type)
        {
        case HgWidgetGrid:
            {
            mUseLowResAction->setEnabled(false);
            mModel->enableLowResImages(false);
            // TODO, init grid different model,
            mModel->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
            break;
            }
        case HgWidgetCoverflow:
            {
            mUseLowResAction->setEnabled(true);
            mModel->enableLowResImages(mUseLowResAction->isChecked());        
            mModel->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
            if (mTBone) {
                mListWidget = new HbListWidget;
                mLayout->addItem(mListWidget);
                mListWidget->addItem( "List item 1");
                mListWidget->addItem( "List item 2");
                mListWidget->addItem( "List item 3");
                }
            break;
            }
        default:
            break;
        }

    HANDLE_ERROR_NULL(mWidget);
    if (mWidget)
    {
        mWidget->setModel( mModel );
        connect(mWidget, SIGNAL(activated(QModelIndex)), SLOT(openDialog(QModelIndex)));
        connect(mWidget, SIGNAL(longPressed(QModelIndex, QPointF)), SLOT(openView(QModelIndex)));
        QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
        if (mainWindows.count() > 0)
        {
            HbMainWindow *primaryWindow = mainWindows[0];
            connect(primaryWindow, SIGNAL(orientationChanged(Qt::Orientation)), mWidget, SLOT(orientationChanged(Qt::Orientation)));
        }

    connect(mWidget, SIGNAL(scrollingStarted()), SLOT(onScrollingStarted()));
    connect(mWidget, SIGNAL(scrollingEnded()), SLOT(onScrollingEnded()));
    }
}

void HgWidgetTestView::openDialog(const QModelIndex& index)
{
    FUNC_LOG;

    QVariant image = mModel->data(index, Qt::DecorationRole);
    QVariant texts = mModel->data(index, Qt::DisplayRole);

    if (mWidgetType == HgWidgetCoverflow)
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

    HbDialog dlg;
    dlg.setTimeout(HbPopup::NoTimeout);
    dlg.setDismissPolicy(HbPopup::TapInside);
    dlg.setPrimaryAction(new HbAction("Close"));
    if (texts.canConvert<QStringList>())
    {
        QStringList strList = texts.toStringList();
        if (strList.count() > 0)
        {
            dlg.setHeadingWidget(new HbLabel(strList.at(0)));
        }
    }
    if (image.canConvert<QImage>())
    {
        HbLabel *content = new HbLabel;
        QImage realImage(image.value<QImage>());
        QPixmap pixmap = QPixmap::fromImage( realImage  );
        content->setIcon(HbIcon(pixmap));
        dlg.setContentWidget(content);
    }
    dlg.exec();
}

void HgWidgetTestView::openView(const QModelIndex& index)
{
    FUNC_LOG;

    QVariant image = mModel->data(index, Qt::DecorationRole);
    QVariant texts = mModel->data(index, Qt::DisplayRole);

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
            QList<HbMainWindow *> mainWindows = hbInstance->allMainWindows();
            if (mainWindows.count() > 0)
            {
                HbMainWindow *primaryWindow = mainWindows[0];
                primaryWindow->addView(view);
                primaryWindow->setViewSwitchingEnabled(false);
                primaryWindow->setCurrentView(view);
            }
        }
    }
}

void HgWidgetTestView::openDeleteItemsDialog()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mWidget);

    HgWidget *widget = copyWidget();
    HANDLE_ERROR_NULL(widget);

    HgSelectionDialog *dlg =
        new HgSelectionDialog("Remove items", "Remove", widget); // Takes ownership of widget
    HANDLE_ERROR_NULL(dlg);

    mWidget->hide();
    widget->setSelectionMode(HgWidget::MultiSelection);
    bool removeItems = (dlg->exec() == dlg->primaryAction());
    QItemSelection selection = mSelectionModel->selection();
    widget->setSelectionMode(HgWidget::NoSelection); // Clears the selection
    delete dlg;

    if (removeItems) {
        mModel->remove(selection);
    }

    mWidget->show();
}

void HgWidgetTestView::openMoveItemsDialog()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mWidget);

    HgWidget *widget = copyWidget();
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
}

void HgWidgetTestView::openAddItemsDialog()
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mWidget);

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
}

void HgWidgetTestView::autoHideScrollBar()
{
    setScrollBarPolicy(HgWidget::ScrollBarAutoHide);
}

void HgWidgetTestView::scrollBarAlwaysOn()
{
    setScrollBarPolicy(HgWidget::ScrollBarAlwaysOn);
}

void HgWidgetTestView::scrollBarAlwaysOff()
{
    setScrollBarPolicy(HgWidget::ScrollBarAlwaysOff);
}

void HgWidgetTestView::setScrollBarPolicy( HgWidget::ScrollBarPolicy policy )
{
    mWidget->setScrollBarPolicy( policy );
}

void HgWidgetTestView::setScrollBarInteractive( bool value )
{
    if( value )
        setScrollBarPolicy(HgWidget::ScrollBarAlwaysOn);

    mWidget->scrollBar()->setInteractive(value);

    if (mWidgetType == HgWidgetCoverflow) {
        mWidget->setIndexFeedbackPolicy(HgWidget::IndexFeedbackSingleCharacter);
    }
    
}

void HgWidgetTestView::interactiveScrollBar()
{
    setScrollBarInteractive(true);
}

void HgWidgetTestView::unInteractiveScrollBar()
{
    setScrollBarInteractive(false);
}

HgWidget *HgWidgetTestView::createWidget(WidgetType type) const
{
    FUNC_LOG;
    HANDLE_ERROR_NULL(mModel);
    HANDLE_ERROR_NULL(mSelectionModel);

    Qt::Orientation scrollDirection = !mToggleOrientation ? Qt::Vertical : Qt::Horizontal ;

    HgWidget* widget = 0;

    switch (type) {
        case HgWidgetGrid:
            widget = new HgGrid(scrollDirection);
            break;
        case HgWidgetCoverflow:
            widget = new HgMediawall();
            break;
        default:
            break;
    }

    HANDLE_ERROR_NULL(widget);

    widget->setModel(mModel);
    widget->setSelectionModel(mSelectionModel);
    widget->setLongPressEnabled(true);
	widget->scrollTo(widget->currentIndex());

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
        copy->setTitlePosition(original->titlePosition());
        copy->setDescriptionPosition(original->descriptionPosition());
        copy->setTitleFontSpec(original->titleFontSpec());
        copy->setDescriptionFontSpec(original->descriptionFontSpec());
    }

    return widget;
}

void HgWidgetTestView::feedqimages()
{
    mModel->setImageDataType(HgWidgetTestDataModel::TypeQImage);
}

void HgWidgetTestView::feedqicons()
{
    mModel->setImageDataType(HgWidgetTestDataModel::TypeQIcon);
}

void HgWidgetTestView::feedhbicons()
{
    mModel->setImageDataType(HgWidgetTestDataModel::TypeHbIcon);
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
void HgWidgetTestView::orientationChanged()
{
    mWidget->orientationChanged(Qt::Horizontal);
}

void HgWidgetTestView::onScrollingStarted()
{
    FUNC_LOG;

    // scrolling started, need to hide 
    // label displaying full resolution image
    if (mFrontItem)
        mFrontItem->setVisible(false);

}

void HgWidgetTestView::onScrollingEnded()
{
    FUNC_LOG;

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
        QVariant imgVariant = mModel->data(index, Qt::UserRole+2);    
        if (imgVariant.isNull())
            return;
        
        QRectF itemRect = poly.boundingRect();
    
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

void HgWidgetTestView::toggleLowResForCoverflow()
{
    if (mWidgetType == HgWidgetCoverflow) {
        mModel->enableLowResImages(mUseLowResAction->isChecked());
        initWidget(mWidgetType);
    }
}

void HgWidgetTestView::setTitleAboveImage()
{
    FUNC_LOG;
    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall) {
        mediawall->setTitlePosition(HgMediawall::PositionAboveImage);
    }
}

void HgWidgetTestView::setTitleBelowImage()
{
    FUNC_LOG;
    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall) {
        mediawall->setTitlePosition(HgMediawall::PositionBelowImage);
    }
}

void HgWidgetTestView::setTitleHidden()
{
    FUNC_LOG;
    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall) {
        mediawall->setTitlePosition(HgMediawall::PositionNone);
    }
}

void HgWidgetTestView::setDescriptionAboveImage()
{
    FUNC_LOG;
    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall) {
        mediawall->setDescriptionPosition(HgMediawall::PositionAboveImage);
    }
}

void HgWidgetTestView::setDescriptionBelowImage()
{
    FUNC_LOG;
    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall) {
        mediawall->setDescriptionPosition(HgMediawall::PositionBelowImage);
    }
}

void HgWidgetTestView::setDescriptionHidden()
{
    FUNC_LOG;
    HgMediawall *mediawall = qobject_cast<HgMediawall *>(mWidget);
    if (mediawall) {
        mediawall->setDescriptionPosition(HgMediawall::PositionNone);
    }
}
