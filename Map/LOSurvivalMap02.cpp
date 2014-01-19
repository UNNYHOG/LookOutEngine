//
//  LOSurvivalMap.cpp
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#include "LOSurvivalMap02.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOEvents.h"

LOSurvivalMap02::LOSurvivalMap02()
{
    reset();
}

LOSurvivalMap02::~LOSurvivalMap02()
{
    
}

void LOSurvivalMap02::reset()
{
    activeMap->applyGlobalColor(1);
    nextRespawnTime[0] = nextRespawnTime[1] = nextRespawnTime[2] = 2;
    respawnPeriod[0] = 0.7;
    respawnPeriod[1] = 1.4;
    respawnPeriod[2] = 1.6;
    
    nextRespawnTime[3] = 1;
    respawnPeriod[3] = 2;
    enemySpeed = 1;
    
//    playingTime = realPlayingTime = 100;
//    respawnPeriod[0] = respawnPeriod[1] = respawnPeriod[2] = 0.15;
}

void LOSurvivalMap02::loadMap(FILE * stream)
{
    reset();
    fread(nextRespawnTime, sizeof(float), 4, stream);
}

void LOSurvivalMap02::saveMap(FILE * stream)
{
    fwrite(nextRespawnTime, sizeof(float), 4, stream);
}

void LOSurvivalMap02::update()
{
    if (playingTime > 5*60)
    {
        enemySpeed = 1 + fminf(1, (playingTime-5*60)*0.01);
    }
        
    //snowflakes
    if (realPlayingTime>nextRespawnTime[3])
    {
        activeMap->createRandomVioletSnowflake(1);
        nextRespawnTime[3] = realPlayingTime + respawnPeriod[3];
    }
    
    //fire
    if (realPlayingTime>nextRespawnTime[0])
    {
        activeMap->createRandomFireBall(enemySpeed);
        nextRespawnTime[0] = realPlayingTime + respawnPeriod[0];
    }
    
    //ground
    if (playingTime>30)
    {
        respawnPeriod[0] = 1.2;
        if (realPlayingTime>nextRespawnTime[1])
        {
            LOEvents::OnGroundBallsAppeared();
            activeMap->createRandomGroundBall(enemySpeed);
            nextRespawnTime[1] = realPlayingTime + respawnPeriod[1];
        }
        
        //water
        if (playingTime>60)
        {
            if (realPlayingTime>nextRespawnTime[2])
            {
                LOEvents::OnWaterBallsAppeared();
                activeMap->createRandomWaterBall(enemySpeed);
                nextRespawnTime[2] = realPlayingTime + respawnPeriod[2];
            }
        }
    }
}
