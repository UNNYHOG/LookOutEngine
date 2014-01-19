//
//  LOTutorialGroundball.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 07.11.13.
//

#include "LOTutorialWaterball.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "iPhone_Strings.h"
#include "LOSettings.h"

const SunnyVector2D tut4_startSnowballPosition = SunnyVector2D(mapSize.x*0.3,mapSize.y*0.5);
const SunnyVector2D tut4_finishSnowballPosition = SunnyVector2D(mapSize.x*0.5,mapSize.y*0.5);
const SunnyVector2D tut4_finishSnowballWaterPosition = SunnyVector2D(mapSize.x*0.8,mapSize.y*0.5);
const SunnyVector2D tut4_CollisionPoint = SunnyVector2D(mapSize.x*0.5,mapSize.y*0.5);
const SunnyVector2D tut4_CollisionWaterPoint = SunnyVector2D(mapSize.x*0.54,mapSize.y*0.5);

const float tut4_waitingTime = 0.3;
const float tut4_RollingTime = 0.6;
const float tut4_RollingBeforeCollision = 0.6;
const float tut4_RollingAfterCollision = 1.2;
const float tut4_waiting2Time = 0.3;

LOTutorialWaterball::LOTutorialWaterball()
{
    setName("WaterBallEffect");
    frameNum = 0;
    
    explosion = 0;
    frameSize = SunnyVector2D(mapSize.x*0.8,mapSize.y*0.7);
}

LOTutorialWaterball::~LOTutorialWaterball()
{
    if (explosion)
        delete explosion;
}

void LOTutorialWaterball::reset()
{
    LOTutorial::reset();
    
    if (explosion)
        delete explosion;
    explosion = 0;
}

void LOTutorialWaterball::update(float deltaTime)
{
    LOTutorial::update(deltaTime);
    if (explosion) explosion->update(deltaTime);
}

void LOTutorialWaterball::setVisible(bool visible)
{
    LOTutorial::setVisible(visible);
    
    reset();
    
    if (LOSettings::checkTurotialFlag(TUTORIAL_WATERBALL))
        addStandartOkButtonToClose();
    else
        LOSettings::addTutorialFlag(TUTORIAL_WATERBALL);
}

void LOTutorialWaterball::crash()
{
    if (explosion) return;
    
    SunnyMatrix m;
    explosion = new LOExplosion;
    m = getRotationZMatrix(M_PI_2)* getTranslateMatrix(SunnyVector3D(tut4_CollisionWaterPoint.x,tut4_CollisionWaterPoint.y,-1));
    explosion->initWithParams(&m, LOExplosion_Water);
    losPlayWSExplosion(1);
}

