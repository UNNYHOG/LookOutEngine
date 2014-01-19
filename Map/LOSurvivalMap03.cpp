//
//  LOSurvivalMap.cpp
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#include "LOSurvivalMap03.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOEvents.h"

const char electricNumbers[6][8] = {{0,1,25,24,7,6,20,21},{2,3,29,28,9,8,24,25},{4,5,33,32,11,10,28,29},{6,7,23,22,13,12,18,19},{8,9,27,26,15,14,22,23},{10,11,31,30,17,16,26,27}};

LOSurvivalMap03::LOSurvivalMap03()
{
    activeMap->mapInfo.electricFieldsCount = 34;
    LOBufElectricField *bufElectricFields = new LOBufElectricField[activeMap->mapInfo.electricFieldsCount];
    
    for (short i = 0;i<18;i++)
    {
        bufElectricFields[i].position = SunnyVector2D(5*(i%6)+2.5,20-10*(i/6));
        bufElectricFields[i].type = 1;
    }
    
    for (short i = 0;i<16;i++)
    {
        bufElectricFields[18+i].position = SunnyVector2D((i/4)*10, 2.5 + 5*(i%4));
        bufElectricFields[18+i].type = 0;
    }
    
    activeMap->mapInfo.electricFields = new LORollingBlades[activeMap->mapInfo.electricFieldsCount];
    for (short i = 0 ;i<activeMap->mapInfo.electricFieldsCount;i++)
    {
        bufElectricFields[i].params = SunnyVector3D(0,-1,0);
        activeMap->mapInfo.electricFields[i].initElectricity(bufElectricFields[i]);
    }
    
    delete []bufElectricFields;
    reset();
}

LOSurvivalMap03::~LOSurvivalMap03()
{
    
}

void LOSurvivalMap03::reset()
{
    activeMap->applyGlobalColor(1);
    nextRespawnTime[0] = nextRespawnTime[1] = nextRespawnTime[2] = nextRespawnTime[4] = 2;
    respawnPeriod[0] = 0.7;
    respawnPeriod[1] = 1.4;
    respawnPeriod[2] = 1.6;
    respawnPeriod[4] = 5;
    
    nextRespawnTime[3] = 1;
    respawnPeriod[3] = 2;
    eclipseNumber = 0;
    enemySpeed = 1;
    calcNextEclipse();
}

void LOSurvivalMap03::calcNextEclipse()
{
    for (short i = 0;i<8;i++)
        activeMap->mapInfo.electricFields[electricNumbers[eclipseNumber][i]].charged = false;
    
    nextEclipse = realPlayingTime + 20 + sRandomf()*30;
    eclipseNumber = sRandomi()%6;
    short max = 3;
    while (eclipseNumber==5 || (eclipseNumber==3 && controlType == LOC_Joystic))
    {
        eclipseNumber = sRandomi()%6;
        max--;
        if (max==0)
            break;
    }
    eclipseStartNumber = sRandomi()%8;
    if (sRandomi()%2 == 0) eclipseStartNumber = -eclipseStartNumber;
    systemsOn = false;
    eclipseTimer = 1;
}

void LOSurvivalMap03::loadMap(FILE * stream)
{
    reset();
    fread(nextRespawnTime, sizeof(float), 5, stream);
    fread(&nextEclipse, sizeof(nextEclipse), 1, stream);
    fread(&eclipseNumber, sizeof(eclipseNumber), 1, stream);
    fread(&eclipseStartNumber, sizeof(eclipseStartNumber), 1, stream);
    fread(&eclipseTimer, sizeof(eclipseTimer), 1, stream);
    fread(&systemsOn, sizeof(systemsOn), 1, stream);
    for (short i = 0;i<8;i++)
        fread(&(activeMap->mapInfo.electricFields[electricNumbers[eclipseNumber][i]].charged),sizeof(short),1,stream);
}

