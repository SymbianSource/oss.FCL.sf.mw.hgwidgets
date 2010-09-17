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
* Description: Container for pan (drag) gesture -related data and logic.
*
*/

#include <QtTest/QtTest>
#include <QMetaType>
#include <QModelIndex>
#include "hbautotest.h"
#include <HbApplication>
#include <HbMainWindow>
#include <HbScrollbar>
#include <HbView>
#include <hgwidgets/hgwidgets.h>
#include <hgwidgets/hggrid.h>
#include <hgwidgets/hgmediawall.h>

Q_DECLARE_METATYPE(QItemSelection)
Q_DECLARE_METATYPE(QModelIndex)

static const QPoint grid_portrait_pos0(60, 60);
static const QPoint grid_portrait_pos1(180, 60);
static const QPoint grid_portrait_pos2(300, 60);
static const QPoint grid_portrait_pos3(60, 180);
static const QPoint grid_portrait_pos4(180, 180);
static const QPoint grid_portrait_pos5(300, 180);
static const QPoint grid_portrait_pos6(60, 300);
static const QPoint grid_portrait_pos7(180, 300);
static const QPoint grid_portrait_pos8(300, 300);

static const int default_delay(1500);

class TestGanesWidgets : public QObject
{
    Q_OBJECT

public:

    TestGanesWidgets();
    virtual ~TestGanesWidgets();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void test_setModel();
    void test_panGridLandscape();
    void test_panCoverFlowLandscape();
    void test_scrollbarGridLandscape();
    void test_scrollbarGridPortrait();
    void test_scrollbarCoverFlowLandscape();
    void test_scrollbarCoverFlowPortrait();
    void test_addRemoveItemsGrid();
    void test_addRemoveItemsCoverflow();
    void test_updateData();
    void test_tap();
    void test_currentItemCoverflow();
    void test_currentItemGrid();
    void test_selectionMode();
    void test_selectionModel();
    void test_scrollTo();
    void test_addItemsCoverFlow();
    void test_removeItemsCoverFlow();
    void test_moveItemsCoverFlow();
    void test_labelFontSpecsCoverFlow();
    void test_resetModelCoverFlow();
    void test_resetModelGrid();
    void test_orientationChanged();
    void test_indexFeedback();
    void test_gridApi();
    
private:

    void pan( Qt::Orientation, TBool begin );

private:

    HbMainWindow* mWindow;
    HgWidget* mWidget;
    HgMediawall* mMediawall;

};

class TestModel : public QAbstractListModel
{
    Q_OBJECT

public:

    explicit TestModel(QList<QModelIndex> *requestedIndexes = 0);
    virtual ~TestModel();

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

    void generateItems(int count);
    void appendItem();
    void insertItems(int index, int count=1);
    void removeItems(int index, int count=1);
    void moveItems(int from, int to, int count=1);
    void changeItem(int index);
    void reset(int newItemCount=0);
    
    QImage mImage;
    QStringList mItems;
    bool mValidData;

    enum TextCount {
        ProvideNone,
        ProvideOne,
        ProvideTwo
    } mTextCount;
    
    enum ImageType {
        TypeQImage,
        TypeQPixmap,
        TypeQIcon,
        TypeHbIcon
    } mImageType;

    bool mItemVisibility;
    bool mProvideValidImage;
    
    QList<QModelIndex> *mRequestedIndexes;
};

TestModel::TestModel(QList<QModelIndex> *requestedIndexes) :
    mValidData(true),
    mRequestedIndexes(requestedIndexes),
    mTextCount(ProvideTwo),
    mImageType(TypeQImage),
    mItemVisibility(true),
    mProvideValidImage(true)
{
    mImage = QImage(":icons/startupHG.jpg").scaled(QSize(120,120));   
}

TestModel::~TestModel()
{

}

void TestModel::generateItems(int count)
{
    for (int i=0; i<count; i++) {
        mItems.append(QString("Item ").append(i));
    }
}

void TestModel::appendItem()
{
    insertItems(mItems.count(),1);
}

void TestModel::insertItems(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index+count-1); // Inclusive
    int end = index+count;
    for ( ;index<end; index++) {
        mItems.insert(index, QString("Item ").append(mItems.count()));
    }
    endInsertRows();
}

void TestModel::removeItems(int index, int count)
{
    if (index >= 0 && index < mItems.count()) {
        beginRemoveRows(QModelIndex(), index, index+count-1); // Inclusive
        int end = index+count;
        for (; index<end; index++) {
            mItems.removeAt(index);
        }
        endRemoveRows();
    }
}

void TestModel::moveItems(int from, int to, int count)
{
    if (beginMoveRows(QModelIndex(), from, from+count-1, QModelIndex(), to)) { // Inclusive
        // No need to actually move rows here
        endMoveRows();
    }
    else {
        qDebug() << "Cannot move" << from << "-" << from+count-1 << "to" << to;
    }
}

void TestModel::reset(int newItemCount)
{
    beginResetModel();
    mItems.clear();
    if (newItemCount > 0) {
        generateItems(newItemCount);
    }
    endResetModel();
}

void TestModel::changeItem(int index)
{
    if ( index >= 0 && index < mItems.count() ) {
        QModelIndex modelIndex = QAbstractItemModel::createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}

int TestModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return mItems.count();
}

QVariant TestModel::data(const QModelIndex &index, int role) const
{
    QVariant returnValue;

    if( !mValidData)
        return returnValue;

    int row = index.row();

    switch ( role )
        {
        case HgWidget::HgVisibilityRole:
            {
            returnValue = mItemVisibility;
            break;
            }
        case Qt::DisplayRole:
            {
            QStringList list;
            switch(mTextCount) {
                case ProvideOne:
                    {
                    list << QString("Primary %1").arg(row);
                    break;
                    }
                case ProvideTwo:
                    {
                    list << QString("Primary %1").arg(row);
                    list << QString("Secondary %1").arg(row);
                    break;
                    }
                case ProvideNone:
                default:
                    {
                    break;
                    }
            }
            returnValue = list;
            break;
            }
        case Qt::DecorationRole:
            {
            switch(mImageType){
                case TypeQImage:
                    {
                    if( mProvideValidImage)
                        returnValue = QImage();
                    else
                        returnValue = mImage;
                    break;
                    }
                case TypeQIcon:
                    {
                    if( mProvideValidImage)
                        returnValue = QIcon();
                    else
                        returnValue = QIcon(":icons/startupHG.jpg");
                    break;
                    }
                case TypeHbIcon:
                    {
                    if( mProvideValidImage)
                        returnValue = HbIcon();
                    else
                        returnValue = HbIcon(":icons/startupHG.jpg");                
                    break;
                    }
                case TypeQPixmap:
                    {
                    if( mProvideValidImage)
                        returnValue = QPixmap();
                    else
                        returnValue = QPixmap(":icons/startupHG.jpg");                
                    break;
                    }
            }
            
            if (mRequestedIndexes && !mRequestedIndexes->contains(index)) {
                mRequestedIndexes->append(index);
                qSort(*mRequestedIndexes);
            }
            break;
            }
        case Hb::IndexFeedbackRole:
            {
            returnValue = QString::number(row);
            break;
            }
        default:
            break;

        }

    return returnValue;
}


