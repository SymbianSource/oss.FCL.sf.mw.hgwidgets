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
#include "hgqtquadrenderer.h"
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

class MyQuaternionAnimation : public QVariantAnimation
{
public:
    virtual void updateCurrentValue(const QVariant& value)
    {
        mValue = value.value<QQuaternion>();
    }
    QQuaternion getValue() const
    {
        return mValue;
    }
private:
    QQuaternion mValue;
};


class HgAnimatedQuad
{
public:

    static HgAnimatedQuad* createScrollDirectionChangeAnimation(
        HgQuad* a, HgQuad* b, const QMatrix4x4& tm, const QMatrix4x4& rm, 
        const QQuaternion& rot, Qt::Orientation orientation, 
        int duration)
    {
        HgAnimatedQuad* q = new HgAnimatedQuad();
        q->mQuad = a;

        q->mPosition.setEasingCurve(QEasingCurve::InOutCubic);
        q->mPosition.setDuration(duration);
        QVector3D pos = tm * (a->position() * rm);
        q->mPosition.setKeyValueAt(0, pos);
        
        QVector3D pos2;
        
        if (orientation == Qt::Horizontal)
        {
            pos2 = QVector3D(pos.x(), 0, pos.z() + (pos.y() > b->position().y() ? -0.5f : -0.5f));
        }
        else
        {
            pos2 = QVector3D(0, pos.y(), pos.z() + (pos.x() > b->position().x() ? -0.5f : -0.5f));            
        }
                
        q->mPosition.setKeyValueAt(0.5f, pos2);
        q->mPosition.setKeyValueAt(1, b->position());
        
        q->mRotation.setEasingCurve(QEasingCurve::InOutCubic);
        q->mRotation.setDuration(duration);
        q->mRotation.setKeyValueAt(0, rot);
        q->mRotation.setKeyValueAt(0.5f, QQuaternion::fromAxisAndAngle(QVector3D(1,1,0), 180));
        q->mRotation.setKeyValueAt(1, b->rotation());
    
        return q;
    }
    
    static HgAnimatedQuad* createBasicAnimation(HgQuad* a, HgQuad* b, int duration)
    {
        HgAnimatedQuad* q = new HgAnimatedQuad();
        q->mQuad = a;

        q->mPosition.setDuration(duration);
        q->mPosition.setKeyValueAt(0, a->position());
        q->mPosition.setKeyValueAt(1, b->position());
        
        q->mRotation.setDuration(duration);
        q->mRotation.setKeyValueAt(0, a->rotation());
        q->mRotation.setKeyValueAt(1, b->rotation());        
    
        return q;
    }
            
    void start()
    {
        mPosition.start();
        mRotation.start();
    }
           
    void update()
    {
        mQuad->setPosition(mPosition.currentValue().value<QVector3D>());
        mQuad->setRotation(mRotation.currentValue().value<QQuaternion>());
    }
    
    const HgQuad* quad() const
    {
        return mQuad;
    }

private:
    HgQuad* mQuad;
    MyVectorAnimation mPosition;
    //MyVectorAnimation mScale;
    MyQuaternionAnimation mRotation;
    
};

class HgAnimatedQuadFactory
{
public:
    virtual HgAnimatedQuad* createQuad(HgQuad* qA, HgQuad* qB) const=0;
};

class HgScrollDirChangeQuadFactory : public HgAnimatedQuadFactory
{
public:
    void setup(Qt::Orientation nextScrollDirection, 
        const QRectF& rect, const QSizeF& spacing, const QSizeF& size, 
        int duration)
    {
        mNextScrollDirection = nextScrollDirection;
        mDuration = duration;
        
        if (mNextScrollDirection == Qt::Horizontal)
        {
            qreal stepY = spacing.height() + size.height();
            qreal posY = 0.5f - (rect.height() / rect.width() / 2.0 - stepY / 2.0);                
            tm.translate(-posY,0);
            rm.rotate(-90, QVector3D(0,0,1));
            rot = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), -90);
        }
        else if (mNextScrollDirection == Qt::Vertical)
        {
            tm.translate(0,0.5f);
            rm.rotate(90, QVector3D(0,0,1));
            rot = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), -90);                
        }
        
        
    }
    
    HgAnimatedQuad* createQuad(HgQuad* qA, HgQuad* qB) const
    {
        return HgAnimatedQuad::createScrollDirectionChangeAnimation(qA, qB, 
            tm, rm, rot, mNextScrollDirection, mDuration);        
    }