void LOSurvivalMap03::saveMap(FILE * stream)
{
    fwrite(nextRespawnTime, sizeof(float), 5, stream);
    fwrite(&nextEclipse, sizeof(nextEclipse), 1, stream);
    fwrite(&eclipseNumber, sizeof(eclipseNumber), 1, stream);
    fwrite(&eclipseStartNumber, sizeof(eclipseStartNumber), 1, stream);
    fwrite(&eclipseTimer, sizeof(eclipseTimer), 1, stream);
    fwrite(&systemsOn, sizeof(systemsOn), 1, stream);
    for (short i = 0;i<8;i++)
        fwrite(&(activeMap->mapInfo.electricFields[electricNumbers[eclipseNumber][i]].charged),sizeof(short),1,stream);
}

void LOSurvivalMap03::update()
{
    if (playingTime > 5*60)
        enemySpeed = 1 + fminf(0.26, (playingTime-5*60)*0.01);
    
    float T = 1;
    if (realPlayingTime>=nextEclipse)
    {
        if (!gamePaused)
        {
            if (systemsOn)
            {
                T = 1;
                eclipseTimer -= deltaTime;
                if (eclipseTimer<=0)
                {
                    T = 1;
                    globalColor.x += deltaTime*0.2;
                    if (globalColor.x>=1)
                    {
                        globalColor.x = 1;
                        calcNextEclipse();
                    }
                    activeMap->applyGlobalColor(globalColor.x);
                }
            } else
            if (globalColor.x>=0.301)
            {
                globalColor.x = 1 - (realPlayingTime-nextEclipse)*0.07;
                if (globalColor.x<0.3) globalColor.x = 0.3;
                activeMap->applyGlobalColor(globalColor.x);
                T = 3;
            } else
            {
                eclipseTimer -= deltaTime;
                if (eclipseTimer<=0)
                {
                    eclipseTimer = 1.5;
                    if (!activeMap->mapInfo.electricFields[electricNumbers[eclipseNumber][abs(eclipseStartNumber)%8]].charged)
                    {
                        activeMap->mapInfo.electricFields[electricNumbers[eclipseNumber][abs(eclipseStartNumber)%8]].charged = true;
                        if (eclipseStartNumber>0)
                            eclipseStartNumber++;
                        else
                            eclipseStartNumber--;
                    } else
                    {
                        systemsOn = true;
                        eclipseTimer = 5;
                    }
                }
            }
        }
    }
    
    //snowflakes
    if (realPlayingTime>nextRespawnTime[3])
    {
        if (realPlayingTime>=nextEclipse)
        {
            SunnyVector2D centerPos(5+10*(eclipseNumber%3), 15 - 10*(eclipseNumber/3));
            short bound = -1;
            SunnyVector2D r = activeMap->randPosOnMapBounds(bound);
            centerPos -= r;
            centerPos.normalize();
            activeMap->createVioletSnowflake(r,centerPos);
        }
        else
            activeMap->createRandomVioletSnowflake(1);
        nextRespawnTime[3] = realPlayingTime + respawnPeriod[3];
    }
    
    //fire
    if (realPlayingTime>nextRespawnTime[0])
    {
        activeMap->createRandomFireBall(enemySpeed);
        nextRespawnTime[0] = realPlayingTime + respawnPeriod[0]*T;
    }
    
    //ground
    if (playingTime>20)
    {
        respawnPeriod[0] = 1.2;
        if (realPlayingTime>nextRespawnTime[1])
        {
            LOEvents::OnGroundBallsAppeared();
            if (realPlayingTime>=nextEclipse)
                activeMap->createRandomFireBall(enemySpeed);
            else
                activeMap->createRandomGroundBall(enemySpeed);
            nextRespawnTime[1] = realPlayingTime + respawnPeriod[1]*T;
        }
        
        //water
        if (playingTime>40)
        {
            if (realPlayingTime>nextRespawnTime[2])
            {
                LOEvents::OnWaterBallsAppeared();
                if (realPlayingTime>=nextEclipse)
                    activeMap->createRandomFireBall(enemySpeed);
                else
                    activeMap->createRandomWaterBall(enemySpeed);
                nextRespawnTime[2] = realPlayingTime + respawnPeriod[2]*T;
            }
        }
        
        //electric
        if (playingTime>100 && realPlayingTime<nextEclipse)
        {
            if (realPlayingTime>nextRespawnTime[4])
            {
                activeMap->createRandomElectricBall(enemySpeed);
                nextRespawnTime[4] = realPlayingTime + respawnPeriod[4]*T;
            }
        }
    }
}
