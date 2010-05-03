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
#include "HgMediaWallRenderer.h"
#include "hgmediawalldataprovider.h"
#include "hgquadrenderer.h"
#include "hgquad.h"
#include "hgimage.h"
#include "HgImageFader.h"
#include "hgvgquadrenderer.h"
#include <qvector3d>
#include <qtimer>
#include <qpropertyanimation>
#include <qstate.h>
#include <qabstracttransition>
#include <qstatemachine>
#include <qsignaltransition>
#include <qsequentialanimationgroup>
#include <qparallelanimationgroup>
#include <qvariantanimation>
#include <qpolygon>
#include <qpainter>
#include <qpaintengine>

const qreal KPi = 3.1415926535897932384626433832795;

static qreal lerp(qreal start, qreal end, qreal t)
{
    return start * (1.0f - t) + end * t;
}

class MyVectorAnimation : public QVariantAnimation
{
public:
    virtual void updateCurrentValue(const QVariant& value)
    {
        mValue = value.value<QVector3D>();
    }
    QVector3D getValue() const
    {
        return mValue;
    }
private:
    QVector3D mValue;
};

class HgAnimatedQuad
{
public:
    
    HgAnimatedQuad(HgQuad* start, HgQuad* end, 
        int duration) : mQuad(start)
    {        
        mPositionAnimation.setDuration(duration);
        mPositionAnimation.setKeyValueAt(0, start->position());
        mPositionAnimation.setKeyValueAt(1.0, end->position());
        mPositionAnimation.setEasingCurve(QEasingCurve::Linear);

        mScaleAnimation.setDuration(duration);
        mScaleAnimation.setKeyValueAt(0, QVector3D(start->scale().x(), start->scale().y(), 0));
        mScaleAnimation.setKeyValueAt(1, QVector3D(end->scale().x(), end->scale().y(), 0));
        mScaleAnimation.setEasingCurve(QEasingCurve::Linear);
    
    }
    
    ~HgAnimatedQuad()
    {
    }
    
    void start()
    {
        mPositionAnimation.start();
        mScaleAnimation.start();
    }
           
    void update()
    {
        mQuad->setPosition(mPositionAnimation.currentValue().value<QVector3D>());
        QVector3D scale = mScaleAnimation.currentValue().value<QVector3D>();
        mQuad->setScale(QVector2D(scale.x(), scale.y()));
    }
    
    HgQuad* mQuad;
    MyVectorAnimation mPositionAnimation;
    MyVectorAnimation mScaleAnimation;
};

HgMediaWallRenderer::HgMediaWallRenderer(HgMediaWallDataProvider* provider, 
    Qt::Orientation scrollDirection, bool coverflowMode) :
    mDataProvider(provider),
    mRenderer(NULL),
    mIndicatorRenderer(NULL),
    mRendererInitialized(false),
    mOrientation(scrollDirection),
    mNextOrientation(scrollDirection),
    mStateAnimationAlpha(0),
    mStateAnimationOnGoing(false),
    mAnimationAlpha(0),
    mOpeningAnimationDuration(500),
    mOpenedItem(-1),
    mFlipAngle(qreal(360)),
    mZoomAmount(qreal(0.5)),
    mCoverflowMode(coverflowMode),
    mRowCount(1),
    mNextRowCount(1),
    mStateAnimationDuration(300),
    mStep(1.1),
    mZfar(-2),
    mSpacing2D(10,10),
    mImageSize2D(100, 60),
    mCameraDistance(0),
    mCameraRotationY(0),
    mCameraRotationZ(0),
    mFrontCoverElevation(0.4),
    mReflectionsEnabled(true),
    mItemCountChanged(false),
    mOpenedItemState(ItemClosed),
    mFrontItemPosition(0,0),
    mFrontItemPositionSet(false)    
{
    createStateMachine();
    mImageFader = new HgImageFader();    
    mRenderer = new HgVgQuadRenderer(256);
    mRendererInitialized = true;
    if (mCoverflowMode) {
        mOrientation = Qt::Horizontal;
        mNextOrientation = mOrientation;
    }
}

HgMediaWallRenderer::~HgMediaWallRenderer()
{
    delete mRenderer;
    delete mImageFader;
    delete mStateMachine;
}