TestGanesWidgets::TestGanesWidgets()
{
    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWindow->show();
    mWindow->setGeometry(QRect(0,0,360,640));
    QTest::qWaitForWindowShown(mWindow);
}

TestGanesWidgets::~TestGanesWidgets()
{
    delete mWindow;
    mWindow = 0;
}

void TestGanesWidgets::initTestCase()
{

}

void TestGanesWidgets::cleanupTestCase()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
}

void TestGanesWidgets::init()
{
}

void TestGanesWidgets::cleanup()
{
}

void TestGanesWidgets::pan( Qt::Orientation orientation, TBool begin )
{
    QPoint start(100,100);
    QPoint move;
    QPoint end;
    if (orientation==Qt::Horizontal){
        move = QPoint(100,0);
    }
    else {
        move = QPoint(0,100);
    }

    if( begin )
        end = start - move;
    else
        end = start + move;

//    QTest::mousePress(mWindow,Qt::LeftButton,Qt::NoModifier,start,-1);
//    QTest::mouseMove(mWindow,end,-1);
//    QTest::mouseRelease(mWindow,Qt::LeftButton,Qt::NoModifier,end,100);
    HbAutoTest::mousePress( (HbAutoTestMainWindow*)mWindow, mWidget, start, -1 );
    HbAutoTest::mouseMove( (HbAutoTestMainWindow*)mWindow, mWidget, end, -1 );
    HbAutoTest::mouseRelease( (HbAutoTestMainWindow*)mWindow, mWidget, end, 100 );
}

void TestGanesWidgets::test_setModel()
{
//    mWindow = new HbMainWindow;
    mWidget = new HgGrid(Qt::Horizontal);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    QVERIFY(mWidget->model() == 0);

    TestModel model1;
    model1.generateItems(10);
    mWidget->setModel(&model1);
    QVERIFY(&model1 == mWidget->model());

//    QTest::qWait(2000);

    TestModel model2;
    model2.generateItems(20);
    mWidget->setModel(&model2);
    QVERIFY(&model2 == mWidget->model());

    //QTest::qWait(2000);

    mWidget->setModel(0);
    QVERIFY(mWidget->model() == 0);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_panGridLandscape()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }

    
    mWidget = new HgGrid(Qt::Horizontal);
    TestModel model;
    model.generateItems(30);
    mWindow->addView( mWidget );
    QVERIFY( mWidget->model() == 0 );
    mWidget->setModel( &model );
    QVERIFY( &model == mWidget->model() );

    mWindow->show();

    //QTest::qWait( 2000 );

    pan( Qt::Horizontal, true );

    model.reset();
    model.generateItems(5);

    QTest::qWait( 1000 );

    pan( Qt::Horizontal, false );

    model.reset();

    QTest::qWait( 1000 );

    pan( Qt::Horizontal, true );

    QTest::qWait(1000);

    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
}


void TestGanesWidgets::test_panCoverFlowLandscape()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    mWidget = new HgMediawall();
    TestModel model;
    model.generateItems(30);
    mWindow->addView( mWidget );
    QVERIFY( mWidget->model() == 0 );
    mWidget->setModel( &model );
    QVERIFY( &model == mWidget->model() );
    mWindow->show();

    QTest::qWait( 2000 );

    pan( Qt::Horizontal, true );

    model.reset();
    model.generateItems(5);

    QTest::qWait( 2000 );

    pan( Qt::Horizontal, false );

    model.reset();

    QTest::qWait( 2000 );

    pan( Qt::Horizontal, true );

    QTest::qWait(2000);

    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
}

void TestGanesWidgets::test_scrollbarGridLandscape()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }

//    mWindow = new HbMainWindow;
    mWidget = new HgMediawall();
    TestModel model;
    model.generateItems(200);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
//    mWindow->show();
//    QTest::qWaitForWindowShown(mWindow);

    //QTest::qWait( 2000 );

    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);

    mWidget->setScrollBarPolicy(HgWidget::ScrollBarAlwaysOn);
    mWidget->scrollBar()->setInteractive(true);
    QRectF rect = mWidget->scrollBar()->rect();

    QTest::qWait(1000);

    QPoint move( 100,0 );

//    QTest::mouseClick(mWindow, Qt::LeftButton, Qt::NoModifier, rect.topLeft().toPoint() + move );
//    QTest::mouseClick(mWindow, Qt::LeftButton, Qt::NoModifier, rect.topLeft().toPoint() + move*2 );
    HbAutoTest::mousePress( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topLeft()+move, -1 );
    HbAutoTest::mouseMove( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight()-move, 50 );
    HbAutoTest::mouseRelease( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight()-move, 100 );

    //QTest::qWait(3000);

    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOn );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOn);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOff );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOff);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAutoHide );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);

    QVERIFY(mWidget->scrollBar() != 0);
    HbScrollBar* scrollBar = new HbScrollBar();
    mWidget->setScrollBar(scrollBar);
    QVERIFY(mWidget->scrollBar()==scrollBar);
    mWidget->setScrollBar(0);
    QVERIFY(mWidget->scrollBar()!= 0);

    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;

}

void TestGanesWidgets::test_scrollbarGridPortrait()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWidget = new HgGrid(Qt::Vertical);
    TestModel model;
    model.generateItems(200);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
    mWindow->show();

    //QTest::qWait( 2000 );

    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOn );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOn);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOff );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOff);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAutoHide );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);

    QVERIFY(mWidget->scrollBar() != 0);
    HbScrollBar* scrollBar = new HbScrollBar();
    mWidget->setScrollBar(scrollBar);
    QVERIFY(mWidget->scrollBar()==scrollBar);
    mWidget->setScrollBar(0);
    QVERIFY(mWidget->scrollBar()!= 0);

    //QTest::qWait(2000);
    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;

//    delete mWindow;
//    mWindow = 0;

}

void TestGanesWidgets::test_scrollbarCoverFlowLandscape()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mMediawall = new HgMediawall();
    mWidget = mMediawall;
    TestModel model;
    model.generateItems(200);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
    mWindow->show();

    //QTest::qWait( 2000 );

    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOn );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOn);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOff );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOff);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAutoHide );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);

    QVERIFY(mWidget->scrollBar() != 0);
    HbScrollBar* scrollBar = new HbScrollBar();
    mWidget->setScrollBar(scrollBar);
    QVERIFY(mWidget->scrollBar()==scrollBar);
    mWidget->setScrollBar(0);
    QVERIFY(mWidget->scrollBar()!= 0);

    QTest::qWait(2000);

    
    
    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;    
    
//    delete mWindow;
//    mWindow = 0;

}

void TestGanesWidgets::test_scrollbarCoverFlowPortrait()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWidget = new HgMediawall();
    TestModel model;
    model.generateItems(200);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
    mWindow->show();

    //QTest::qWait( 2000 );

    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOn );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOn);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAlwaysOff );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAlwaysOff);
    mWidget->setScrollBarPolicy( HgWidget::ScrollBarAutoHide );
    QVERIFY(mWidget->scrollBarPolicy() == HgWidget::ScrollBarAutoHide);

    QVERIFY(mWidget->scrollBar() != 0);
    HbScrollBar* scrollBar = new HbScrollBar();
    mWidget->setScrollBar(scrollBar);
    QVERIFY(mWidget->scrollBar()==scrollBar);
    mWidget->setScrollBar(0);
    QVERIFY(mWidget->scrollBar()!= 0);

    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;

}

