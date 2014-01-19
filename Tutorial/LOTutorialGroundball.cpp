//
//  LOTutorialGroundball.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 07.11.13.
//
//

#include "LOTutorialGroundball.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "iPhone_Strings.h"
#include "LOSettings.h"

const SunnyVector2D tut3_startSnowballPosition = SunnyVector2D(mapSize.x*0.35,mapSize.y*0.5);
const SunnyVector2D tut3_finishSnowballPosition = SunnyVector2D(mapSize.x*0.65,mapSize.y*0.5);
const SunnyVector2D tut3_CollisionPoint = SunnyVector2D(mapSize.x*0.5,mapSize.y*0.5);
const SunnyVector2D tut3_CollisionGroundPoint = SunnyVector2D(mapSize.x*0.55,mapSize.y*0.5);

const float tut3_waitingTime = 0.3;
const float tut3_RollingTime = 1;
const float tut3_RollingBeforeCollision = 0.5;
const float tut3_RollingAfterCollision = 1.5;
const float tut3_waiting2Time = 0.2;

LOTutorialGroundball::LOTutorialGroundball()
{
    setName("GroundBallEffect");
    frameNum = 0;
    
    explosion = 0;
}

LOTutorialGroundball::~LOTutorialGroundball()
{
    if (explosion)
        delete explosion;
}

void LOTutorialGroundball::reset()
{
    LOTutorial::reset();
    
    if (explosion)
        delete explosion;
    explosion = 0;
    
    centerAngle = sRandomf()*M_PI*2;
    for (short i =0;i<stonesInGroundModelCount;i++)
    {
        angles[i] = SunnyVector2D(sRandomf()*2,sRandomf())*M_PI;
        anglesV[i] = SunnyVector2D(0.5 - sRandomf(),0.5 - sRandomf())*10;
    }
}

void LOTutorialGroundball::update(float deltaTime)
{
    LOTutorial::update(deltaTime);
    if (explosion) explosion->update(deltaTime);
}

void LOTutorialGroundball::setVisible(bool visible)
{
    LOTutorial::setVisible(visible);
    
    reset();
    
    if (LOSettings::checkTurotialFlag(TUTORIAL_GROUNDBALL))
        addStandartOkButtonToClose();
    else
        LOSettings::addTutorialFlag(TUTORIAL_GROUNDBALL);
}

void LOTutorialGroundball::crash()
{
    if (explosion) return;
    
    SunnyMatrix m;
    explosion = new LOExplosion;
    m = getTranslateMatrix(SunnyVector3D(tut3_CollisionGroundPoint.x,tut3_CollisionGroundPoint.y,-1));
    explosion->initWithParams(&m, LOExplosion_Ground);
    losPlayGSExplosion(1);
}

void LOTutorialGroundball::render()
{
    LOTutorial::render();
    
    float time = tutorialTime;
    SunnyMatrix m;
    losPreUpdateSounds();
    float sandFriction = 1;
    bool drawGround = false;
    if (time<=tut3_waitingTime)
    {
        losSetRollingSnowballSpeed(0);
        currentSnowBallPosition = tut3_startSnowballPosition;
    } else
    {
        time -= tut3_waitingTime;
        if (time<=tut3_RollingTime)//roll forth
        {
            losSetRollingSnowballSpeed(1);
            currentSnowBallPosition = time/tut3_RollingTime*(tut3_finishSnowballPosition-tut3_startSnowballPosition) + tut3_startSnowballPosition;
        } else
        {
            time-=tut3_RollingTime;
            if (time<=tut3_RollingTime)//roll back
            {
                losSetRollingSnowballSpeed(1);
                currentSnowBallPosition = -time/tut3_RollingTime*(tut3_finishSnowballPosition-tut3_startSnowballPosition) + tut3_finishSnowballPosition;
            } else
            {
                time-=tut3_RollingTime;
                if (time<=tut3_RollingBeforeCollision)//half way before collision
                {
                    currentSnowBallPosition = time/tut3_RollingBeforeCollision*(tut3_CollisionPoint-tut3_startSnowballPosition) + tut3_startSnowballPosition;
                    currentGroundBallPosition = time/tut3_RollingBeforeCollision*(tut3_CollisionGroundPoint-tut3_finishSnowballPosition) + tut3_finishSnowballPosition;
                    drawGround = true;
                    losSetRollingSnowballSpeed(1);
                    losAddGroundBallSound(&frameNum, (currentSnowBallPosition-currentGroundBallPosition).length());
                    
                    centerAngle -= M_PI*1.5*deltaTime;
                    for (short i = 0;i<stonesInGroundModelCount;i++)
                        angles[i] += anglesV[i]*deltaTime;
                } else
                {
                    crash();
                    time-=tut3_RollingBeforeCollision;
                    if (time<=tut3_RollingAfterCollision)
                    {
                        float angle = (time/tut3_RollingAfterCollision-1)*M_PI_2;
                        sandFriction = (1+sinf(angle));
                        currentSnowBallPosition = sandFriction*(tut3_finishSnowballPosition-tut3_CollisionPoint) + tut3_CollisionPoint;
                        losSetRollingSnowballSpeed(sandFriction);
                    } else
                    {
                        time -= tut3_RollingAfterCollision;
                        if (time<tut3_waiting2Time)
                        {
                            losSetRollingSnowballSpeed(0);
                            currentSnowBallPosition = tut3_finishSnowballPosition;
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
    
    //if (tutorialTime<tut2_waitingTime + tut2_RollingTime*tut2_almostTouch)
    {
        static const SunnyVector3D grColor = SunnyVector3D(0.8,0.4,0.0);
        SunnyVector3D color(1,1,1.15);
        if (sandFriction!=1)
        {
            float k = sandFriction*0.4+0.6;
            color = color*k + (1-k)*grColor;
        }
            
        SunnyMatrix rotationMatrix = getRotationYMatrix(-(currentSnowBallPosition-tut3_startSnowballPosition).length()*rotationSpeed);
        m = getScaleMatrix(cellSize*tutorialScale) * rotationMatrix * getTranslateMatrix(SunnyVector3D(currentSnowBallPosition.x,currentSnowBallPosition.y,-5));
        glEnable(GL_DEPTH_TEST);
        LOPlayer::renderForMenu(&m,1,color);
    }
    
    SHTextureBase::bindTexture(objectsTexture);
    if (explosion)
    {
        glDisable(GL_DEPTH_TEST);
        bindGameGlobal();
        m = getTranslateMatrix(SunnyVector3D(0,0,-4));
        sunnyUseShader(globalShaders[LOS_Textured3DA]);
        LOExplosion::prepareForRender();
        explosion->render(&m);
    } else
    {
        glEnable(GL_DEPTH_TEST);
        if (drawGround)
        {
            m = getScaleMatrix(tutorialScale) * getTranslateMatrix(SunnyVector3D(currentGroundBallPosition.x,currentGroundBallPosition.y,-5));
            LOGroundBall::renderForMenu(&m,centerAngle,angles);
        }
    }
}

bool LOTutorialGroundball::renderOverlay()
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