void HgMediaWallRenderer::setCameraDistance(qreal distance)
{
    mCameraDistance = distance;
}

void HgMediaWallRenderer::setCameraRotationY(qreal angle)
{
    mCameraRotationY = angle;
}

void HgMediaWallRenderer::setCameraRotationZ(qreal angle)
{
    mCameraRotationZ = angle;
}

qreal HgMediaWallRenderer::getCameraDistance() const
{
    return mCameraDistance;
}

qreal HgMediaWallRenderer::getCameraRotationY() const
{
    return mCameraRotationY;
}

qreal HgMediaWallRenderer::getCameraRotationZ() const
{
    return mCameraRotationZ;
}

void HgMediaWallRenderer::draw(
    const QPointF& startPosition,
    const QPointF& position, 
    const QPointF& targetPosition, 
    qreal springVelocity,
    QPainter* painter)
{
    // if still not initialized we cant draw anything
    if (!mRendererInitialized)
        return;
        
    if (mOrientation != mNextOrientation ||
        mRowCount != mNextRowCount)
    {
                
        // save old state of the quads         
        recordState(mOldState);
        
        // goto wanted orientation / rowcount
        mOrientation = mNextOrientation;
        mRowCount = mNextRowCount;
        setImageSize(mNextImageSize);
        
        // setup quads to new state
        setupRows(startPosition, position, targetPosition, springVelocity, painter);

        // record state for animation
        recordState(mNextState);

        startStateAnimation(painter);
    }
    else
    {
        if (!mStateAnimationOnGoing)
        {
            setupRows(startPosition, position, targetPosition, springVelocity, painter);
        }
        else
        {
            setupStateAnimation(painter);
        }    
    }
    
    updateCameraMatrices();
    drawQuads(painter);
}

void HgMediaWallRenderer::setupRows(const QPointF& startPosition,
    const QPointF& position, 
    const QPointF& targetPosition, 
    qreal springVelocity,
    QPainter* painter)
{
    // draw the state for it 
    resetQuads();
    updateSpacingAndImageSize();
    
    if (mCoverflowMode)
    {
        //setupRow(startPosition, position, targetPosition, springVelocity, painter, 0);
        setupCoverflow(startPosition, position, targetPosition, springVelocity, painter);
    }
    else
    {
        if (mOrientation == Qt::Vertical)
        {
            setupGridPortrait(startPosition, position, targetPosition, 
              springVelocity, painter);            
        }
        else
        {
            setupGridLandscape(startPosition, position, targetPosition, 
                springVelocity, painter);
        }
    }        
}

void HgMediaWallRenderer::setFlipAnimationAngle(qreal angleInDegrees)
{
    mFlipAngle = angleInDegrees;
}
    
void HgMediaWallRenderer::setOpeningAnimationType(HgMediaWallRenderer::OpeningAnimationType type)
{
    mOpeningAnimationType = type;
}

void HgMediaWallRenderer::setOpeningAnimationDuration(int msecs)
{
    mOpeningAnimationDuration = msecs;
}

qreal HgMediaWallRenderer::animationAlpha() const
{
    return mAnimationAlpha;
}

void HgMediaWallRenderer::setAnimationAlpha(qreal alpha)
{
    mAnimationAlpha = alpha;
    
    if (mOpenedItemState == ItemClosing && alpha == 0.0f)
        mOpenedItemState = ItemClosed;
    
    if (mOpenedItemState == ItemOpening && alpha == 1.0f)
        mOpenedItemState = ItemOpened;
    
    emit renderingNeeded();
}

qreal HgMediaWallRenderer::stateAnimationAlpha() const
{
    return mStateAnimationAlpha;
}

void HgMediaWallRenderer::setStateAnimationAlpha(qreal alpha)
{
    mStateAnimationAlpha = alpha;
    if (alpha == 1 && mStateAnimationOnGoing)
    {
        mStateAnimationOnGoing = false;
    }
    emit renderingNeeded();
}