void TestGanesWidgets::test_addRemoveItemsGrid()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWidget = new HgMediawall();
    TestModel model;
    model.generateItems(2);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
    mWindow->show();

    //QTest::qWait( 2000 );

    model.appendItem();
    model.appendItem();
    model.removeItems(0);
    model.removeItems(3);
    model.removeItems(0);
    model.removeItems(0);

    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_addRemoveItemsCoverflow()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWidget = new HgMediawall();
    TestModel model;
    model.generateItems(2);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
    mWindow->show();

    //QTest::qWait( 2000 );

    model.appendItem();
    model.appendItem();
    model.removeItems(0);
    model.removeItems(3);
    model.removeItems(0);
    model.removeItems(0);

    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_tap()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    mWidget = new HgMediawall();
    TestModel model;
    model.generateItems(50);
    mWindow->addView( mWidget );
    mWidget->setModel( &model );
    mWindow->show();

    QTest::qWait( 2000 );

    QSignalSpy stateSpy( mWidget, SIGNAL( activated(QModelIndex) ) );
    QSignalSpy stateSpy2( mWidget, SIGNAL( longPressed(QModelIndex) ) );

    QPoint pos(100,300);
//    QTest::mouseClick(mWindow, Qt::LeftButton, Qt::NoModifier, pos );
    HbAutoTest::mouseClick( (HbAutoTestMainWindow*)mWindow, mWidget, pos, 100 );

    QTest::qWait(3000);

    // Generating gestures doesn't work so enable this condition later.
//    QCOMPARE(stateSpy.count(),1);

    QVERIFY(!mWidget->longPressEnabled());
    mWidget->setLongPressEnabled(true);
    QVERIFY(mWidget->longPressEnabled());
    mWidget->setLongPressEnabled(true);
    QVERIFY(mWidget->longPressEnabled());

    HbAutoTest::mouseClick( (HbAutoTestMainWindow*)mWindow, mWidget, pos, 700 );
//    QTest::mouseRelease( mWindow, Qt::LeftButton, Qt::NoModifier, pos, 400 );

    // Generating gestures doesn't work so enable this condition later.
//    QCOMPARE( stateSpy2.count(),1 );

    mWidget->setLongPressEnabled(false);
    QVERIFY(!mWidget->longPressEnabled());

    QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;

    mWidget = new HgGrid(Qt::Vertical);
    mWindow->addView( mWidget );
    mWidget->setModel( &model );
    mWindow->show();

    QVERIFY(!mWidget->longPressEnabled());
    mWidget->setLongPressEnabled(true);
    QVERIFY(mWidget->longPressEnabled());

    QTest::qWait(2000);
    HbAutoTest::mouseClick( (HbAutoTestMainWindow*)mWindow, mWidget, pos, 700 );
    QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
}

void TestGanesWidgets::test_updateData()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWidget = new HgGrid( Qt::Vertical );
    TestModel model;
    model.generateItems(50);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    model.mValidData = false;
    mWidget->setModel( &model );
    mWindow->show();

    //QTest::qWait( 2000 );

    model.mValidData = true;
    for(int i=0;i<50;i++){
        model.changeItem(i);
    }

    // Test various data types that are supported.
    model.mImageType = TestModel::TypeQIcon;
    model.mTextCount = TestModel::ProvideNone;
    model.reset(50);
    QTest::qWait(2000);
    model.mProvideValidImage = false;
    model.reset(50);
    QTest::qWait(2000);
    
    model.mProvideValidImage = true;
    model.mImageType = TestModel::TypeQPixmap;
    model.mTextCount = TestModel::ProvideOne;
    model.reset(50);
    QTest::qWait(2000);
    model.mProvideValidImage = false;
    model.reset(50);
    QTest::qWait(2000);
    
    model.mProvideValidImage = true;
    model.mImageType = TestModel::TypeHbIcon;
    model.mTextCount = TestModel::ProvideTwo;
    model.reset(50);
    QTest::qWait(2000);
    model.mProvideValidImage = false;
    model.reset(50);
    QTest::qWait(2000);
    
    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;

//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_currentItemCoverflow()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    const QPoint pos1(160, 300);
    const QPoint pos2(300, 300);
    const QPoint pos3(20, 300);

    mWidget = new HgMediawall();

    TestModel model;
    model.generateItems(50);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel(&model);
    mWindow->show();

    QVERIFY(mWidget->selectionModel());
    qRegisterMetaType<QModelIndex>("QModelIndex");
    QSignalSpy currentItemSpy(mWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)));

    //QTest::qWait(default_delay);

    QVERIFY(mWidget->currentIndex() == model.index(0, 0));

    mWidget->setCurrentIndex(model.index(7, 0));
    QVERIFY(mWidget->currentIndex() == model.index(7, 0));

    mWidget->setCurrentIndex(QModelIndex());
    QVERIFY(!mWidget->currentIndex().isValid());

    currentItemSpy.clear();
//    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, pos1, 100 );
//    QTest::mouseRelease( mWindow, Qt::LeftButton, Qt::NoModifier, pos, 2000 );
/*    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, pos1, 100);
    QTest::qWait(500);
    mWidget->update();
    QTest::qWait(100);
//    User::After(3000000);
//    mWidget->update();
//    User::After(3000000);
    QVERIFY(mWidget->currentIndex() == model.index(7, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(0, 0));

    currentItemSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, pos2, -1);
    //QTest::qWait(default_delay);
    QVERIFY(mWidget->currentIndex() == model.index(1, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(1, 0));

    currentItemSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, pos2, -1);
    //QTest::qWait(default_delay);
    QVERIFY(mWidget->currentIndex() == model.index(2, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(2, 0));

    currentItemSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, pos2, -1);
    //QTest::qWait(default_delay);
    QVERIFY(mWidget->currentIndex() == model.index(3, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(3, 0));

    currentItemSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, pos3, -1);
    //QTest::qWait(default_delay);
    QVERIFY(mWidget->currentIndex() == model.index(2, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(2, 0));

    currentItemSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, pos3, -1);
    //QTest::qWait(default_delay);
    QVERIFY(mWidget->currentIndex() == model.index(1, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(1, 0));

    currentItemSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, pos3, -1);
    //QTest::qWait(default_delay);
    QVERIFY(mWidget->currentIndex() == model.index(0, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(0, 0));

    currentItemSpy.clear();
    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, pos3, -1);
    //QTest::qWait(default_delay);
    QVERIFY(mWidget->currentIndex() == model.index(0, 0));
    QVERIFY(currentItemSpy.count() == 0);
*/
    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
    //QTest::qWait(default_delay);

