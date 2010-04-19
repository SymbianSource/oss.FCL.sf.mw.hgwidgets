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
#include <hgwidgets/hgwidgets.h>

class HbAction;
class QGraphicsLinearLayout;
class HgWidgetTestDataModel;
class HbListWidget;
class HgFlipWidget;
class HbLabel;

class HgWidgetTestView : public HbView
{
    Q_OBJECT

public:

    HgWidgetTestView(QGraphicsItem *parent = 0);
    ~HgWidgetTestView();

private slots:

    void switchWidget(HbAction* action);
    void toggleScrollDirection();
    void openDialog(const QModelIndex &index);
    void openView(const QModelIndex& index);
    void openDeleteItemsDialog();
    void openMoveItemsDialog();
    void openAddItemsDialog();
    void autoHideScrollBar();
    void scrollBarAlwaysOn();
    void scrollBarAlwaysOff();
    void interactiveScrollBar();
    void unInteractiveScrollBar();
    void feedqimages();
    void feedqicons();
    void feedhbicons();
    void flipClosed();
    void orientationChanged();
    void onScrollingStarted();
    void onScrollingEnded();
    void setDescriptionAboveImage();
    void setDescriptionBelowImage();
    void setDescriptionHidden();
    void setTitleAboveImage();
    void setTitleBelowImage();
    void setTitleHidden();
    void toggleLowResForCoverflow();

private:
    
    enum WidgetType {
        HgWidgetGrid,
        HgWidgetCoverflow
    };

    void createMenu();
    void initWidget( WidgetType type );
    void setScrollBarPolicy( HgWidget::ScrollBarPolicy policy );
    void setScrollBarInteractive( bool value );
    HgWidget *createWidget(WidgetType type) const;
    HgWidget *copyWidget() const;

private: // data

    HgWidget                *mWidget;
    QGraphicsLinearLayout   *mLayout;
    HgWidgetTestDataModel   *mModel;
    WidgetType              mWidgetType;
    HbListWidget            *mListWidget;
    bool                    mToggleOrientation;
    bool                    mTBone;
    QItemSelectionModel     *mSelectionModel;
    HgFlipWidget*           mFlipWidget;
    QModelIndex             mFlippedIndex;
    HbLabel*                mFrontItem;
    HbAction*               mUseLowResAction;
};


#endif /* HGWIDGETTESTVIEW_H_ */
