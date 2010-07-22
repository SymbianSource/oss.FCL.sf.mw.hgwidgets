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
* Description:   Multiselection dialog
*
*/

#ifndef HGSELECTIONDIALOG_H
#define HGSELECTIONDIALOG_H

#include <HbDialog>

class HbPushButton;
class HbLabel;
class HgWidget;

class HgSelectionDialog : public HbDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(HgSelectionDialog)

public:
    HgSelectionDialog(const QString &title, const QString &primaryText, HgWidget *content, QGraphicsItem *parent=0);
    ~HgSelectionDialog();

private slots:
    void updateItems();
    void selectAll();

private:
    HgWidget *mHgWidget;
    HbPushButton *mSelectAll;
    HbLabel *mCountLabel;
    HbAction* mPrimaryAction;
};

#endif  //HGSELECTIONDIALOG_H