//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_currentItemGrid()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWidget = new HgGrid( Qt::Vertical);
    mWidget->setItemSizePolicy(HgWidget::ItemSizeUserDefined);
    QVERIFY(mWidget->itemSizePolicy() == HgWidget::ItemSizeUserDefined);
    mWidget->setItemSize(QSizeF(120, 120));
    QVERIFY(mWidget->itemSize() == QSizeF(120,120));
    mWidget->setItemSpacing(QSizeF(0,0));
    QVERIFY(mWidget->itemSpacing() == QSizeF(0,0));
    QImage defaultIcon = QImage(":icons/startupHG.jpg").scaled(QSize(120,120));
    mWidget->setDefaultImage(defaultIcon);
    
    
    TestModel model;
    model.generateItems(50);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel(&model);
    mWindow->show();

    QVERIFY(mWidget->selectionModel());
    qRegisterMetaType<QModelIndex>("QModelIndex");
    QSignalSpy currentItemSpy(mWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)));

    //QTest::qWait(2000);

    QVERIFY(mWidget->currentIndex() == model.index(0, 0));

    mWidget->setCurrentIndex(model.index(7, 0));
    QVERIFY(mWidget->currentIndex() == model.index(7, 0));

    mWidget->setCurrentIndex(QModelIndex());
    QVERIFY(!mWidget->currentIndex().isValid());

    currentItemSpy.clear();
    
    //    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, grid_portrait_pos1, -1);
    //QTest::qWait(1000);
//    mWindow->layout()->activate();
//    mWidget->hide();
//    mWidget->show();
//    mWidget->
/*    mWidget->scrollTo(model.index(1,0));
//    QTest::qWait(1000);
    QVERIFY(mWidget->currentIndex() == model.index(1, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(1, 0));

    currentItemSpy.clear();
//    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, grid_portrait_pos4, -1);
    mWidget->scrollTo(model.index(4,0));
    //QTest::qWait(1000);
    QVERIFY(mWidget->currentIndex() == model.index(4, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(4, 0));

    currentItemSpy.clear();
//    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, grid_portrait_pos4, -1);
    mWidget->scrollTo(model.index(4,0));
    //QTest::qWait(1000);
    QVERIFY(mWidget->currentIndex() == model.index(4, 0));
    QVERIFY(currentItemSpy.count() == 0);

    currentItemSpy.clear();
//    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, grid_portrait_pos8, -1);
    //QTest::qWait(1000);
    mWidget->scrollTo(model.index(8,0));
    QVERIFY(mWidget->currentIndex() == model.index(8, 0));
    QVERIFY(currentItemSpy.count() == 1);
    QVERIFY(currentItemSpy.at(0).count() > 0);
    QVERIFY(qvariant_cast<QModelIndex>(currentItemSpy.at(0).at(0)) == model.index(8, 0));
*/
    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_selectionMode()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWidget = new HgGrid( Qt::Vertical);

    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWindow->show();
    //QTest::qWaitForWindowShown(mWindow);

    //QTest::qWait(2000);

    // Widget does not have selection model yet
    QVERIFY(mWidget->selectionModel() == 0);

    // Selection methods should have no effect unless there is a model
    mWidget->setSelectionMode(HgWidget::MultiSelection);
    QVERIFY(mWidget->selectionMode() == HgWidget::MultiSelection);
    QVERIFY(mWidget->selectionModel() == 0);
    mWidget->selectAll();
    QVERIFY(mWidget->selectionModel() == 0);

    TestModel model;
    model.generateItems(9);
    mWidget->setModel(&model);
    QVERIFY(mWidget->selectionModel() != 0);

    qRegisterMetaType<QItemSelection>("QItemSelection");
    QSignalSpy selectionSpy(mWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)));

    QTest::qWait(2000);

    mWidget->setSelectionMode(HgWidget::NoSelection);
    QVERIFY(mWidget->selectionMode() == HgWidget::NoSelection);
    // Default selection mode: no selection
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget,grid_portrait_pos0, -1);
    //QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 0);
    QVERIFY(selectionSpy.count() == 0);

    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos1, -1);
    //QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 0);
    QVERIFY(selectionSpy.count() == 0);

    mWidget->selectAll();
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 0);
    QVERIFY(selectionSpy.count() == 0);

    // Single selection mode: at most 1 item selected at a time
    mWidget->setSelectionMode(HgWidget::SingleSelection);
    QVERIFY(mWidget->selectionMode() == HgWidget::SingleSelection);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 0);

    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos1, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 1);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(1, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    QItemSelection selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(1, 0)));

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos3, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 1);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(3, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 1);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0)); // new selected
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(3, 0)));
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(1)); // deselected
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(1, 0)));

    selectionSpy.clear();
    mWidget->clearSelection();
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 0);
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 0);

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos8, -1);
    //QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 1);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(8, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(8, 0)));

    selectionSpy.clear();
    // Changing selection mode with default parameter should clear the selection
    mWidget->setSelectionMode(HgWidget::ContiguousSelection);
    QVERIFY(mWidget->selectionMode() == HgWidget::ContiguousSelection);

    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 0);
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 0);

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos2, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 1);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(2, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(2, 0)));

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos3, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 2);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(3, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(3, 0)));

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos5, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 4);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(4, 0)));
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(5, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 2);
    QVERIFY(selection.contains(model.index(4, 0)));
    QVERIFY(selection.contains(model.index(5, 0)));

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos0, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 6);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(0, 0)));
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(1, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 2);
    QVERIFY(selection.contains(model.index(0, 0)));
    QVERIFY(selection.contains(model.index(1, 0)));

    // In contiguous selection mode, clicking a selected item does nothing
    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos5, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 6);
    QVERIFY(selectionSpy.count() == 0);

    selectionSpy.clear();
    mWidget->selectAll();
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 9);
    QVERIFY(selectionSpy.count() == 1);

    selectionSpy.clear();
    // Changing selection mode with resetSelection = false should NOT clear the selection
    mWidget->setSelectionMode(HgWidget::MultiSelection, false);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 9);
    QVERIFY(selectionSpy.count() == 0);

    selectionSpy.clear();
    // In multiselection mode, clicking a selected item deselects it
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos5, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 8);
    QVERIFY(!(mWidget->selectionModel()->isSelected(model.index(5, 0))));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 1);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0)); // new selected
    QVERIFY(selection.indexes().count() == 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(1)); // deselected
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(5, 0)));

    selectionSpy.clear();
    mWidget->clearSelection();
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 0);
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 1);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0)); // new selected
    QVERIFY(selection.indexes().count() == 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(1)); // deselected
    QVERIFY(selection.indexes().count() == 8);

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos3, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 1);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(3, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 1);

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos5, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 2);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(5, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(5, 0)));

    selectionSpy.clear();
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos8, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 3);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(8, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0));
    QVERIFY(selection.indexes().count() == 1);
    QVERIFY(selection.contains(model.index(8, 0)));

    selectionSpy.clear();
    // Setting the same mode does nothing
    mWidget->setSelectionMode(HgWidget::MultiSelection);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 3);
    QVERIFY(selectionSpy.count() == 0);

    selectionSpy.clear();
    // Keep the selection, even if it is not suitable for the new selection mode
    mWidget->setSelectionMode(HgWidget::SingleSelection, false);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 3);
    QVERIFY(selectionSpy.count() == 0);

    selectionSpy.clear();
    // First click resets the selection to a valid setup
    HbAutoTest::mouseClick((HbAutoTestMainWindow*)mWindow, mWidget, grid_portrait_pos5, -1);
    QTest::qWait(1000);
    QVERIFY(mWidget->selectionModel()->selectedIndexes().count() == 1);
    QVERIFY(mWidget->selectionModel()->isSelected(model.index(5, 0)));
    QVERIFY(selectionSpy.count() == 1);
    QVERIFY(selectionSpy.at(0).count() > 1);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(0)); // new selected
    QVERIFY(selection.indexes().count() == 0);
    selection = qvariant_cast<QItemSelection>(selectionSpy.at(0).at(1)); // deselected
    QVERIFY(selection.indexes().count() == 2);
    QVERIFY(selection.contains(model.index(3, 0)));
    QVERIFY(selection.contains(model.index(8, 0)));

    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;    
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_selectionModel()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWidget = new HgGrid( Qt::Vertical);
    TestModel model;
    model.generateItems(9);
    mWidget->setModel(&model);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWindow->show();
