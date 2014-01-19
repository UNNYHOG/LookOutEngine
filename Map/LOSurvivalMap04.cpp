//
//  LOSurvivalMap.cpp
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//

#include "LOSurvivalMap04.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOEvents.h"

LOSurvivalMap04::LOSurvivalMap04()
{
    reset();
}

LOSurvivalMap04::~LOSurvivalMap04()
{
    
}

void LOSurvivalMap04::reset()
{
    activeMap->applyGlobalColor(1);
    nextRespawnTime[0] = nextRespawnTime[1] = nextRespawnTime[2] = 2;
    respawnPeriod[0] = 3;
    respawnPeriod[1] = 3.2;
    respawnPeriod[2] = 3.4;
    respawnPeriod[4] = 5.1;
    respawnPeriod[5] = 6.3;
    nextRespawnTime[4] = 6;
    nextRespawnTime[5] = 4;
    
    nextRespawnTime[3] = 1;
    respawnPeriod[3] = 2;
    enemySpeed = 1;
}

void LOSurvivalMap04::loadMap(FILE * stream)
{
    reset();
    fread(nextRespawnTime, sizeof(float), 6, stream);
}

void LOSurvivalMap04::saveMap(FILE * stream)
{
    fwrite(nextRespawnTime, sizeof(float), 6, stream);
}

void LOSurvivalMap04::update()
{
    if (playingTime > 5*60)
        enemySpeed = 1 + fminf(1, (playingTime-5*60)*0.01);
    
    respawnPeriod[0] = fmaxf(2,3 - playingTime/300);
    respawnPeriod[1] = fmaxf(2,3.2 - playingTime/300);
    respawnPeriod[2] = fmaxf(2,3.4 - playingTime/300);
    respawnPeriod[4] = fmaxf(4.1,5.1 - playingTime/300);
    respawnPeriod[5] = fmaxf(4,6.3 - playingTime/300);
    
    //snowflakes
    if (realPlayingTime>nextRespawnTime[3] && activeMap->violetSnowflakesCount<=8)
    {
        const float d = 3;
        SunnyVector2D pos = SunnyVector2D(sRandomf()*(mapSize.x-5*d)+d,sRandomf()*(mapSize.y-2*d)+d);
        
        if (controlType==LOC_Joystic)//Do not respawn snowflakes at the Joustic position
        {
            if (pos.x<mapSize.x/3 && pos.y<mapSize.y/2)
            {
                if (sRandomi()%2==0)
                    pos.x = sRandomf()*(mapSize.x*2/3-4*d)+mapSize.x/3;
                else
                    pos.y = sRandomf()*(mapSize.y/2-d)+mapSize.y/2;
            }
        }
        activeMap->createVioletSnowflake(pos);
        nextRespawnTime[3] = realPlayingTime + respawnPeriod[3];
    }
    
    //fire
    if (realPlayingTime>nextRespawnTime[0])
    {
        activeMap->createRandomFireBall(enemySpeed);
        nextRespawnTime[0] = realPlayingTime + respawnPeriod[0];
    }
    
    //ground
    if (realPlayingTime>nextRespawnTime[1])
    {
        LOEvents::OnGroundBallsAppeared();
        activeMap->createRandomGroundBall(enemySpeed);
        nextRespawnTime[1] = realPlayingTime + respawnPeriod[1];
    }
    
    //water
    if (realPlayingTime>nextRespawnTime[2])
    {
        LOEvents::OnWaterBallsAppeared();
        activeMap->createRandomWaterBall(enemySpeed);
        nextRespawnTime[2] = realPlayingTime + respawnPeriod[2];
    }
    
    //electric
    if (realPlayingTime>nextRespawnTime[4])
    {
        LOEvents::OnWaterBallsAppeared();
        activeMap->createRandomElectricBall(enemySpeed);
        nextRespawnTime[4] = realPlayingTime + respawnPeriod[4];
    }
    
    //angry
    if (realPlayingTime>nextRespawnTime[5])
    {
        LOEvents::OnWaterBallsAppeared();
        activeMap->createRandomAngryBall();
        nextRespawnTime[5] = realPlayingTime + respawnPeriod[5];
    }
}