void HgMediaWallRenderer::createStateMachine()
{
    mStateMachine = new QStateMachine(this);
    mStateMachine->setAnimated(true);
    
    QState* root = new QState(QState::ParallelStates);
    QState* p1 = new QState(root);
    QState* p2 = new QState(root);
    
    // create idle/opened states
    {            
        QState* idle = new QState(p1);
        QState* opened = new QState(p1);

        idle->assignProperty(this, "animationAlpha", qreal(0));                                
        opened->assignProperty(this, "animationAlpha", qreal(1));

        // add opening animation
        QPropertyAnimation* anim1 = new QPropertyAnimation(this, "animationAlpha");
        anim1->setDuration(mOpeningAnimationDuration);
        idle->addTransition(this, SIGNAL(toggleItem()), opened)->addAnimation(anim1);
            
        // add closing animation
        QPropertyAnimation* anim2 = new QPropertyAnimation(this, "animationAlpha");
        anim2->setDuration(mOpeningAnimationDuration);
        opened->addTransition(this, SIGNAL(toggleItem()), idle)->addAnimation(anim2);

        QObject::connect(idle, SIGNAL(entered()), this, SLOT(onIdleState()));
        QObject::connect(opened, SIGNAL(entered()), this, SLOT(onOpenedState()));
    
        p1->setInitialState(idle);
    }
    
    // create two states to animate between
    {
        QState* s1 = new QState(p2);
        QState* s2 = new QState(p2);

        s1->assignProperty(this, "stateAnimationAlpha", qreal(0));
        s2->assignProperty(this, "stateAnimationAlpha", qreal(0));
        
        QPropertyAnimation* anim = new QPropertyAnimation(this, "stateAnimationAlpha");
        anim->setStartValue(qreal(0));
        anim->setEndValue(qreal(1));
        anim->setDuration(mStateAnimationDuration);
        
        s1->addTransition(this, SIGNAL(toggleState()), s2)->addAnimation(anim);
        s2->addTransition(this, SIGNAL(toggleState()), s1)->addAnimation(anim);        

        p2->setInitialState(s1);        
    }

    root->setInitialState(p1);
    mStateMachine->addState(root);    
    mStateMachine->setInitialState(root);
    mStateMachine->start();

}

void HgMediaWallRenderer::onIdleState()
{
    emit itemClosed(mOpenedItem);
}

void HgMediaWallRenderer::onOpenedState()
{
    emit itemOpened(mOpenedItem);
}

void HgMediaWallRenderer::setOrientation(Qt::Orientation orientation, bool animate)
{
    // coverflow is always horizontal
    if (mCoverflowMode)
    {
        mOrientation = Qt::Horizontal;
        mNextOrientation = mOrientation;
        return;
    }
    
    if (mOrientation != orientation)
    {
        mStateMachine->setAnimated(animate);
        mNextOrientation = orientation;

        if (!animate)
            mOrientation = orientation;
        else
        {
            emit renderingNeeded();            
        }
    }
}

Qt::Orientation HgMediaWallRenderer::getOrientation() const
{
    return mOrientation;
}

void HgMediaWallRenderer::drawQuads(QPainter* painter)
{
    
    mRenderer->transformQuads(mViewMatrix, mProjMatrix, mRect);

    mRenderer->drawQuads(mRect, painter);    
}


void HgMediaWallRenderer::enableCoverflowMode(bool enabled)
{
    mCoverflowMode = enabled;
}

bool HgMediaWallRenderer::coverflowModeEnabled() const
{
    return mCoverflowMode;
}

void HgMediaWallRenderer::setRowCount(int rowCount, const QSizeF& newImageSize, bool animate)
{
    if (rowCount != mRowCount)
    {
        mStateMachine->setAnimated(animate);

        mNextRowCount = rowCount;
        mNextImageSize = newImageSize;

        mColumnCount = rowCount;
        
        if (!animate)
        {
            mRowCount = rowCount;
        }
        else
        {
            emit renderingNeeded();            
        }
         
    }

}

int HgMediaWallRenderer::getRowCount() const
{
    return mRowCount;
}

void HgMediaWallRenderer::recordState(HgMediaWallRenderer::State& state)
{
    // cleanup old quads
    for (int i = 0; i < state.mQuads.size(); i++)
    {
        delete state.mQuads[i];
    }
    
    state.mQuads.clear();
    
    // record new quads
    for (int i = 0; i < mRenderer->quadCount(); i++)
    {
        HgQuad* quad = mRenderer->quad(i);
        if (!quad->visible())
            continue;
        
        state.mQuads.append(quad->copy());
    }    
}