private:
    QMatrix4x4 tm;
    QMatrix4x4 rm;
    QQuaternion rot;
    Qt::Orientation mNextScrollDirection;
    int mDuration;
};

class HgRowCountChangeQuadFactory : public HgAnimatedQuadFactory
{
public:
    HgRowCountChangeQuadFactory(int duration) : mDuration(duration)
    {
        
    }
    
    HgAnimatedQuad* createQuad(HgQuad* qA, HgQuad* qB) const
    {
        return HgAnimatedQuad::createBasicAnimation(qA, qB, mDuration);
    }
private:
    int mDuration;
};


HgMediaWallRenderer::HgMediaWallRenderer(HgMediaWallDataProvider* provider, 
    Qt::Orientation orientation, Qt::Orientation scrollDirection, bool coverflowMode) :
    mDataProvider(provider),
    mRenderer(NULL),
    mIndicatorRenderer(NULL),
    mRendererInitialized(false),
    mScrollDirection(scrollDirection),
    mNextScrollDirection(scrollDirection),
    mOrientation(orientation),
    mStateAnimationAlpha(0),
    mStateAnimationOnGoing(false),
    mAnimationAlpha(0),
    mCoverflowMode(coverflowMode),
    mRowCount(1),
    mNextRowCount(1),
    mStateAnimationDuration(500),
    mStep(1.1),
    mZfar(-2),
    mSpacing2D(10,10),
    mImageSize2D(100, 60),
    mCameraDistance(0),
    mCameraRotationY(0),
    mCameraRotationZ(0),
    mFrontCoverElevation(0.4),
    mFrontItemPosition(0,0)
{
    createStateMachine();
    mRenderer = new HgQtQuadRenderer(64);
    mRenderer->enableReflections(true);
    mRendererInitialized = true;
    if (mCoverflowMode) {
        mScrollDirection = Qt::Horizontal;
        mNextScrollDirection = mScrollDirection;
    }
}

