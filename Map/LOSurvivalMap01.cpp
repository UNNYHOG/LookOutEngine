//
//  LOSurvivalMap.cpp
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#include "LOSurvivalMap01.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOEvents.h"

const float survival01RespawnPeriod[3] = {0.75,1.5,1.5};
const float survival01Snowflake = 3;

LOSurvivalMap01::LOSurvivalMap01()
{
    reset();
}

LOSurvivalMap01::~LOSurvivalMap01()
{
    
}

void LOSurvivalMap01::reset()
{
    activeMap->applyGlobalColor(1);
    nextRespawnTime[0] = nextRespawnTime[1] = nextRespawnTime[2] = 2;
    for (short i = 0;i<3;i++)
        respawnPeriod[i] = survival01RespawnPeriod[i];
    snowflakePeriod = survival01Snowflake;
    massSnowflakesChance = 0;
    enemySpeed = 1;
    
//    playingTime = realPlayingTime = 250;
}

void LOSurvivalMap01::loadMap(FILE * stream)
{
    reset();
    fread(nextRespawnTime, sizeof(float), 3, stream);
    fread(&massSnowflakesChance, sizeof(massSnowflakesChance), 1, stream);
}

void LOSurvivalMap01::saveMap(FILE * stream)
{
    fwrite(nextRespawnTime, sizeof(float), 3, stream);
    fwrite(&massSnowflakesChance, sizeof(massSnowflakesChance), 1, stream);
}

void LOSurvivalMap01::update()
{
    if (!activeMap->violetSnowflakesCount && (realPlayingTime - activeMap->mapInfo.lastSnowFlakeTime > snowflakePeriod))
    {
        if (sRandomf()<massSnowflakesChance)
        {
            massSnowflakesChance = 0;
            SunnyVector2D pos;
            short r = sRandomi()%3;
            if (r==0)//line
            {
                for (short i = 0;i<8;i++)
                {
                    pos = SunnyVector2D(mapSize.x + 1.49, 1.25 + 2.5*i);
                    activeMap->createVioletSnowflake(pos,SunnyVector2D(-1,0));
                }
                if (sRandomi()%10==0)//second line
                {
                    for (short i = 0;i<8;i++)
                    {
                        pos = SunnyVector2D(mapSize.x + 2.99, 1.25 + 2.5*i);
                        activeMap->createVioletSnowflake(pos,SunnyVector2D(-1,0));
                    }
                }
            } else
            if (r==1)//2 squares
            {
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,13.75),SunnyVector2D(-1,0));
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,16.25),SunnyVector2D(-1,0));
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,13.75),SunnyVector2D(-1,0));
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,16.25),SunnyVector2D(-1,0));
                
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,3.75),SunnyVector2D(-1,0));
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,6.25),SunnyVector2D(-1,0));
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,3.75),SunnyVector2D(-1,0));
                activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,6.25),SunnyVector2D(-1,0));
                
                if (sRandomi()%10==0)//second line
                {
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,13.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,16.25),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,13.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,16.25),SunnyVector2D(-1,0));
                    
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,3.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,6.25),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,3.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,6.25),SunnyVector2D(-1,0));
                }
            } else
                if (r==2)//square in the middle
                {
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,13.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,11.25),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,13.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,11.25),SunnyVector2D(-1,0));
                    
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,8.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 1.49,6.25),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,8.75),SunnyVector2D(-1,0));
                    activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 2.99,6.25),SunnyVector2D(-1,0));
                    
                    if (sRandomi()%10==0)//second line
                    {
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,13.75),SunnyVector2D(-1,0));
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,11.25),SunnyVector2D(-1,0));
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,13.75),SunnyVector2D(-1,0));
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,11.25),SunnyVector2D(-1,0));
                        
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,8.75),SunnyVector2D(-1,0));
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 4.49,6.25),SunnyVector2D(-1,0));
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,8.75),SunnyVector2D(-1,0));
                        activeMap->createVioletSnowflake(SunnyVector2D(mapSize.x + 5.99,6.25),SunnyVector2D(-1,0));
                    }
                }
        } else
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
            
            massSnowflakesChance+=0.05;
        }
    }
    
    const float eclipseTime = 300;
    if (playingTime>eclipseTime)
    {
        globalColor.x = 1.0 - (playingTime-eclipseTime)*0.01;
        if (globalColor.x<0.3) globalColor.x = 0.3;
        activeMap->applyGlobalColor(globalColor.x);
        
        for (short i = 0;i<3;i++)
            respawnPeriod[i] = fmaxf(survival01RespawnPeriod[i]*0.8, survival01RespawnPeriod[i] - (playingTime-eclipseTime)*0.01);
        
        snowflakePeriod = fmaxf(0.1,survival01Snowflake - (playingTime-eclipseTime)*0.05);
        
        enemySpeed = 1.0 + fminf(1,(playingTime-eclipseTime)*0.01);
    }
    
    //fire
    if (realPlayingTime>nextRespawnTime[0])
    {
        SunnyVector2D pos,vel(-enemySpeed,0);
        pos.x = mapSize.x+1;
        if (sRandomi()%4 == 0)
            pos.y = player->getPosition().y;
        else
            pos.y = mapSize.y*sRandomf();
        activeMap->createFireBall(pos, vel);
        nextRespawnTime[0] = realPlayingTime + respawnPeriod[0];
    }
    
    //ground
    if (playingTime>60)
    {
        if (realPlayingTime>nextRespawnTime[1])
        {
            LOEvents::OnGroundBallsAppeared();
            SunnyVector2D pos,vel(-enemySpeed,0);
            pos.x = mapSize.x+1;
            pos.y = 1.25+(mapSize.y-2.5)*sRandomf();
            activeMap->createGroundBall(pos, vel);
            nextRespawnTime[1] = realPlayingTime + respawnPeriod[1];
        }
        
        //water
        if (playingTime>120)
        {
            if (realPlayingTime>nextRespawnTime[2])
            {
                LOEvents::OnWaterBallsAppeared();
                SunnyVector2D pos,vel(-enemySpeed,0);
                pos.x = mapSize.x+1;
                pos.y = 1.25+(mapSize.y-2.5)*sRandomf();
                activeMap->createWaterBall(pos, vel);
                nextRespawnTime[2] = realPlayingTime + respawnPeriod[2];
            }
        }
    }
}
