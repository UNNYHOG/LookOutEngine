//
//  LOTutorialFireball.cpp
//  LookOut
//
//  Created by Ignatov on 06.11.13.
//
//

#include "LOTutorialFireball.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "LOSettings.h"

const SunnyVector2D tut2_startSnowballPosition = SunnyVector2D(mapSize.x*0.3,mapSize.y*0.5);
const SunnyVector2D tut2_finishSnowballPosition = SunnyVector2D(mapSize.x*0.5,mapSize.y*0.5);
const SunnyVector2D tut2_startFireBallPosition = SunnyVector2D(mapSize.x*0.7,mapSize.y*0.5);
const SunnyVector2D tut2_finishFireBallPosition = SunnyVector2D(mapSize.x*0.53,mapSize.y*0.5);
const float tut2_waitingTime = 0.5;
const float tut2_RollingTime = 1.5;
const float tut2_waiting2Time = 1.5;
const float tut2_almostTouch = 0.85;

void LOTutorialFireball::reset()
{
    LOTutorial::reset();
    
    if (explosion)
        delete explosion;
    explosion = 0;
}

void LOTutorialFireball::update(float deltaTime)
{
    LOTutorial::update(deltaTime);
    
    if (tutorialTime>tut2_waitingTime && tutorialTime<tut2_waitingTime + tut2_RollingTime)
        losSetRollingSnowballSpeed(1);
    else
        losSetRollingSnowballSpeed(0);
    frameNum++;
    
    if (explosion) explosion->update(deltaTime);
}

void LOTutorialFireball::crash()
{
    if (explosion) return;

    SunnyMatrix m;
    explosion = new LOExplosion;
    m = getTranslateMatrix(SunnyVector3D(tut2_finishSnowballPosition.x,tut2_finishSnowballPosition.y,-1));
    explosion->initWithParams(&m, LOExplosion_Fire);
    losPlayFSExplosion(1);
    
    snowBallCrashObj->prepareToCrash(m, SunnyVector2D(-3,0));
}

void LOTutorialFireball::render()
{
    LOTutorial::render();
    
    SunnyMatrix m;
    float time = tutorialTime;
    SunnyMatrix rotationMatrix = sunnyIdentityMatrix;
    losPreUpdateSounds();
    if (tutorialTime<=tut2_waitingTime)
    {
        currentSnowBallPosition = tut2_startSnowballPosition;
        losSetRollingSnowballSpeed(0);
        currentSnowBallPosition = tut2_startSnowballPosition;
        currentFireBallPosition = tut2_startFireBallPosition;
        losAddFireBallSound(&frameNum, (currentSnowBallPosition-currentFireBallPosition).length());
    } else
        if (tutorialTime>tut2_waitingTime && tutorialTime<tut2_waitingTime + tut2_RollingTime*tut2_almostTouch)
        {
            time -= tut2_waitingTime;
            losSetRollingSnowballSpeed(1);
            currentSnowBallPosition = time/tut2_RollingTime*(tut2_finishSnowballPosition-tut2_startSnowballPosition) + tut2_startSnowballPosition;
            currentFireBallPosition = time/tut2_RollingTime*(tut2_finishFireBallPosition-tut2_startFireBallPosition) + tut2_startFireBallPosition;
            rotationMatrix = getRotationYMatrix((currentSnowBallPosition-tut2_startSnowballPosition).length()*rotationSpeed);
            losAddFireBallSound(&frameNum, (currentSnowBallPosition-currentFireBallPosition).length());
        } else
        {
            crash();
            losSetRollingSnowballSpeed(0);
            if (tutorialTime>tut2_waitingTime + tut2_RollingTime + tut2_waiting2Time)
            {
                reset();
                addStandartOkButtonToClose();
                return;
            }
        }
    losUpdateSounds();

    if (tutorialTime<tut2_waitingTime + tut2_RollingTime*tut2_almostTouch)
    {
        m = getScaleMatrix(cellSize*tutorialScale) * rotationMatrix * getTranslateMatrix(SunnyVector3D(currentSnowBallPosition.x,currentSnowBallPosition.y,-5));
        glEnable(GL_DEPTH_TEST);
        LOPlayer::renderForMenu(&m);
        
        bindGameGlobal();
        m = getScaleMatrix(tutorialScale) * getRotationZMatrix(M_PI_2) * getTranslateMatrix(SunnyVector3D(currentFireBallPosition.x,currentFireBallPosition.y,-5));
        LOFireBall::renderForMenu(&m, frameNum);
    }
    
    if (explosion)
    {
        m = getTranslateMatrix(SunnyVector3D(0,0,-4));
        sunnyUseShader(globalShaders[LOS_Textured3DSnow]);
        snowBallCrashObj->update();
        snowBallCrashObj->render(&m);
        
        bindGameGlobal();
        SHTextureBase::bindTexture(objectsTexture);
        sunnyUseShader(globalShaders[LOS_Textured3DA]);
        LOExplosion::prepareForRender();
        explosion->render(&m);
    }
}

void LOTutorialFireball::setVisible(bool visible)
{
    LOTutorial::setVisible(visible);
    
    reset();
    
    if (LOSettings::checkTurotialFlag(TUTORIAL_FIREBALL))
        addStandartOkButtonToClose();
    else
        LOSettings::addTutorialFlag(TUTORIAL_FIREBALL);
}


LOTutorialFireball::LOTutorialFireball()
{
    setName("AvoidFireBalls");
    frameNum = 0;
    
    explosion = 0;
}

LOTutorialFireball::~LOTutorialFireball()
{
    if (explosion)
        delete explosion;
}

bool LOTutorialFireball::renderOverlay()
{
    SunnyMatrix m;
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
    }
    
    return true;
}