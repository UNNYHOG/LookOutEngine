//
//  LOTutorialSnowflake.cpp
//  LookOut
//
//  Created by Ignatov on 01.11.13.
//
//

#include "LOTutorialSnowflake.h"
#include "LookOutEngine.h"
#include "LOGlobalVar.h"
#include "LOSettings.h"

const SunnyVector2D tut1_startSnowballPosition = SunnyVector2D(mapSize.x*0.3,mapSize.y*0.6);
const SunnyVector2D tut1_snowflakePositions[3] = {SunnyVector2D(mapSize.x/2,mapSize.y*0.6),SunnyVector2D(mapSize.x/2,mapSize.y*0.4),SunnyVector2D(mapSize.x*0.7,mapSize.y*0.4)};

const float tut1_waitingTime = 0.5;
const float tut1_horizontalRollingTime = 1;
const float tut1_verticalRollingTime = tut1_horizontalRollingTime*mapSize.y/mapSize.x;
const float tut1_waiting2Time = 0.5;

const float tut1_almostTouch = 0.65;

void LOTutorialSnowflake::reset()
{
    LOTutorial::reset();
    
    for (short i = 0;i<3;i++)
        showflakeState[i] = 0;
}

void LOTutorialSnowflake::update(float deltaTime)
{
    LOTutorial::update(deltaTime);
    if (tutorialTime>tut1_waitingTime && tutorialTime<tut1_waitingTime + 2*tut1_horizontalRollingTime + tut1_verticalRollingTime)
        losSetRollingSnowballSpeed(1);
    else
        losSetRollingSnowballSpeed(0);
}

void LOTutorialSnowflake::render()
{
    LOTutorial::render();
    
    SunnyMatrix m;
    
   // SunnyVector2D joyPos = SunnyVector2D(0,0);
    float time = tutorialTime;
    SunnyMatrix rotationMatrix = sunnyIdentityMatrix;
    if (time < tut1_waitingTime)
    {
        currentSnowBallPosition = tut1_startSnowballPosition;
    } else
    {
        time -= tut1_waitingTime;
        if (time<tut1_horizontalRollingTime)
        {
            //joyPos = SunnyVector2D(1,0);
            currentSnowBallPosition = time/tut1_horizontalRollingTime*(tut1_snowflakePositions[0]-tut1_startSnowballPosition) + tut1_startSnowballPosition;
            if (time>tut1_horizontalRollingTime*tut1_almostTouch && showflakeState[0] == 0)
            {
                showflakeState[0] = 1;
                losPlaySnowflakePickUp();
            }
            rotationMatrix = getRotationYMatrix(-(currentSnowBallPosition-tut1_startSnowballPosition).length()*rotationSpeed);
        } else
        {
            rotationMatrix = getRotationYMatrix(-(tut1_snowflakePositions[0]-tut1_startSnowballPosition).length()*rotationSpeed);
            time -= tut1_horizontalRollingTime;
            if (time<tut1_verticalRollingTime)
            {
                //joyPos = SunnyVector2D(0,-1);
                currentSnowBallPosition = time/tut1_verticalRollingTime*(tut1_snowflakePositions[1]-tut1_snowflakePositions[0]) + tut1_snowflakePositions[0];
                if (time>tut1_verticalRollingTime*tut1_almostTouch && showflakeState[1] == 0)
                {
                    showflakeState[1] = 1;
                    losPlaySnowflakePickUp();
                }
                rotationMatrix = rotationMatrix * getRotationXMatrix((currentSnowBallPosition-tut1_snowflakePositions[0]).length()*rotationSpeed);
            } else
            {
                rotationMatrix = rotationMatrix * getRotationXMatrix((tut1_snowflakePositions[1]-tut1_snowflakePositions[0]).length()*rotationSpeed);
                time -= tut1_verticalRollingTime;
                if (time<tut1_horizontalRollingTime)
                {
                    //joyPos = SunnyVector2D(1,0);
                    currentSnowBallPosition = time/tut1_horizontalRollingTime*(tut1_snowflakePositions[2]-tut1_snowflakePositions[1]) + tut1_snowflakePositions[1];
                    if (time>tut1_horizontalRollingTime*tut1_almostTouch && showflakeState[2] == 0)
                    {
                        showflakeState[2] = 1;
                        losPlaySnowflakePickUp();
                    }
                    rotationMatrix = rotationMatrix * getRotationYMatrix(-(currentSnowBallPosition-tut1_snowflakePositions[1]).length()*rotationSpeed);
                } else
                {
                    rotationMatrix = rotationMatrix * getRotationYMatrix(-(tut1_snowflakePositions[2]-tut1_snowflakePositions[1]).length()*rotationSpeed);
                    time -= tut1_horizontalRollingTime;
                    if (time<tut1_waiting2Time)
                    {
                        currentSnowBallPosition = tut1_snowflakePositions[2];
                    } else
                    {
                        reset();
                        addStandartOkButtonToClose();
                    }
                }
            }
        }
    }
    
    bindGameGlobal();
    //loRenderJoystic(this,SunnyVector2D(mapSize.x/4,mapSize.y/4),joyPos);
    
    for (short i = 0;i<3;i++)
    {
        if (showflakeState[i] == 0)
            LOSnowflake::render(tut1_snowflakePositions[i], tutorialTime, tutorialScale);
        else
            if (showflakeState[i]!=3)
            {
                LOSnowflake::renderDissapear(tut1_snowflakePositions[i], showflakeState[i]-1, tutorialScale);
                showflakeState[i]++;
            }
    }
    
    m = getScaleMatrix(cellSize*tutorialScale) * rotationMatrix * getTranslateMatrix(SunnyVector3D(currentSnowBallPosition.x,currentSnowBallPosition.y,-5));
    
    glEnable(GL_DEPTH_TEST);
    LOPlayer::renderForMenu(&m);
}

void LOTutorialSnowflake::setVisible(bool visible)
{
    LOTutorial::setVisible(visible);
    
    reset();
    
    if (LOSettings::checkTurotialFlag(TUTORIAL_SNOWFLAKES))
        addStandartOkButtonToClose();
    else
        LOSettings::addTutorialFlag(TUTORIAL_SNOWFLAKES);
}


LOTutorialSnowflake::LOTutorialSnowflake()
{
    setName("CollectSnowflakes");
}
                 
bool LOTutorialSnowflake::renderOverlay()
{
    for (short i = 0;i<3;i++)
        if (showflakeState[i] == 0)
            LOSnowflake::renderOverlay(tut1_snowflakePositions[i], tutorialScale);
    
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(currentSnowBallPosition.x,currentSnowBallPosition.y,tutorialScale,tutorialScale));
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(0,0,-1));
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
    
    return true;
}