void HgMediaWallRenderer::setupStateAnimation(QPainter* painter)
{
    Q_UNUSED(painter)
    
    resetQuads();
    // setup quads from animated state
    for (int i = 0; i < mOldState.mQuads.size(); i++)
    {
        mAnimatedQuads[i]->update();
        mRenderer->quad(i)->copyFrom(*mOldState.mQuads[i]);
    }
}

void HgMediaWallRenderer::resetQuads()
{
    for (int i = 0; i < mRenderer->quadCount(); i++)
        mRenderer->quad(i)->setVisible(false);    
}

HgQuad* HgMediaWallRenderer::getQuadAt(const QPointF& position) const
{
    if (!mRendererInitialized)
        return NULL;
        
    return mRenderer->getQuadAt(position);//mapFromWindow(position));
}

bool HgMediaWallRenderer::isItemOpen() const
{
    return (mOpenedItem != -1 && mAnimationAlpha > 0);
}

void HgMediaWallRenderer::setRect(const QRectF& windowRect)
{
    mRect = windowRect;
}

const QRectF& HgMediaWallRenderer::getRect() const
{
    return mRect;
}

void HgMediaWallRenderer::updateCameraMatrices()
{    
    QMatrix4x4 view;
        
    view.setToIdentity();
    
    view.lookAt(QVector3D(0.0, 0.0, 1.0f  + mCameraDistance), 
        QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 rot;
    rot.rotate(mCameraRotationZ, QVector3D(0,0,1));
    rot.rotate(mCameraRotationY, QVector3D(0,1,0));
    view *= rot;
        
    qreal aspect = mRect.width() / mRect.height();
    
    QMatrix4x4 proj;
    proj.setToIdentity();
    
    if (mRect.width() <= mRect.height())
    {
        qreal aspect = mRect.height() / mRect.width();
        proj.frustum(-0.5f, 0.5f, -0.5f*aspect, 0.5f*aspect, 1.0f, 1000.0f);
    }
    else
    {
        qreal aspect = mRect.width() / mRect.height();
        proj.frustum(-0.5f*aspect, 0.5f*aspect, -0.5f, 0.5f, 1.0f, 1000.0f);
    }

    mViewMatrix = view;
    mProjMatrix = proj;

    qreal mirrorPlaneY;
    if (mCoverflowMode)
    {
        mirrorPlaneY = -mImageSize3D.height()/2;
    }
    else // grid
    {
        mirrorPlaneY = getRowPosY(mRowCount-1)-mImageSize3D.height()/2;
    }

    mRenderer->setMirroringPlaneY(mirrorPlaneY);
}


void HgMediaWallRenderer::updateSpacingAndImageSize()
{
    qreal div = mRect.width() <= mRect.height() ? mRect.width() : mRect.height();
    
    mSpacing3D = mSpacing2D / div;
    mImageSize3D = mImageSize2D / div;
}

void HgMediaWallRenderer::setSpacing(const QSizeF& spacing)
{
    mSpacing2D = spacing;
}

void HgMediaWallRenderer::setImageSize(const QSizeF& imageSize)
{
    mImageSize2D = imageSize;
    mNextImageSize = imageSize;
}

const QSizeF& HgMediaWallRenderer::getSpacing() const
{
    return mSpacing2D;
}

const QSizeF& HgMediaWallRenderer::getImageSize() const
{
    return mImageSize2D;
}

void HgMediaWallRenderer::setFrontCoverElevationFactor(qreal elevation)
{
    mFrontCoverElevation = elevation;
}

qreal HgMediaWallRenderer::getFrontCoverElevationFactor() const
{
    return mFrontCoverElevation;
}

void HgMediaWallRenderer::openItem(int index, bool animate)
{
    if (isItemOpen())
        return;
    
    mOpenedItem = index;
    mOpenedItemState = animate ? ItemOpening : ItemOpened;

    mStateMachine->setAnimated(animate);
    emit toggleItem();
    
}

void HgMediaWallRenderer::closeItem(bool animate)
{
    if (!isItemOpen())
        return;

    mOpenedItemState = animate ? ItemClosing : ItemClosed;
    if (!animate)
        mOpenedItem = -1;

    mStateMachine->setAnimated(animate);
    emit toggleItem();
}

qreal HgMediaWallRenderer::getRowPosY(int row)
{
    qreal step = mSpacing3D.height() + mImageSize3D.height();            
    return mRowCount == 1 ? qreal(0) : (((qreal)mRowCount/qreal(2)-qreal(0.5)) - (qreal)row) * step; 
}

qreal HgMediaWallRenderer::getColumnPosX(int col)
{
    qreal step = -(mSpacing3D.width() + mImageSize3D.width());                
    return mColumnCount == 1 ? qreal(0) : (((qreal)mColumnCount/qreal(2)-qreal(0.5)) - (qreal)col) * step; 
}


void HgMediaWallRenderer::enableReflections(bool enabled)
{
    mReflectionsEnabled = enabled;
}

bool HgMediaWallRenderer::reflectionsEnabled() const
{
    return mReflectionsEnabled;
}
    
QPointF HgMediaWallRenderer::mapFromWindow(const QPointF& point) const
{
    return QPointF(point.x(), mRect.height() - point.y());
}

void HgMediaWallRenderer::emitUpdate()
{
    emit renderingNeeded();
}

void HgMediaWallRenderer::applyOpeningAnimation(HgQuad* quad)
{
    QQuaternion rot(0,0,0,1);
    qreal rotAngle = mAnimationAlpha * mFlipAngle;
    rot = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), rotAngle);
    quad->setRotation(rot);
    quad->setPosition(quad->position() + QVector3D(0,0,mAnimationAlpha * mZoomAmount));    
}


