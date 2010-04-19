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

#include <hbaction.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <hgwidgets/hgwidgets.h>
#include <QGraphicsLinearLayout>
#include "hgselectiondialog.h"
#include "trace.h"

HgSelectionDialog::HgSelectionDialog(
    const QString &title, const QString &primaryText,
    HgWidget *content, QGraphicsItem *parent) :
    HbDialog(parent),
    mHgWidget(content),
    mSelectAll(new HbPushButton("---")),
    mCountLabel(new HbLabel("--/--"))
{
    FUNC_LOG;

    setDismissPolicy(HbDialog::NoDismiss);
    setTimeout(HbDialog::NoTimeout);
    setHeadingWidget(new HbLabel(title, this));
    setPrimaryAction(new HbAction(primaryText, this));
    primaryAction()->setDisabled(true);
    setSecondaryAction(new HbAction("Cancel", this));

    connect(content->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            SLOT(updateItems()));
    connect(content->model(),
            SIGNAL(rowsInserted(QModelIndex, int, int)),
            SLOT(updateItems()));
    connect(content->model(),
            SIGNAL(rowsRemoved(QModelIndex, int, int)),
            SLOT(updateItems()));

    HbWidget *contentWidget = new HbWidget;
    HANDLE_ERROR_NULL(contentWidget);
    if (contentWidget) {
        QGraphicsLinearLayout *contentLayout = new QGraphicsLinearLayout(Qt::Vertical);
        HANDLE_ERROR_NULL(contentLayout);
        if (contentLayout) {
            QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout(Qt::Horizontal);
            HANDLE_ERROR_NULL(topLayout);
            if (topLayout) {
                HANDLE_ERROR_NULL(mSelectAll);
                if (mSelectAll) {
                    topLayout->addItem(mSelectAll);
                    connect(mSelectAll, SIGNAL(clicked()), SLOT(selectAll()));
                }
                if (mCountLabel) {
                    mCountLabel->setAlignment(Qt::AlignRight);
                    topLayout->addItem(mCountLabel);
                }

                contentLayout->addItem(topLayout);
            }

            contentLayout->addItem(mHgWidget);
            contentWidget->setLayout(contentLayout);
        }
        setContentWidget(contentWidget);
    }

    updateItems();
}

HgSelectionDialog::~HgSelectionDialog()
{
    FUNC_LOG;
}

void HgSelectionDialog::updateItems()
{
    FUNC_LOG;

    if (mHgWidget) {
        QItemSelectionModel *selectionModel = mHgWidget->selectionModel();
        QAbstractItemModel *model = mHgWidget->model();
        if (selectionModel && model) {
            primaryAction()->setEnabled(selectionModel->hasSelection());

            int selectedCount = selectionModel->selectedIndexes().count();
            int itemCount = model->rowCount();

            if (selectedCount == itemCount) {
                mSelectAll->setText("Unselect all");
                mSelectAll->setChecked(false);
            }
            else {
                mSelectAll->setText("Select all");
                mSelectAll->setChecked(false);
            }

            mCountLabel->setPlainText(tr("%1/%2").arg(selectedCount).arg(itemCount));
        }
    }
}

void HgSelectionDialog::selectAll()
{
    FUNC_LOG;

    if (mHgWidget) {
        QItemSelectionModel *selectionModel = mHgWidget->selectionModel();
        QAbstractItemModel *model = mHgWidget->model();
        if (selectionModel && model) {
            int selectedCount = selectionModel->selectedIndexes().count();
            int itemCount = model->rowCount();

            if (selectedCount == itemCount) {
                mHgWidget->clearSelection();
            }
            else {
                mHgWidget->selectAll();
            }
        }
    }
}