void LOTutorialWaterball::render()
{
    LOTutorial::render();
    
    float time = tutorialTime;
    SunnyMatrix m;
    losPreUpdateSounds();
    float waterFriction = 0;
    bool drawGround = false;
    if (time<=tut4_waitingTime)
    {
        losSetRollingSnowballSpeed(0);
        currentSnowBallPosition = tut4_startSnowballPosition;
    } else
    {
        time -= tut4_waitingTime;
        if (time<=tut4_RollingTime)//roll forth
        {
            losSetRollingSnowballSpeed(1);
            currentSnowBallPosition = time/tut4_RollingTime*(tut4_finishSnowballPosition-tut4_startSnowballPosition) + tut4_startSnowballPosition;
        } else
        {
            time-=tut4_RollingTime;
            if (time<=tut4_RollingTime)//roll back
            {
                losSetRollingSnowballSpeed(1);
                currentSnowBallPosition = -time/tut4_RollingTime*(tut4_finishSnowballPosition-tut4_startSnowballPosition) + tut4_finishSnowballPosition;
            } else
            {
                time-=tut4_RollingTime;
                if (time<=tut4_RollingBeforeCollision)//half way before collision
                {
                    currentSnowBallPosition = time/tut4_RollingBeforeCollision*(tut4_CollisionPoint-tut4_startSnowballPosition) + tut4_startSnowballPosition;
                    currentWaterBallPosition = time/tut4_RollingBeforeCollision*(tut4_CollisionWaterPoint-tut4_finishSnowballWaterPosition) + tut4_finishSnowballWaterPosition;
                    drawGround = true;
                    losSetRollingSnowballSpeed(1);
                    losAddWaterBallSound(&frameNum, (currentSnowBallPosition-currentWaterBallPosition).length());
                } else
                {
                    crash();
                    time-=tut4_RollingBeforeCollision;
                    if (time<=tut4_RollingAfterCollision)
                    {
                        float angle = (time/tut4_RollingAfterCollision)*M_PI_2;
                        waterFriction = sinf(angle);
                        currentSnowBallPosition = waterFriction*(tut4_finishSnowballWaterPosition-tut4_CollisionPoint) + tut4_CollisionPoint;
                        waterFriction = 1-waterFriction;
                        losSetRollingSnowballSpeed(waterFriction);
                    } else
                    {
                        time -= tut4_RollingAfterCollision;
                        if (time<tut4_waiting2Time)
                        {
                            losSetRollingSnowballSpeed(0);
                            currentSnowBallPosition = tut4_finishSnowballWaterPosition;
                        } else
                        {
                            reset();
                            addStandartOkButtonToClose();
                            return;
                        }
                    }
                }
            }
        }
    }
    
    losUpdateSounds();
    
    {
        static const SunnyVector3D wColor = SunnyVector3D(0,0.3,1.0);
        SunnyVector3D color(1,1,1.15);
        if (waterFriction!=0)
        {
            float k = waterFriction*0.4;
            color = color*(1-k) + k*wColor;
        }
            
        SunnyMatrix rotationMatrix = getRotationYMatrix((currentSnowBallPosition-tut4_startSnowballPosition).length()*rotationSpeed);
        m = getScaleMatrix(cellSize*tutorialScale) * rotationMatrix * getTranslateMatrix(SunnyVector3D(currentSnowBallPosition.x,currentSnowBallPosition.y,-5));
        glEnable(GL_DEPTH_TEST);
        LOPlayer::renderForMenu(&m,1,color);
    }
    
    glDisable(GL_DEPTH_TEST);
    if (explosion)
    {
        bindGameGlobal();
        SHTextureBase::bindTexture(objectsTexture);
        m = getTranslateMatrix(SunnyVector3D(0,0,-4));
        sunnyUseShader(globalShaders[LOS_Textured3DA]);
        LOExplosion::prepareForRender();
        explosion->render(&m);
    } else
    {
        if (drawGround)
        {
            m = getScaleMatrix(tutorialScale) * getTranslateMatrix(SunnyVector3D(currentWaterBallPosition.x,currentWaterBallPosition.y,-5));
            //LOGroundBall::renderForMenu(&m,centerAngle,angles);
            LOWaterBall::renderForMenu(&m);
        }
    }
}

bool LOTutorialWaterball::renderOverlay()
{
/*    SunnyMatrix m;
    if (tutorialTime<tut2_waitingTime + tut2_RollingTime*tut2_almostTouch)
    {
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(currentSnowBallPosition.x,currentSnowBallPosition.y,tutorialScale,tutorialScale));
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
        m = getTranslateMatrix(SunnyVector3D(0,0,-1));
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOverlaySnowflake_VAO);
        
        m = getScaleMatrix(tutorialScale) * getRotationZMatrix(M_PI_2) * getTranslateMatrix(SunnyVector3D(currentFireBallPosition.x,currentFireBallPosition.y,-5));
        LOFireBall::renderOverlayForMenu(&m);
    }
    
    if (explosion)
    {
        m = getTranslateMatrix(SunnyVector3D(0,0,-4));
        explosion->renderOverlay(&m);
    }*/
    
    return true;
}