HgMediaWallRenderer::~HgMediaWallRenderer()
{
    delete mRenderer;
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
    QPainter* painter, 
    const QTransform& sceneTransform,
    const QRectF& rect)
{
    // save new rect
    mRect = rect;
    
    // if still not initialized we cant draw anything
    if (!mRendererInitialized)
        return;
        
    if (mScrollDirection != mNextScrollDirection)
    {
        startScrollDirectionChangeAnimation(startPosition, position, 
            targetPosition, springVelocity, painter, sceneTransform,
            rect);
    }
    else if (mRowCount != mNextRowCount)
    {
        startRowCountChangeAnimation(startPosition, position, 
            targetPosition, springVelocity, painter, sceneTransform,
            rect);
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
    drawQuads(painter, sceneTransform);
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
        if (mScrollDirection == Qt::Vertical)
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

qreal HgMediaWallRenderer::animationAlpha() const
{
    return mAnimationAlpha;
}

void HgMediaWallRenderer::setAnimationAlpha(qreal alpha)
{
    mAnimationAlpha = alpha;
        
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
    QState* p2 = new QState(root);
        
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

    root->setInitialState(p2);
    mStateMachine->addState(root);    
    mStateMachine->setInitialState(root);
    mStateMachine->start();

}

void HgMediaWallRenderer::setScrollDirection(Qt::Orientation scrollDirection, bool animate)
{
    // coverflow is always horizontal
    if (mCoverflowMode)
    {
        mScrollDirection = Qt::Horizontal;
        mNextScrollDirection = mScrollDirection;
        return;
    }
    
    if (mScrollDirection != scrollDirection)
    {
        mStateMachine->setAnimated(animate);
        mNextScrollDirection = scrollDirection;

        if (!animate)
            mScrollDirection = scrollDirection;
        else
        {
            //emit renderingNeeded();            
        }
    }
}

Qt::Orientation HgMediaWallRenderer::getScrollDirection() const
{
    return mScrollDirection;
}

void HgMediaWallRenderer::drawQuads(QPainter* painter, 
    const QTransform& sceneTransform)
{
    mRenderer->drawQuads(painter, mRect, mViewMatrix, mProjMatrix, mOrientation, 
        sceneTransform);    
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
    qDeleteAll(state.mQuads.begin(), state.mQuads.end());    
    state.mQuads.clear();
    
    // record new quads
    for (int i = 0; i < mRenderer->quadCount(); i++)
    {
        HgQuad* quad = mRenderer->quad(i);
        if (!quad->visible())
            continue;
        
        int index = quad->userData().toInt();
        state.mQuads[index] = quad->copy();
    }    
}

void HgMediaWallRenderer::setupStateAnimation(QPainter* painter)
{
    Q_UNUSED(painter)
    
    resetQuads();
    updateSpacingAndImageSize();

    // setup quads from animated state
    for (int i = 0; i < mAnimatedQuads.count(); i++)
    {
        if (i >= mRenderer->quadCount())
            return;
        
        mAnimatedQuads[i]->update();        
        mRenderer->quad(i)->copyFrom(*mAnimatedQuads[i]->quad());
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
        
    return mRenderer->getQuadAt(position);
}

void HgMediaWallRenderer::updateCameraMatrices()
{    
    QMatrix4x4 view;
        
    view.setToIdentity();
    
    view.lookAt(QVector3D(0.0, 0.0, 1.0f  + mCameraDistance), 
        QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    QMatrix4x4 rot;
    //rot.rotate(mCameraRotationZ, QVector3D(0,0,1));
    rot.rotate(mCameraRotationY, QVector3D(0,1,0));
    view *= rot;
            
    QMatrix4x4 proj;
    proj.setToIdentity();
    
    // setup projection matrix so that width of the item wichi has the 
    // width of the screen has width of 1 in 3D space
    qreal aspect = mRect.height() / mRect.width();
    proj.frustum(-0.5f, 0.5f, -0.5f * aspect, 0.5f * aspect, 1.0f, 1000.0f);
        
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
    qreal div = mRect.width();
    
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
    mRenderer->enableReflections(enabled);
}

bool HgMediaWallRenderer::reflectionsEnabled() const
{
    return mRenderer->reflectionsEnabled();
}
    
qreal HgMediaWallRenderer::getWorldWidth() const
{   
    qreal width = ceil((qreal)mDataProvider->imageCount() / (qreal)mRowCount - 1.0f);
    
    // if we are in vertical orientation we want last and first item
    // to place at the top and bottom of the screen instead of center
    if (mScrollDirection == Qt::Vertical)
    {
        qreal step = mSpacing2D.height() + mImageSize2D.height(); 
        width -= (mRect.height() / step - 1.0f);
    }
    else if (mScrollDirection == Qt::Horizontal && !mCoverflowMode)
    {
        qreal step = mSpacing2D.width() + mImageSize2D.width();
        width -= (mRect.width() / step - 1.0f);
    }
       
    return width;
}


void HgMediaWallRenderer::createAnimatedQuads(const HgAnimatedQuadFactory& factory)
{
    // clear previous animation quads
    qDeleteAll(mAnimatedQuads.begin(), mAnimatedQuads.end());
    mAnimatedQuads.clear();
    
    // default quad is used if no counterpart for the current quad exits.
    HgQuad* defaultQuad = new HgQuad();
    defaultQuad->setPosition(QVector3D(100,100,-100));
    
    // setup new animated quads
    QMap<int, HgQuad*>::iterator i = mNextState.mQuads.begin();
    while(i != mNextState.mQuads.end())
    {
        HgQuad* qB = i.value();
        HgQuad* qA = NULL;
        QMap<int, HgQuad*>::iterator j = mOldState.mQuads.find(i.key());
        if (j != mOldState.mQuads.end())
        {
            qA = j.value();
        }
        else
        {
            qA = defaultQuad->copy();
        }
        
        HgAnimatedQuad* q = factory.createQuad(qA, qB);
        mAnimatedQuads.append(q);
        
        q->start();
        i++;
    }    
}

void HgMediaWallRenderer::startScrollDirectionChangeAnimation(
    const QPointF& startPosition,
    const QPointF& position, 
    const QPointF& targetPosition, 
    qreal springVelocity,
    QPainter* painter, 
    const QTransform& sceneTransform,
    const QRectF& rect)
{

    // save state for current orientation
    setupRows(startPosition, position, targetPosition, springVelocity, painter);
    recordState(mOldState);
    
    // goto wanted orientation
    mScrollDirection = mNextScrollDirection;
    
    // setup quads to new state
    setupRows(startPosition, position, targetPosition, springVelocity, painter);

    // record state for animation
    recordState(mNextState);
    
    HgScrollDirChangeQuadFactory factory;
    factory.setup(mNextScrollDirection, mRect, mSpacing3D, mImageSize3D, mStateAnimationDuration);

    createAnimatedQuads(factory);
        
    mStateAnimationOnGoing = true;
    
    // setup first frame of the animation
    setupStateAnimation(painter);        

    // toggle state animation on
    toggleState();
    
}

void HgMediaWallRenderer::startRowCountChangeAnimation(
    const QPointF& startPosition,
    const QPointF& position, 
    const QPointF& targetPosition, 
    qreal springVelocity,
    QPainter* painter, 
    const QTransform& sceneTransform,
    const QRectF& rect)
{
    setupRows(startPosition, position, targetPosition, springVelocity, painter);
    recordState(mOldState);
    
    mRowCount = mNextRowCount;
    setImageSize(mNextImageSize);
    
    setupRows(startPosition, position, targetPosition, springVelocity, painter);
    recordState(mNextState);

    HgRowCountChangeQuadFactory factory(mStateAnimationDuration);    

    createAnimatedQuads(factory);
        
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
        setupDefaultQuad(QVector3D(posX, posY, posZ), itemIndex, reflectionsEnabled(), quadIndex);
                         
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
    
    // we need to setup 2 times more rows than visible, because we need
    // more quads for the orientation switch
    int rowCount = (mRect.height() / mImageSize2D.height() + 1) * 3;       
    int rowsUp = rowCount / 3;
        
    qreal stepY = mSpacing3D.height() + mImageSize3D.height();
    qreal ipos = floorf(position.x());
    qreal frac = (position.x() - ipos) * stepY;
    qreal posY = -(qreal)rowsUp * stepY - frac;
        
    // adjust height so that we begin from top
    posY -= mRect.height() / mRect.width() / 2.0 - stepY / 2.0;
    
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
            continue;
        }
        else if (itemIndex >= count || quadIndex >= mRenderer->quadCount() || row >= rowCount)
        {
            break;
        }
        
        setupGridRow(-posY, itemIndex, quadIndex);
                        
        posY += stepY;
        row++;
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
    int colsLeft = colCount / 3;

    qreal stepX = mSpacing3D.width() + mImageSize3D.width();
    qreal ipos = floorf(position.x());
    qreal frac = (position.x() - ipos) * stepX;
    qreal posX = -(qreal)colsLeft * stepX - frac;    
    
    posX -= 0.5f - stepX / 2.0;

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
            continue;
        }
        else if (itemIndex >= count || col >= colCount || quadIndex >= mRenderer->quadCount())
        {
            break;
        }
        
        setupGridColumn(posX, itemIndex, quadIndex);
                        
        posX += stepX;
        col++;
    }
}

void HgMediaWallRenderer::setupGridColumn(qreal posX, int& itemIndex, int& quadIndex)
{
    for (int i = 0; i < mRowCount; i++)
    {
        if (quadIndex >= mRenderer->quadCount() || itemIndex >= mDataProvider->imageCount())
            return;
        
        qreal posY = getRowPosY(i);
        
        // enable reflections for the last row needed
        bool reflections = (i == (mRowCount-1) && reflectionsEnabled());

        setupDefaultQuad(QVector3D(posX, posY, 0), itemIndex++, reflections, quadIndex);
        
    }    
}

void HgMediaWallRenderer::setupGridRow(qreal posY, int& itemIndex, int& quadIndex)
{
    for (int i = 0; i < mColumnCount; i++)
    {
        if (quadIndex >= mRenderer->quadCount() || itemIndex >= mDataProvider->imageCount())
            return;

        qreal posX = getColumnPosX(i);

        setupDefaultQuad(QVector3D(posX, posY, 0), itemIndex++, false, quadIndex);
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
    
    // setup indicator/decorator for the item if needed 
    int flags = mDataProvider->flags(itemIndex);
    const HgImage* indicatorImage = mDataProvider->indicator(flags);
    if (flags != 0 && indicatorImage && quadIndex < mRenderer->quadCount())
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

}

HgQuadRenderer* HgMediaWallRenderer::getRenderer()
{
    return mRenderer;
}

bool HgMediaWallRenderer::getItemPoints(int index, QPolygonF& points) const
{
    QPolygonF poly;
    if (!mRenderer->getQuadTranformedPointsByUserData(poly, QVariant(index)))
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

void HgMediaWallRenderer::setOrientation(Qt::Orientation orientation)
{
    mOrientation = orientation;
}