qreal HgMediaWallRenderer::getWorldWidth() const
{   
    qreal width = ceil((qreal)mDataProvider->imageCount() / (qreal)mRowCount - 1.0f);
    
    // if we are in vertical orientation we want last and first item
    // to place at the top and bottom of the screen instead of center
    if (mOrientation == Qt::Vertical)
    {
        qreal step = mSpacing2D.height() + mImageSize2D.height(); 
        width -= (mRect.height() / step - 1.0f);
    }
       
    return width;
}

void HgMediaWallRenderer::beginRemoveRows(int start, int end)
{
    mRemoveStart = start;
    mRemoveEnd = end;
    mItemCountChanged = true;
    
    recordState(mOldState);

}

void HgMediaWallRenderer::endRemoveRows()
{
        
    mStateMachine->setAnimated(true);

    emit renderingNeeded();            
    
}

void HgMediaWallRenderer::startStateAnimation(QPainter* painter)
{
    
    // clear previous animation quads
    for (int i = 0; i < mAnimatedQuads.size(); i++)
    {
        delete mAnimatedQuads[i];
    }        
    mAnimatedQuads.clear();
    
    // setup animated quads
    HgQuad* defaultQuad = new HgQuad();
    defaultQuad->setPosition(QVector3D(100,100,-100));
    int n = mOldState.mQuads.count() < mNextState.mQuads.count() ? mNextState.mQuads.count() : mOldState.mQuads.count();
    for (int i = 0; i < n; i++)
    {
        HgQuad* qA = (i >= mOldState.mQuads.count()) ? defaultQuad : mOldState.mQuads[i];
        HgQuad* qB = (i >= mNextState.mQuads.count()) ? defaultQuad : mNextState.mQuads[i];
        
        HgAnimatedQuad* q = new HgAnimatedQuad(qA, qB, mStateAnimationDuration);
        mAnimatedQuads.append(q);
        q->start();
    }
    
    mStateAnimationOnGoing = true;
    
    // setup first frame of the animation
    setupStateAnimation(painter);        

    // toggle state animation on
    toggleState();

}

