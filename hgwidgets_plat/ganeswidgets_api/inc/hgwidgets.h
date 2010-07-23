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

#ifndef HGWIDGET_H
#define HGWIDGET_H

#include <QItemSelectionModel>
#include <HbGlobal>
#include <hbnamespace.h>
#include <HbWidget>

#ifdef BUILD_HG_WIDGETS
#    define HG_WIDGETS_EXPORT Q_DECL_EXPORT
#    define HG_WIDGETS_PRIVATE_EXPORT Q_DECL_EXPORT
#else
#    define HG_WIDGETS_EXPORT Q_DECL_IMPORT
#    define HG_WIDGETS_PRIVATE_EXPORT Q_DECL_IMPORT
#endif // BUILD_HG_WIDGETS

class HgWidgetPrivate;
class HbScrollBar;

class HG_WIDGETS_EXPORT HgWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(bool longPressEnabled  READ longPressEnabled  WRITE setLongPressEnabled )
    Q_PROPERTY(ScrollBarPolicy scrollBarPolicy READ scrollBarPolicy WRITE setScrollBarPolicy)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode)
    Q_PROPERTY(IndexFeedbackPolicy IndexFeedbackPolicy READ indexFeedbackPolicy WRITE setIndexFeedbackPolicy)
    Q_PROPERTY(ItemSizePolicy ItemSizePolicy READ itemSizePolicy WRITE setItemSizePolicy)
    Q_PROPERTY(QSizeF itemSize READ itemSize WRITE setItemSize)
    Q_PROPERTY(QSizeF itemSpacing READ itemSpacing WRITE setItemSpacing)
    Q_ENUMS(ScrollBarPolicy)
    Q_ENUMS(HgWidgetType)
    Q_ENUMS(SelectionMode)
    Q_ENUMS(IndexFeedbackPolicy)
    Q_ENUMS(ItemSizePolicy)

public:

    enum HgDataRole
    {
        HgVisibilityRole = Qt::UserRole + 1
    };

    enum IndexFeedbackPolicy {
        IndexFeedbackNone = 0,
        IndexFeedbackSingleCharacter,
        IndexFeedbackThreeCharacter,
        IndexFeedbackString
    };

    enum ScrollBarPolicy {
        ScrollBarAsNeeded = Qt::ScrollBarAsNeeded,
        ScrollBarAlwaysOff = Qt::ScrollBarAlwaysOff,
        ScrollBarAlwaysOn = Qt::ScrollBarAlwaysOn,
        ScrollBarAutoHide
    };

    enum ItemSizePolicy {
        ItemSizeAutomatic,
        ItemSizeUserDefined
    };

    explicit HgWidget (QGraphicsItem *parent = 0);
    virtual ~HgWidget ();

    virtual void setModel(QAbstractItemModel *model);
    QAbstractItemModel *model() const;

    void setSelectionModel(QItemSelectionModel *selectionModel);
    QItemSelectionModel *selectionModel() const;

    enum SelectionMode {
        NoSelection = 0,
        SingleSelection,
        MultiSelection,
        ContiguousSelection
    };

    SelectionMode selectionMode() const;
    void setSelectionMode(SelectionMode mode, bool resetSelection = true);
    void selectAll();
    void clearSelection();

    QModelIndex currentIndex() const;
    void setCurrentIndex(const QModelIndex &index,
        QItemSelectionModel::SelectionFlags selectionFlag=QItemSelectionModel::NoUpdate);

    void scrollTo(const QModelIndex &index);

    bool longPressEnabled() const;
    void setLongPressEnabled(bool value);

    ScrollBarPolicy scrollBarPolicy() const;
    void setScrollBarPolicy(ScrollBarPolicy policy);
    HbScrollBar *scrollBar() const;
    void setScrollBar(HbScrollBar *scrollBar);

    bool getItemOutline(const QModelIndex& index, QPolygonF& points);
    Qt::Orientation scrollDirection() const;

    QList<QModelIndex> getVisibleItemIndices() const;

    void setIndexFeedbackPolicy( IndexFeedbackPolicy policy);
    IndexFeedbackPolicy indexFeedbackPolicy() const;

    void setDefaultImage(QImage defaultImage);

    void setItemSizePolicy(ItemSizePolicy policy);
    ItemSizePolicy itemSizePolicy() const;

    void setItemSize(const QSizeF& size);
    QSizeF itemSize() const;

    void setItemSpacing(const QSizeF& spacing);
    QSizeF itemSpacing() const;

signals:
    void activated(const QModelIndex &index);
    void longPressed(const QModelIndex &index, const QPointF &coords);
    void scrollingStarted();
    void scrollingEnded();
public slots:

    void aboutToChangeOrientation();
    void orientationChanged(Qt::Orientation orientation);

protected slots:

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

protected:
    HgWidgetPrivate * const p_ptr;
    HgWidget(HgWidgetPrivate &dd, QGraphicsItem *parent);

    bool eventFilter(QObject *obj,QEvent *event);

    bool event(QEvent *event);

private:
    Q_DECLARE_PRIVATE_D(p_ptr, HgWidget)
    Q_DISABLE_COPY(HgWidget)
    Q_PRIVATE_SLOT(d_func(), void _q_scrollPositionChanged(qreal index, bool scrollBarAnimation))
    Q_PRIVATE_SLOT(d_func(), void _q_releaseItems(int releaseStart, int releaseEnd))
    Q_PRIVATE_SLOT(d_func(), void _q_requestItems(int requestStart, int requestEnd))
    Q_PRIVATE_SLOT(d_func(), void _q_hideScrollBars() )
    Q_PRIVATE_SLOT(d_func(), void _q_thumbPositionChanged(qreal value, Qt::Orientation orientation))
    Q_PRIVATE_SLOT(d_func(), void _q_insertRows(const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_removeRows(const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_moveRows(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow))
    Q_PRIVATE_SLOT(d_func(), void _q_groovePressed(qreal, Qt::Orientation))
    Q_PRIVATE_SLOT(d_func(), void _q_modelReset())
    Q_PRIVATE_SLOT(d_func(), void _q_updateCurrentItem(const QModelIndex &current, const QModelIndex &previous))
};

#endif  //HGWIDGET_H

