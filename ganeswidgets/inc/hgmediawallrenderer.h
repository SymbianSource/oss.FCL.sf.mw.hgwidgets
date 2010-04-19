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
#ifndef HGMEDIAWALLRENDERER_H
#define HGMEDIAWALLRENDERER_H

#include <qmatrix4x4>
#include <qobject>

class HgQuadRenderer;
class HgMediaWallDataProvider;
class QPointF;
class QPainter;
class QTimer;
class QStateMachine;
class HgAnimatedQuad;
class HgQuad;
class HgImageFader;
class HgImage;
class QPolygonF;

/**
 * MediaWall rendering engine class.
 */
class HgMediaWallRenderer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal animationAlpha READ animationAlpha WRITE setAnimationAlpha)
    Q_PROPERTY(qreal stateAnimationAlpha READ stateAnimationAlpha WRITE setStateAnimationAlpha)
public:

    enum OpeningAnimationType
    {
        OpeningAnimationFlip,
        OpeningAnimationZoomIn,
        OpeningAnimationZoomOver
    };
            
    explicit HgMediaWallRenderer(HgMediaWallDataProvider* provider);
    
    virtual ~HgMediaWallRenderer();

    void setCameraDistance(qreal distance);    
    qreal getCameraDistance() const;
    void setCameraRotationY(qreal angle);    
    qreal getCameraRotationY() const;    
    void setCameraRotationZ(qreal angle);    
    qreal getCameraRotationZ() const;
        
    void draw(
        const QPointF& startPosition,
        const QPointF& position, 
        const QPointF& targetPosition,
        qreal springVelocity,
        QPainter* painter);
    
    void setFlipAnimationAngle(qreal angleInDegrees);
    
    void setOpeningAnimationType(OpeningAnimationType type);
    
    void setOpeningAnimationDuration(int msecs);
    
    void setOrientation(Qt::Orientation orientation, bool animate=false);        
    Qt::Orientation getOrientation() const;
        
    void enableCoverflowMode(bool enabled);    
    bool coverflowModeEnabled() const;
    
    void setRowCount(int rowCount, const QSizeF& newImageSize, bool animate=false);    
    int getRowCount() const;
    
    HgQuad* getQuadAt(const QPointF& position) const;
        
    void setRect(const QRectF& windowRect);    
    const QRectF& getRect() const;    
    void setSpacing(const QSizeF& spacing);    
    const QSizeF& getSpacing() const;    
    void setImageSize(const QSizeF& imageSize);    
    const QSizeF& getImageSize() const;

    void setFrontCoverElevationFactor(qreal elevation);    
    qreal getFrontCoverElevationFactor() const;

    void openItem(int index, bool animate);
    void closeItem(bool animate);
    bool isItemOpen() const;    
    
    void enableReflections(bool enabled);    
    bool reflectionsEnabled() const;
    
    qreal getWorldWidth() const;
    
    void beginRemoveRows(int start, int end);    
    void endRemoveRows();

    int getSelectedItem() const;
    
    void initialize(QPainter* painter);
    bool initialized() const;
    
    HgQuadRenderer* getRenderer();    

    bool getItemPoints(int index, QPolygonF& points) const;
    
    QList<HgQuad*> getVisibleQuads() const;
    
signals:
    void renderingNeeded();
    void itemOpened(int index);
    void itemClosed(int index);
    void itemMarked(int index);
    void toggleItem();
    void toggleState();
    void itemsRemoved(int start, int end);
private slots:
    void onIdleState();
    void onOpenedState();
    void emitUpdate();
protected:            
    
    struct State
    {
        QList<HgQuad*> mQuads;
    };
   
    void setAnimationAlpha(qreal alpha);
    qreal animationAlpha() const;

    void createStateMachine();
    void recordState(State& state);
    
    void updateStateAnimation(qreal alpha);    
    void setStateAnimationAlpha(qreal alpha);
    qreal stateAnimationAlpha() const;
    void setupStateAnimation(QPainter* painter);
    
    void setupRows(
        const QPointF& startPosition,
        const QPointF& position, 
        const QPointF& targetPosition,
        qreal springVelocity,
        QPainter* painter);
        
    void drawQuads(QPainter* painter);
    void resetQuads();
    void updateCameraMatrices();
    void updateSpacingAndImageSize();
    qreal getRowPosY(int row);
    qreal getColumnPosX(int column);
    QPointF mapFromWindow(const QPointF& point) const;
    void applyOpeningAnimation(HgQuad* quad);
    void startStateAnimation(QPainter* painter);
    
    
    void setupCoverflow(const QPointF& startPosition,
        const QPointF& position, 
        const QPointF& targetPosition, 
        qreal springVelocity,
        QPainter* painter);
    
    void setupGridPortrait(const QPointF& startPosition,
        const QPointF& position, 
        const QPointF& targetPosition, 
        qreal springVelocity,
        QPainter* painter);

    void setupGridLandscape(const QPointF& startPosition,
        const QPointF& position, 
        const QPointF& targetPosition, 
        qreal springVelocity,
        QPainter* painter);

    void setupGridRow(qreal posY, int itemIndex, int& quadIndex);
    void setupGridColumn(qreal posX, int itemIndex, int& quadIndex);    
    void setupDefaultQuad(const QVector3D& pos, int itemIndex, bool reflectionsEnabled, int& quadIndex);
    void setupIndicator(HgQuad* parent, 
        HgQuad* indicator, const HgImage* indicatorImage, int itemIndex);    
    bool initializeRenderer(QPainter* painter);
        
protected:
    
    enum ItemState
    {
        ItemClosed,
        ItemOpening,
        ItemOpened,
        ItemClosing
    };
    
    HgMediaWallDataProvider* mDataProvider;
    HgQuadRenderer* mRenderer;
    HgQuadRenderer* mIndicatorRenderer;
    bool mRendererInitialized;

    Qt::Orientation mOrientation;
    Qt::Orientation mNextOrientation;

    qreal mStateAnimationAlpha;
    bool mStateAnimationOnGoing;

    qreal mAnimationAlpha;

    OpeningAnimationType mOpeningAnimationType;
    int mOpeningAnimationDuration;
    int mOpenedItem;
    int mSelectedItem;
    qreal mFlipAngle;
    QTimer* mAnimationTimer;
    QMatrix4x4 mViewMatrix;
    QMatrix4x4 mProjMatrix;
    QStateMachine* mStateMachine;
    qreal mZoomAmount;
    bool mCoverflowMode;
    int mRowCount;
    int mNextRowCount;

    QList<HgAnimatedQuad*> mAnimatedQuads;
    int mStateAnimationDuration;
    qreal mStep;
    qreal mZfar;
    QRectF mRect;
    
    QSizeF mSpacing2D;
    QSizeF mSpacing3D;
    QSizeF mImageSize2D;
    QSizeF mImageSize3D;
    
    qreal mCameraDistance;
    qreal mCameraRotationY;
    qreal mCameraRotationZ;
        
    qreal mFrontCoverElevation;
    
    State mOldState;
    State mNextState;

    
    bool mReflectionsEnabled;
    HgImageFader* mImageFader;
    
    QSizeF mNextImageSize;
            
    bool mItemCountChanged;
    int mRemoveStart;
    int mRemoveEnd;
    int mInsertStart;
    int mInsertEnd;
    int mColumnCount;
    
    ItemState mOpenedItemState;
    
private:
    Q_DISABLE_COPY(HgMediaWallRenderer)
};

#endif