//    mWindow->layout()->activate();
    //QTest::qWaitForWindowShown(mWindow);
    //QTest::qWait(2000);

    QVERIFY(mWidget->selectionModel() != 0);

    QItemSelectionModel *defaultSelectionModel = mWidget->selectionModel();
    QSignalSpy defaultSelectionSpy(defaultSelectionModel, SIGNAL(selectionChanged(QItemSelection, QItemSelection)));

    QItemSelectionModel *testSelectionModel1 = new QItemSelectionModel(&model);
    QSignalSpy testSelectionSpy1(testSelectionModel1, SIGNAL(selectionChanged(QItemSelection, QItemSelection)));

    QItemSelectionModel *testSelectionModel2 = new QItemSelectionModel(&model);
    QSignalSpy testSelectionSpy2(testSelectionModel2, SIGNAL(selectionChanged(QItemSelection, QItemSelection)));

    mWidget->setSelectionMode(HgWidget::MultiSelection);
/*
    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, grid_portrait_pos2, -1);
    //QTest::qWait(1000);
    QVERIFY(defaultSelectionModel->selectedIndexes().count() == 1);
    QVERIFY(defaultSelectionSpy.count() == 1);
    QVERIFY(testSelectionModel1->selectedIndexes().count() == 0);
    QVERIFY(testSelectionSpy1.count() == 0);
    QVERIFY(testSelectionModel2->selectedIndexes().count() == 0);
    QVERIFY(testSelectionSpy2.count() == 0);

    defaultSelectionSpy.clear();
    testSelectionSpy1.clear();
    testSelectionSpy2.clear();

    mWidget->setSelectionModel(testSelectionModel1);
    QTest::mouseClick( mWindow, Qt::LeftButton, Qt::NoModifier, grid_portrait_pos5, -1);
    //QTest::qWait(1000);
    // Default selection model is not valid any more
    QVERIFY(defaultSelectionSpy.count() == 0);
    QVERIFY(testSelectionModel1->selectedIndexes().count() == 1);
    QVERIFY(testSelectionSpy1.count() == 1);
    QVERIFY(testSelectionModel2->selectedIndexes().count() == 0);
    QVERIFY(testSelectionSpy2.count() == 0);

    defaultSelectionSpy.clear();
    testSelectionSpy1.clear();
    testSelectionSpy2.clear();

    mWidget->setSelectionModel(testSelectionModel2);
    mWidget->selectAll();
    QVERIFY(testSelectionModel1->selectedIndexes().count() == 1);
    QVERIFY(testSelectionSpy1.count() == 0);
    QVERIFY(testSelectionModel2->selectedIndexes().count() == 9);
    QVERIFY(testSelectionSpy2.count() == 1);

    defaultSelectionSpy.clear();
    testSelectionSpy1.clear();
    testSelectionSpy2.clear();

    // Setting the same selection model again does nothing
    mWidget->setSelectionModel(testSelectionModel2);
    QVERIFY(testSelectionModel1->selectedIndexes().count() == 1);
    QVERIFY(testSelectionSpy1.count() == 0);
    QVERIFY(testSelectionModel2->selectedIndexes().count() == 9);
    QVERIFY(testSelectionSpy2.count() == 0);

    defaultSelectionSpy.clear();
    testSelectionSpy1.clear();
    testSelectionSpy2.clear();

    mWidget->setSelectionModel(testSelectionModel1);
    mWidget->clearSelection();
    QVERIFY(testSelectionModel1->selectedIndexes().count() == 0);
    QVERIFY(testSelectionSpy1.count() == 1);
    QVERIFY(testSelectionModel2->selectedIndexes().count() == 9);
    QVERIFY(testSelectionSpy2.count() == 0);

    //QTest::qWait(2000);
*/
    delete testSelectionModel1;
    delete testSelectionModel2;

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_scrollTo()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    qRegisterMetaType<QModelIndex>("QModelIndex");
    qRegisterMetaType<QModelIndex>("QItemSelection");
    // TODO: How to verify that items are freed?

//    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWidget = new HgGrid( Qt::Vertical);
    QList<QModelIndex> requestedIndexes;
    TestModel model(&requestedIndexes);
    model.generateItems(1024);
    mWidget->setModel(&model);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWindow->show();
    QTest::qWait(2000);

    QVERIFY(requestedIndexes.count() == 120); // Scroll buffer size in grid mode is assumed to be 120
    QVERIFY(requestedIndexes.front() == model.index(0, 0));
    QVERIFY(requestedIndexes.back() == model.index(119, 0));
    requestedIndexes.clear();

    QSignalSpy activatedSpy(mWidget, SIGNAL(activated(QModelIndex)));
    QSignalSpy currentSpy(mWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)));
    QSignalSpy selectionSpy(mWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)));

    mWidget->scrollTo(model.index(100, 0));
    //QTest::qWait(1000);
    QVERIFY(activatedSpy.count() == 0); // scrollto doesn't activate item
//    QVERIFY(currentSpy.count() == 0); // scrollto doesn't change the current
    QVERIFY(selectionSpy.count() == 0); // scrollto doesn't change the selection
    int count = requestedIndexes.count();
    QVERIFY(requestedIndexes.count() == 39); // The whole scroll buffer should be updated
    QVERIFY(requestedIndexes.front() == model.index(120, 0));
    QVERIFY(requestedIndexes.back() == model.index(158, 0));
    requestedIndexes.clear();

    mWidget->scrollTo(model.index(1023, 0));
    //QTest::qWait(1000);
    QVERIFY(activatedSpy.count() == 0); // scrollto doesn't activate item
//    QVERIFY(currentSpy.count() == 0); // scrollto doesn't change the current
    QVERIFY(selectionSpy.count() == 0); // scrollto doesn't change the selection
    QVERIFY(requestedIndexes.count() == 120); // The whole scroll buffer should be updated
    int aa = requestedIndexes.front().row();
    int bb = requestedIndexes.back().row();
    QVERIFY(requestedIndexes.front() == model.index(904, 0));
    QVERIFY(requestedIndexes.back() == model.index(1023, 0));
    requestedIndexes.clear();

    mWidget->scrollTo(QModelIndex());
    //QTest::qWait(1000);
    QVERIFY(activatedSpy.count() == 0); // scrollto doesn't activate item
