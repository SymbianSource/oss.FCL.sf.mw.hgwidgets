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

#ifndef HGWIDGETTESTVIEW_H_
#define HGWIDGETTESTVIEW_H_

#include <hbview.h>
#include <hgwidgets/hgmediawall.h>
#include "hgtestdefs.h"

class HbAction;
class QGraphicsLinearLayout;
class HgWidgetTestDataModel;
class HbListWidget;
class HgFlipWidget;
class HbLabel;
class HbDialog;

class HgItemSizeDialog;

class HgWidgetTestView : public HbView
{
    Q_OBJECT

public:

    HgWidgetTestView(QGraphicsItem *parent = 0);
    ~HgWidgetTestView();

private slots:
    void initWidget(HgTestWidgetType type);
    void changeScrollBarVisibility(HgWidget::ScrollBarPolicy policy);
    void changeScrollBarInteractivity(bool value);
    void changeModelImageType(HgTestImageType);
    void changeLowResImageUse(bool value);
    void changeTitlePosition(HgMediawall::LabelPosition);
    void changeTitleFont(const HbFontSpec &);
    void changeDescriptionPosition(HgMediawall::LabelPosition);
    void changeDescriptionFont(const HbFontSpec &);
    void changeWidgetHeight(int);
    void changeWidgetWidth(int);
    void toggleScrollDirection();
    void openDialog(const QModelIndex &index);
    void openView(const QModelIndex& index);
    void closeCurrentView();
    void openDeleteItemsDialog();
    void openMoveItemsDialog();
    void openAddItemsDialog();
    void flipClosed();
    void onScrollingStarted();
    void onScrollingEnded();
    void showOptions();
    void hideOptions();
    void orientationChanged(Qt::Orientation);
    
    void startItemSizeChange();
    void itemSizeDialogClosed();
    void updateItemSizeAndSpacing();

    void startItemPosChange();
    void updateItemPos();
    void itemPosDialogClosed();

    void resetOptions();
    void changeReflectionsEnabled(bool);
    void resetModel();
private:

    void createMenu();
    void setupWidget();
    void setupWidgetOptions();
    void setupWidgetSize();
    HgWidget *createWidget(HgTestWidgetType type) const;
    HgWidget *copyWidget() const;
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    Qt::Orientation orientation() const;

private: // data

    HgWidget                *mWidget;
    QGraphicsLinearLayout   *mLayout;
    HgWidgetTestDataModel   *mModel;
    HgTestWidgetType        mWidgetType;
    HbListWidget            *mListWidget;
    bool                    mToggleOrientation;
    QItemSelectionModel     *mSelectionModel;
    HgFlipWidget*           mFlipWidget;
    QModelIndex             mFlippedIndex;
    HbLabel*                mFrontItem;
    HbView                  *mOptionsView;
    HgItemSizeDialog       *mItemSizeDialog;
    HgItemSizeDialog       *mItemPosDialog;
};


#endif /* HGWIDGETTESTVIEW_H_ */