void HgMediaWallRenderer::setupCoverflow(const QPointF& startPosition,
    const QPointF& position, 
    const QPointF& targetPosition, 
    qreal springVelocity,
    QPainter* painter)
{   
    Q_UNUSED(startPosition)
    Q_UNUSED(targetPosition)
    Q_UNUSED(springVelocity)
    Q_UNUSED(painter)
    
    // save selected item for coverflow
    mSelectedItem = ceil(position.x());
    
    int quadsVisible = (mRect.width() / mImageSize2D.width() + 1) * 4;
    int selectedItemIndex = quadsVisible / 2;

    qreal step = mSpacing3D.width() + mImageSize3D.width();                
    qreal ipos = floorf(position.x());
    qreal frac = (position.x() - ipos) * step;
    qreal posX = -(qreal)(selectedItemIndex + 0) * step - frac;
    qreal zFar = -mFrontCoverElevation;
    qreal posY = 0;

    int count = mDataProvider->imageCount();
    int quadIndex = 0;
    int itemIndex = ((int)(ipos - (qreal)selectedItemIndex));
    int index = 0;
    
    while (1)
    {
        if (itemIndex < 0)
        {
            itemIndex++;
            posX += step;
            index++;
            continue;
        }
        else if (itemIndex >= count || index >= quadsVisible || quadIndex >= mRenderer->quadCount())
        {
            break;
        }
                        
        qreal posZ = zFar;

        // if this is center item modify its z
        qreal p = posX / step;
        if (p > -1.0f && p < 1.0f)
        {
            qreal d = lerp(-zFar, 0, qBound(qreal(0), qAbs(springVelocity)/6.0f, qreal(1)));
            posZ = zFar + sin((p+1.0f) * KPi / 2) * d;                
        }

        // modify z also for sorting
        posZ -= 0.001f * abs(posX/step);
                
        // setup quad for this item
        HgQuad* quad = mRenderer->quad(quadIndex);
        setupDefaultQuad(QVector3D(posX, posY, posZ), itemIndex, mReflectionsEnabled, quadIndex);
                         
        // step to next item                    
        posX += step;        
        itemIndex++;
        index++;
    }
    
}


void HgMediaWallRenderer::setupGridPortrait(const QPointF& startPosition,
    const QPointF& position, 
    const QPointF& targetPosition, 
    qreal springVelocity,
    QPainter* painter)
{
    Q_UNUSED(startPosition)
    Q_UNUSED(targetPosition)
    Q_UNUSED(springVelocity)
    Q_UNUSED(painter)
    
    int rowCount = (mRect.height() / mImageSize2D.height() + 1) * 4;
    int rowsUp = rowCount/2;
        
    qreal stepY = mSpacing3D.height() + mImageSize3D.height();
    qreal ipos = floorf(position.x());
    qreal frac = (position.x() - ipos) * stepY;
    qreal posY = -(qreal)rowsUp * stepY - frac;
        
    // adjust height so that we begin from top
    qreal div = mRect.width() <= mRect.height() ? mRect.width() : mRect.height();
    posY -= mRect.height() / div / 2.0 - stepY / 2.0;
    
    int count = mDataProvider->imageCount();
    int itemIndex = ((int)(ipos - (qreal)rowsUp)) * mColumnCount;
    int row = 0;
    int quadIndex = 0;
    
    while (1)
    {
        if (itemIndex < 0)
        {
            itemIndex+=mColumnCount;
            posY += stepY;
            row++;
            continue;
        }
        else if (itemIndex >= count || quadIndex >= mRenderer->quadCount() || row >= rowCount)
        {
            break;
        }
        
        setupGridRow(-posY, itemIndex, quadIndex);
                        
        posY += stepY;
        row++;
        itemIndex+=mColumnCount;
    }
    
}

void HgMediaWallRenderer::setupGridLandscape(const QPointF& startPosition,
    const QPointF& position, 
    const QPointF& targetPosition, 
    qreal springVelocity,
    QPainter* painter)
{
    Q_UNUSED(startPosition)
    Q_UNUSED(targetPosition)
    Q_UNUSED(springVelocity)
    Q_UNUSED(painter)
    
    int colCount = (mRect.width() / mImageSize2D.width() + 1) * 3;
    int colsLeft = colCount/2;

    qreal stepX = mSpacing3D.width() + mImageSize3D.width();
    qreal ipos = floorf(position.x());
    qreal frac = (position.x() - ipos) * stepX;
    qreal posX = -(qreal)colsLeft * stepX - frac;    
    
    int count = mDataProvider->imageCount();
    int itemIndex = ((int)(ipos - (qreal)colsLeft)) * mRowCount;
    int col = 0;
    int quadIndex = 0;
    
    while (1)
    {
        if (itemIndex < 0)
        {
            itemIndex+=mColumnCount;
            posX += stepX;
            col++;
            continue;
        }
        else if (itemIndex >= count || col >= colCount || quadIndex >= mRenderer->quadCount())
        {
            break;
        }
        
        setupGridColumn(posX, itemIndex, quadIndex);
                        
        posX += stepX;
        col++;
        itemIndex+=mRowCount;
    }
}