//    QVERIFY(currentSpy.count() == 0); // scrollto doesn't change the current
    QVERIFY(selectionSpy.count() == 0); // scrollto doesn't change the selection
    QVERIFY(requestedIndexes.count() == 0); // Items are not re-fetched from model

    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;

    // Test scrollTo before widget activation
    mWidget = new HgGrid( Qt::Vertical);
    mWidget->setModel(&model);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWindow->show();
    mWidget->scrollTo(model.index(200,0));
    mWidget->scrollTo(model.index(0,0));
    QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;    
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_addItemsCoverFlow()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    // TODO: How to verify that items are freed?

//    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWidget = new HgMediawall();

    QList<QModelIndex> requestedIndexes;
    TestModel model(&requestedIndexes);
    model.generateItems(110);
    mWidget->setModel(&model);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWindow->show();
    model.reset(110);

    mWidget->update();
    QTest::qWait(2000);

    QVERIFY(requestedIndexes.count() == 25); // Scroll buffer size in coverflow mode is assumed to be 30
    int aa = requestedIndexes.front().row();
    int aa2 = requestedIndexes.back().row();
    QVERIFY(requestedIndexes.front() == model.index(0, 0));
    QVERIFY(requestedIndexes.back() == model.index(24, 0));
    requestedIndexes.clear();

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(95, 0));
    mWidget->scrollTo(model.index(95, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Add one item to beginning of buffer
    model.insertItems(80, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // Buffer is moved up to 81
    requestedIndexes.clear();
    // Last item is now 110

    // Add many items to beginning of buffer
    model.insertItems(81, 4);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // // Buffer is moved up to 85
    requestedIndexes.clear();
    // Last item is now 114
/*
    // Add one item to the end of the buffer
    model.insertItems(99, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // The new item is requested
    QVERIFY(requestedIndexes.front() == model.index(114, 0));
    requestedIndexes.clear();
    // Last item is now 115

    // Add many items to the end
    model.insertItems(106, 4);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 4); // The new items are requested
    QVERIFY(requestedIndexes.front() == model.index(111, 0));
    QVERIFY(requestedIndexes.back() == model.index(114, 0));
    requestedIndexes.clear();
    // Last item is now 119

    // Add one item to middle of buffer
    model.insertItems(100, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // The new item is requested
    QVERIFY(requestedIndexes.front() == model.index(100, 0));
    requestedIndexes.clear();
    // Last item is now 130

    // Add many items to middle of buffer
    model.insertItems(100, 4);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 4); // The new items are requested
    QVERIFY(requestedIndexes.front() == model.index(100, 0));
    QVERIFY(requestedIndexes.back() == model.index(103, 0));
    requestedIndexes.clear();
    // Last item is now 134

    // Add items to the buffer limit (beginning of buffer)
    model.insertItems(70, 20);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New item falls outside of buffer as buffer is moved up
    // Last item is now 154

    // Add items to outside of buffer (before buffer)
    model.insertItems(0, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // The new items are not requested
    requestedIndexes.clear();
*/
    // Move buffer to the beginning of items
    mWidget->setCurrentIndex(model.index(15, 0));
    mWidget->scrollTo(model.index(15, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Add one item to beginning
    model.insertItems(0, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // Buffer is moved up

    // Add many items to beginning
    model.insertItems(1, 5);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // Buffer is moved up

    // Add one item to middle of buffer
/*    model.insertItems(10, 1);
    QTest::qWait(1000);
    int cc = requestedIndexes.count();
    QVERIFY(requestedIndexes.count() == 1); // The new item is requested
    QVERIFY(requestedIndexes.front() == model.index(10, 0));
    requestedIndexes.clear();

    // Add many items to middle of buffer
    model.insertItems(10, 5);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 5); // The new items are requested
    QVERIFY(requestedIndexes.front() == model.index(10, 0));
    QVERIFY(requestedIndexes.back() == model.index(14, 0));
    requestedIndexes.clear();

    // Add one item to end of buffer
    model.insertItems(35, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // The new item is requested
    QVERIFY(requestedIndexes.front() == model.index(35, 0));
    requestedIndexes.clear();

    // Add many items to end of buffer
    model.insertItems(26, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 10); // The new items are requested
    QVERIFY(requestedIndexes.front() == model.index(26, 0));
    QVERIFY(requestedIndexes.back() == model.index(35, 0));
    requestedIndexes.clear();

    // Add items to outside of buffer (after buffer)
    model.insertItems(50, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // The new items are not requested
    requestedIndexes.clear();

    // Add items to the buffer limit (end of buffer)
    model.insertItems(31, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 5); // The new items inside buffer are requested
    QVERIFY(requestedIndexes.front() == model.index(31, 0));
    QVERIFY(requestedIndexes.back() == model.index(35, 0));

    // Move buffer to the middle of items
    mWidget->setCurrentIndex(model.index(60, 0));
    mWidget->scrollTo(model.index(60, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Add items to the buffer limit (beginning of buffer)
    model.insertItems(40, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // The buffer is moved up

    // Add items over the whole buffer
    model.insertItems(40, 50);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // The buffer is moved up
*/
    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_removeItemsCoverFlow()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    // TODO: How to verify that items are freed?

//    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWidget = new HgMediawall();

    QList<QModelIndex> requestedIndexes;
    TestModel model(&requestedIndexes);
    mWidget->setModel(&model);
    model.insertItems(0,230);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWindow->show();

    QTest::qWait(2000);

    QVERIFY(requestedIndexes.count() == 25); // Scroll buffer size in coverflow mode is assumed to be 40
    QVERIFY(requestedIndexes.front() == model.index(0, 0));
    QVERIFY(requestedIndexes.back() == model.index(24, 0));
    requestedIndexes.clear();

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(229, 0));
    mWidget->scrollTo(model.index(229, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Remove one item from the beginning of buffer
    model.removeItems(200, 1);
    //QTest::qWait(1000);
//    QVERIFY(requestedIndexes.count() == 1); // New item is fetched to replace the removed one
//    QVERIFY(requestedIndexes.front() == model.index(199, 0));
//    requestedIndexes.clear();
    // Last item is now 228
/*
    // Remove many items from beginning of buffer
    model.removeItems(199, 4);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 4); // New items are fetched to replace the removed ones
    QVERIFY(requestedIndexes.front() == model.index(195, 0));
    QVERIFY(requestedIndexes.back() == model.index(198, 0));
    requestedIndexes.clear();
    // Last item is now 224

    // Remove one item from the end
    model.removeItems(224, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // New item is fetched to replace the removed one
    QVERIFY(requestedIndexes.front() == model.index(194, 0));
    requestedIndexes.clear();
    // Last item is now 223

    // Remove many items from the end
    model.removeItems(220, 4);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 4); // New items are fetched to replace the removed ones
    QVERIFY(requestedIndexes.front() == model.index(190, 0));
    QVERIFY(requestedIndexes.back() == model.index(193, 0));
    requestedIndexes.clear();
    // Last item is now 219

    // Remove one item from the middle of buffer
    model.removeItems(210, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // New item is fetched to replace the removed one
    QVERIFY(requestedIndexes.front() == model.index(189, 0));
    requestedIndexes.clear();
    // Last item is now 218

    // Remove many items from the middle of buffer
    model.removeItems(210, 4);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 4); // New items are fetched to replace the removed ones
    QVERIFY(requestedIndexes.front() == model.index(185, 0));
    QVERIFY(requestedIndexes.back() == model.index(188, 0));
    requestedIndexes.clear();
    // Last item is now 214

    // Remove items from the buffer limit (beginning of buffer)
    model.removeItems(180, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 5); // New items are fetched to replace the removed ones
    QVERIFY(requestedIndexes.front() == model.index(180, 0));
    QVERIFY(requestedIndexes.back() == model.index(184, 0));
    requestedIndexes.clear();
    // Last item is now 204

    // Remove items from outside of buffer (before buffer)
    model.removeItems(0, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // Buffer is not moved
    requestedIndexes.clear();
    // Last item is now 194

    // Move buffer to the beginning of items
    mWidget->setCurrentIndex(model.index(0, 0));
    mWidget->scrollTo(model.index(0, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Remove one item from beginning
    model.removeItems(0, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // New item is fetched to replace the removed one
    QVERIFY(requestedIndexes.front() == model.index(29, 0));
    requestedIndexes.clear();
    // Last item is now 193

    // Remove many items from beginning
    model.removeItems(0, 5);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 5); // New items are fetched to replace the removed ones
    QVERIFY(requestedIndexes.front() == model.index(25, 0));
    QVERIFY(requestedIndexes.back() == model.index(29, 0));
    requestedIndexes.clear();
    // Last item is now 188

    // Remove one item from the middle of buffer
    model.removeItems(15, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // New item is fetched to replace the removed one
    QVERIFY(requestedIndexes.front() == model.index(29, 0));
    requestedIndexes.clear();
    // Last item is now 187

    // Remove many items from the middle of buffer
    model.removeItems(15, 5);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 5); // New items are fetched to replace the removed ones
    QVERIFY(requestedIndexes.front() == model.index(25, 0));
    QVERIFY(requestedIndexes.back() == model.index(29, 0));
    requestedIndexes.clear();
    // Last item is now 182

    // Remove one item from the end of buffer
    model.removeItems(29, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // New item is fetched to replace the removed one
    QVERIFY(requestedIndexes.front() == model.index(29, 0));
    requestedIndexes.clear();
    // Last item is now 181

    // Remove many items from the end of buffer
    model.removeItems(20, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 10); // New items are fetched to replace the removed ones
    QVERIFY(requestedIndexes.front() == model.index(20, 0));
    QVERIFY(requestedIndexes.back() == model.index(29, 0));
    requestedIndexes.clear();
    // Last item is now 171

    // Remove items from outside of buffer (after buffer)
    model.removeItems(50, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // Buffer is not updated
    requestedIndexes.clear();
    // Last item is now 161

    // Remove items from the buffer limit (end of buffer)
    model.removeItems(25, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 5); // The new items inside buffer are requested
    QVERIFY(requestedIndexes.front() == model.index(25, 0));
    QVERIFY(requestedIndexes.back() == model.index(29, 0));
    // Last item is now 151

    // Move buffer to the middle of items
    mWidget->setCurrentIndex(model.index(75, 0));
    mWidget->scrollTo(model.index(75, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Remove items from the buffer limit (beginning of buffer)
    model.removeItems(59, 2);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 1); // New item is fetched to replace the one removed from the buffer
    QVERIFY(requestedIndexes.front() == model.index(88, 0)); // Buffer is moved forward, this is the last item
    requestedIndexes.clear();
    // Last item is now 149

    // Remove items over the whole buffer
    model.removeItems(55, 50);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // Whole buffer is updated
*/
    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;    
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_moveItemsCoverFlow()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    // TODO: How to verify that items are freed?

//    mWindow = new HbMainWindow;
    mWindow->viewport()->grabGesture(Qt::PanGesture);
    mWindow->viewport()->grabGesture(Qt::TapGesture); // Add TapAndHoldGesture once it's working
    mWidget = new HgMediawall();

    QList<QModelIndex> requestedIndexes;
    TestModel model(&requestedIndexes);
    model.generateItems(120);
    mWidget->setModel(&model);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWindow->show();

    QTest::qWait(2000);

    QVERIFY(requestedIndexes.count() == 25); // Scroll buffer size in coverflow mode is assumed to be 40
    QVERIFY(requestedIndexes.front() == model.index(0, 0));
    QVERIFY(requestedIndexes.back() == model.index(24, 0));
    requestedIndexes.clear();

    // Move one item forward
    model.moveItems(0, 20, 1);
    QTest::qWait(1000);
/*    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    model.moveItems(0, 2, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    model.moveItems(0, 29, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move many items forward
    model.moveItems(0, 20, 5);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move one item backward
    model.moveItems(29, 20, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    model.moveItems(29, 28, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    model.moveItems(29, 0, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move many items backward
    model.moveItems(20, 5, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // This should do nothing
    model.moveItems(20, 20, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move items from the border of the buffer forward
    model.moveItems(25, 50, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset
    requestedIndexes.clear();

    // Move items from the border of the buffer backward
    model.moveItems(25, 10, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset
    requestedIndexes.clear();

    // Move items from the buffer outside it
    model.moveItems(20, 90, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset
    requestedIndexes.clear();

    // Move items from outside the buffer inside it
    model.moveItems(90, 15, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset
    requestedIndexes.clear();

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move one item forward
    model.moveItems(90, 100, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    model.moveItems(90, 92, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    model.moveItems(90, 119, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move many items forward
    model.moveItems(90, 100, 5);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(120, 0));
    mWidget->scrollTo(model.index(120, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move one item backward
    model.moveItems(119, 100, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(120, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    model.moveItems(119, 118, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    model.moveItems(119, 90, 1);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move many items backward
    model.moveItems(110, 95, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 0); // New items are not fetched as the changes happened inside the buffer

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move items from the border of the buffer backward
    model.moveItems(85, 60, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move items from the border of the buffer forward
    model.moveItems(85, 100, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move items from the buffer outside it
    model.moveItems(100, 10, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move items from outside the buffer inside it
    model.moveItems(10, 100, 10);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(110, 0));
    mWidget->scrollTo(model.index(110, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move buffer to the middle of items
    mWidget->setCurrentIndex(model.index(60, 0));
    mWidget->scrollTo(model.index(60, 0));
    //QTest::qWait(1000);

    // Move buffer to the end of items
    mWidget->setCurrentIndex(model.index(60, 0));
    mWidget->scrollTo(model.index(60, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move items over the whole buffer forward
    model.moveItems(40, 110, 50);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset

    // Move buffer to the middle of items
    mWidget->setCurrentIndex(model.index(60, 0));
    mWidget->scrollTo(model.index(60, 0));
    //QTest::qWait(1000);
    requestedIndexes.clear();

    // Move items over the whole buffer backward
    model.moveItems(40, 10, 50);
    //QTest::qWait(1000);
    QVERIFY(requestedIndexes.count() == 30); // The whole buffer is reset
*/
    QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_labelFontSpecsCoverFlow()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mMediawall = new HgMediawall();
    mMediawall->enableReflections(true);
    mWidget = mMediawall;
    
    TestModel model;
    model.generateItems(50);
    mWindow->addView(mMediawall);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mMediawall->setModel(&model);
    mWindow->show();

    mMediawall->setTitleFontSpec(HbFontSpec(HbFontSpec::Primary));
    QVERIFY(mMediawall->titleFontSpec() == HbFontSpec(HbFontSpec::Primary));

    mMediawall->setDescriptionFontSpec(HbFontSpec(HbFontSpec::Secondary));
    QVERIFY(mMediawall->descriptionFontSpec() == HbFontSpec(HbFontSpec::Secondary));

    mMediawall->setTitleFontSpec(HbFontSpec(HbFontSpec::Title));
    QVERIFY(mMediawall->titleFontSpec() == HbFontSpec(HbFontSpec::Title));

    mMediawall->setDescriptionFontSpec(HbFontSpec(HbFontSpec::PrimarySmall));
    QVERIFY(mMediawall->descriptionFontSpec() == HbFontSpec(HbFontSpec::PrimarySmall));

    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;
}

void TestGanesWidgets::test_resetModelCoverFlow()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mMediawall = new HgMediawall();
    mWidget = mMediawall;
    
    TestModel model;
    model.generateItems(50);
    mWindow->addView(mMediawall);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mMediawall->setModel(&model);
    mWindow->show();

    // Reset with same item count
    model.reset(50);
    //QTest::qWait(2000);

    // Reset with smaller item count
    model.reset(20);    
    //QTest::qWait(2000);

    // Reset with larger item count
    model.reset(100);    
    //QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;

}

void TestGanesWidgets::test_resetModelGrid()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWidget = new HgGrid(Qt::Vertical);

    TestModel model;
    model.generateItems(50);
    mWindow->addView(mWidget);
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel(&model);
    mWindow->show();

    //QTest::qWait(2000);

    // Reset with same item count
    model.reset(50);
    //QTest::qWait(2000);

    // Reset with smaller item count
    model.reset(20);    
    //QTest::qWait(2000);

    // Reset with larger item count
    model.reset(100);    
    //QTest::qWait(2000);
  
    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
    
//    delete mWindow;
//    mWindow = 0;

}

void TestGanesWidgets::test_orientationChanged()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    mWidget = new HgGrid(Qt::Horizontal);
    TestModel model;
    model.generateItems(200);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
    mWindow->show();

    QTest::qWait( 1000 );

    mWidget->orientationChanged(Qt::Vertical);
    QTest::qWait(2000);
    mWidget->orientationChanged(Qt::Vertical);
    QTest::qWait(2000);

    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
}

void TestGanesWidgets::test_indexFeedback()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
    mMediawall = new HgMediawall();
    mWidget = mMediawall;
    TestModel model;
    model.generateItems(30);
    mWindow->addView( mWidget );
    QVERIFY( mWidget->model() == 0 );
    mWidget->setModel( &model );
    QVERIFY( &model == mWidget->model() );
    mWindow->show();

    QTest::qWait( 2000 );

    mMediawall->scrollBar()->setInteractive(true);
    QRectF rect = mWidget->scrollBar()->rect();
    QPointF move(0,300);
    
    QVERIFY(mMediawall->indexFeedbackPolicy() == HgWidget::IndexFeedbackNone);
    
    mMediawall->setIndexFeedbackPolicy(HgWidget::IndexFeedbackSingleCharacter);
    QVERIFY(mMediawall->indexFeedbackPolicy() == HgWidget::IndexFeedbackSingleCharacter);
    HbAutoTest::mousePress( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topLeft(), -1 );
    HbAutoTest::mouseMove( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight()+move, 1000 );
    HbAutoTest::mouseRelease( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight()+move, 1000 );

    mMediawall->setIndexFeedbackPolicy(HgWidget::IndexFeedbackThreeCharacter);
    QVERIFY(mMediawall->indexFeedbackPolicy() == HgWidget::IndexFeedbackThreeCharacter);
    HbAutoTest::mousePress( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topLeft(), -1 );
    mMediawall->scrollTo(model.index(10,0));
    HbAutoTest::mouseRelease( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topLeft(), 3000 );

    mMediawall->setIndexFeedbackPolicy(HgWidget::IndexFeedbackString);
    QVERIFY(mMediawall->indexFeedbackPolicy() == HgWidget::IndexFeedbackString);
    HbAutoTest::mousePress( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topLeft(), -1 );
//    HbAutoTest::mouseMove( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight()+move, 1000 );
    HbAutoTest::mouseRelease( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight(), 3000 );

    mMediawall->setIndexFeedbackPolicy(HgWidget::IndexFeedbackNone);
    QVERIFY(mMediawall->indexFeedbackPolicy() == HgWidget::IndexFeedbackNone);
    HbAutoTest::mousePress( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topLeft(), -1 );
    HbAutoTest::mouseMove( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight()+move, 1000 );
    HbAutoTest::mouseRelease( (HbAutoTestMainWindow*)mWindow, mWidget->scrollBar(), rect.topRight()+move, 1000 );    
    
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
}

void TestGanesWidgets::test_gridApi()
{
    if(mWidget) {
        mWindow->removeView(mWidget);
        delete mWidget;
        mWidget = 0;
    }
//    mWindow = new HbMainWindow;
    HgGrid *grid = new HgGrid(Qt::Horizontal);
    mWidget = grid;
    TestModel model;
    model.generateItems(200);
    mWindow->addView( mWidget );
    mWidget->setGeometry(QRectF(0,0,360,640));
    mWidget->setModel( &model );
    mWindow->show();

    QTest::qWait( 1000 );

    QList<QModelIndex> list = mWidget->getVisibleItemIndices();
    foreach(QModelIndex index, list) {
        QPolygonF pol;
        mWidget->getItemOutline(index,pol);
    }
    
    QVERIFY(!grid->pinchEnabled());
    grid->setPinchEnabled(true);
    QVERIFY(grid->pinchEnabled());
    grid->setPinchEnabled(false);
    QVERIFY(!grid->pinchEnabled());
    
    QVERIFY(grid->effect3dEnabled());
    grid->setEffect3dEnabled(true);
    QVERIFY(grid->effect3dEnabled());
    grid->setEffect3dEnabled(false);
    QVERIFY(!grid->effect3dEnabled());
        
    mWindow->removeView(mWidget);
    delete mWidget;
    mWidget = 0;
}


#ifdef _UNITTEST_GANESWIDGETS_LOG_TO_C_
    int main (int argc, char* argv[])
    {
        HbApplication app(argc, argv);
        TestGanesWidgets tc;
//        int c = 3;
//        char* v[] = {argv[0], "-o", "c:/test.txt"};
        return QTest::qExec(&tc, argc, argv);
//        return QTest::qExec(&tc, c, v);
    }
#else
    QTEST_APPLESS_MAIN(TestGanesWidgets)
#endif

#include "unittest_ganeswidgets.moc"

// EOF