void HgMediaWallRenderer::setupGridColumn(qreal posX, int itemIndex, int& quadIndex)
{
    for (int i = 0; i < mRowCount; i++)
    {
        qreal posY = getRowPosY(i);
        
        // enable reflections for the last row needed
        bool reflections = (i == (mRowCount-1) && mReflectionsEnabled);

        setupDefaultQuad(QVector3D(posX, posY, 0), itemIndex++, reflections, quadIndex);
        
        if (itemIndex >= mDataProvider->imageCount())
            return;    
    }    
}

void HgMediaWallRenderer::setupGridRow(qreal posY, int itemIndex, int& quadIndex)
{
    for (int i = 0; i < mColumnCount; i++)
    {
        qreal posX = getColumnPosX(i);
        setupDefaultQuad(QVector3D(posX, posY, 0), itemIndex++, false, quadIndex);
        if (itemIndex >= mDataProvider->imageCount())
            return;     
    }    
}

void HgMediaWallRenderer::setupDefaultQuad(const QVector3D& pos, int itemIndex, bool reflectionsEnabled, int& quadIndex)
{
    HgQuad* quad = mRenderer->quad(quadIndex++);
    quad->setPosition(pos);
    quad->setImage(mDataProvider->image(itemIndex));
    quad->setVisible(true);
    quad->setScale(QVector2D(mImageSize3D.width(),mImageSize3D.height()));
    quad->setPivot(QVector2D(0,0));
    quad->setUserData(QVariant(itemIndex));
    quad->setRotation(QQuaternion(1,0,0,0));
    quad->setOuterRotation(QQuaternion(1,0,0,0));
    quad->enableMirrorImage(reflectionsEnabled);
    quad->setAlpha(1.0f);
    
    // apply opening animation if needed
 /*   if (itemIndex == mOpenedItem)
        applyOpeningAnimation(quad);
*/
    // setup indicator/decorator for the item if needed 
    int flags = mDataProvider->flags(itemIndex);
    const HgImage* indicatorImage = mDataProvider->indicator(flags);
    if (flags != 0 && indicatorImage)
    {
        HgQuad* indicator = mRenderer->quad(quadIndex++);
        setupIndicator(quad, indicator, indicatorImage, 
            itemIndex);
        indicator->enableMirrorImage(reflectionsEnabled);
    }


}

void HgMediaWallRenderer::setupIndicator(HgQuad* parent, 
    HgQuad* indicator, const HgImage* indicatorImage, int itemIndex)
{
    indicator->setPosition(parent->position()+
        QVector3D(0.25*mImageSize3D.width(), -0.25*mImageSize3D.height(), 0.0001f));
    indicator->setImage(indicatorImage);
    indicator->setVisible(true);
    indicator->setScale(QVector2D(0.25f*mImageSize3D.width(), 0.25f*mImageSize3D.height()));
    indicator->setPivot(QVector2D(0.0, 0.0));
    indicator->setUserData(QVariant(itemIndex));
    indicator->setRotation(parent->rotation());
    indicator->setOuterRotation(parent->outerRotation());
    indicator->enableMirrorImage(false);
    indicator->setAlpha(parent->alpha());

    // apply opening animation to indicator if needed
    if (itemIndex == mOpenedItem)
        applyOpeningAnimation(indicator);
}

HgQuadRenderer* HgMediaWallRenderer::getRenderer()
{
    return mRenderer;
}

bool HgMediaWallRenderer::getItemPoints(int index, QPolygonF& points) const
{
    QPolygonF poly;
    if (!mRenderer->getQuadTranformedPoints(poly, index))
        return false;
    
    points = poly;
    return true;
}

QList<HgQuad*> HgMediaWallRenderer::getVisibleQuads() const
{
    return mRenderer->getVisibleQuads(QRectF(0, 0, mRect.width(), mRect.height()));
}

void HgMediaWallRenderer::setFrontItemPosition(const QPointF& position)
{
    mFrontItemPosition = position;
    
    mRenderer->setTranslation(
        QVector2D(position.x(), position.y()));    
}

QPointF HgMediaWallRenderer::frontItemPosition() const
{
    return mFrontItemPosition;
}
