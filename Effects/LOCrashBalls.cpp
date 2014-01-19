//
//  LOCrashBalls.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 16.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "LOCrashBalls.h"
#include "LOGlobalVar.h"
#include "stdlib.h"
#include "iPhone_Strings.h"
#include "LOTrace.h"

LOCrashBalls *_sharedCrashBalls = 0;

LOCrashBalls::LOCrashBalls()
{
    fireBallsCount = groundBallsCount = snowWallCount = 0;
}

LOCrashBalls::~LOCrashBalls()
{
    _sharedCrashBalls = 0;
}

LOCrashBalls * LOCrashBalls::sharedCrashBalls()
{
    if (!_sharedCrashBalls) _sharedCrashBalls = new LOCrashBalls;
    
    return _sharedCrashBalls;
}

void LOCrashBalls::reset()
{
    fireBallsCount = groundBallsCount = snowWallCount = 0;
}

void LOCrashBalls::update()
{
//    SunnyMatrix rot = getIdentityMatrix();
    for (short i = 0;i<fireBallsCount;i++)
    {
        short dest = 0;
        for (short j = 0;j<stonesInFireModelCount;j++)
        {
            fireBalls[i].pos[j].pos.x += fireBalls[i].velocity[j].x*deltaTime;
            fireBalls[i].pos[j].pos.y += fireBalls[i].velocity[j].y*deltaTime;
            fireBalls[i].angles[j] += fireBalls[i].rotationSpeed[j]*deltaTime;
            if (fireBalls[i].pos[j].pos.z>0)
            {
                fireBalls[i].pos[j].pos.z += (fireBalls[i].velocity[j].z - 2*deltaTime)*deltaTime;
                fireBalls[i].velocity[j].z -= 4*deltaTime;
            } else
                if (fireBalls[i].pos[j].pos.z>-cellSize)
                {
                    fireBalls[i].velocity[j] *= 0.95;
                    fireBalls[i].rotationSpeed[j] *= 0.95;
                    fireBalls[i].pos[j].pos.z -= deltaTime/3;
                    if (fabsf(fireBalls[i].velocity[j].x) + fabsf(fireBalls[i].velocity[j].y)>0.1 && (rand()%10==0)) 
                        LOTrace::sharedTrace()->addRandomTrace(SunnyVector2D(fireBalls[i].pos[j].pos.x,fireBalls[i].pos[j].pos.y),0.3);
                } else 
                {
                    dest++;
                }
        }
        if (dest ==stonesInFireModelCount)
        {
            if (i!=fireBallsCount-1)
                fireBalls[i] = fireBalls[fireBallsCount-1];
            i--;
            fireBallsCount--;
            continue;
        }
    }
    
    for (short i = 0;i<groundBallsCount;i++)
    {
        short dest = 0;
        for (short j = 0;j<stonesInGroundModelCount;j++)
        {
            groundBalls[i].pos[j].pos.x += groundBalls[i].velocity[j].x*deltaTime;
            groundBalls[i].pos[j].pos.y += groundBalls[i].velocity[j].y*deltaTime;
            groundBalls[i].angles[j] += groundBalls[i].rotationSpeed[j]*deltaTime;
            if (groundBalls[i].pos[j].pos.z>0)
            {
                groundBalls[i].pos[j].pos.z += (groundBalls[i].velocity[j].z - 2*deltaTime)*deltaTime;
                groundBalls[i].velocity[j].z -= 4*deltaTime;
            } else
                if (groundBalls[i].pos[j].pos.z>-cellSize)
                {
                    groundBalls[i].velocity[j] *= 0.95;
                    groundBalls[i].rotationSpeed[j] *= 0.95;
                    groundBalls[i].pos[j].pos.z -= deltaTime/3;
                    if (fabsf(groundBalls[i].velocity[j].x) + fabsf(groundBalls[i].velocity[j].y)>0.1 && (rand()%10==0)) 
                        LOTrace::sharedTrace()->addRandomTrace(SunnyVector2D(groundBalls[i].pos[j].pos.x,groundBalls[i].pos[j].pos.y),0.3);
                } else 
                {
                    dest++;
                }
        }
        if (dest == stonesInGroundModelCount)
        {
            if (i!=groundBallsCount-1)
                groundBalls[i] = groundBalls[groundBallsCount-1];
            i--;
            groundBallsCount--;
            continue;
        }
    }
    
    for (short i = 0;i<snowWallCount;i++)
    {
        short dest = 0;
        for (short j = 0;j<partsInSnowWall;j++)
        {
            snowWall[i].pos[j].pos.x += snowWall[i].velocity[j].x*deltaTime;
            snowWall[i].pos[j].pos.y += snowWall[i].velocity[j].y*deltaTime;
            snowWall[i].angles[j] += snowWall[i].rotationSpeed[j]*deltaTime;
            if (snowWall[i].pos[j].pos.z>0)
            {
                snowWall[i].pos[j].pos.z += (snowWall[i].velocity[j].z - 2*deltaTime)*deltaTime;
                snowWall[i].velocity[j].z -= 4*deltaTime;
            } else
                if (snowWall[i].pos[j].pos.z>-cellSize)
                {
                    snowWall[i].velocity[j] *= 0.95;
                    snowWall[i].rotationSpeed[j] *= 0.95;
                    snowWall[i].pos[j].pos.z -= deltaTime/3;
                    if (fabsf(snowWall[i].velocity[j].x) + fabsf(snowWall[i].velocity[j].y)>0.1 && (rand()%10==0)) 
                        LOTrace::sharedTrace()->addRandomTrace(SunnyVector2D(snowWall[i].pos[j].pos.x,snowWall[i].pos[j].pos.y),0.3);
                } else 
                {
                    dest++;
                }
        }
        if (dest == partsInSnowWall)
        {
            if (i!=snowWallCount-1)
                snowWall[i] = snowWall[snowWallCount-1];
            i--;
            snowWallCount--;
            continue;
        }
    }
}

void LOCrashBalls::renderShadows()
{
    if (fireBallsCount)
    {
        for (short i = 0;i<fireBallsCount;i++)
            fireBallObj->renderShadowsByMatrices(fireBalls[i].pos);
    }
    
    if (groundBallsCount)
    {
        for (short i = 0;i<groundBallsCount;i++)
            groundBallObj->renderShadowsByMatrices(groundBalls[i].pos);
    }
    
    if (snowWallCount)
    {
        for (short i = 0;i<snowWallCount;i++)
            snowWallObj->renderShadowsByMatrices(snowWall[i].pos);
    }
}


void LOCrashBalls::render(SunnyMatrix *MV)
{
    if (fireBallsCount)
    {
        fireBallObj->prepareForRender();
        for (short i = 0;i<fireBallsCount;i++)
            fireBallObj->renderByMatrices(MV, fireBalls[i].pos,fireBalls[i].angles);
    }
    
    if (groundBallsCount)
    {
        groundBallObj->prepareForRender();
        for (short i = 0;i<groundBallsCount;i++)
            groundBallObj->renderByMatrices(MV, groundBalls[i].pos,groundBalls[i].angles);
    }
    
    if (snowWallCount)
    {
        snowWallObj->prepareForRender();
        for (short i = 0;i<snowWallCount;i++)
            snowWallObj->renderByMatrices(MV, snowWall[i].pos,snowWall[i].angles);
    }
}

void LOCrashBalls::addFireBallCrash(SunnyMatrix *pos)
{
    if (fireBallsCount>=maxCrashCount) return;
    
    fireBallObj->getMatricesForCrash(fireBalls[fireBallsCount].pos);
    float angle ;
    for (short i = 0;i<stonesInFireModelCount;i++)
    {
        fireBalls[fireBallsCount].pos[i] = fireBalls[fireBallsCount].pos[i] * *pos;
        fireBalls[fireBallsCount].rotationSpeed[i] = -360*3;
        fireBalls[fireBallsCount].angles[i] = sRandomf()*360;
        angle = sRandomf()*2*M_PI;
        fireBalls[fireBallsCount].velocity[i] = SunnyVector3D(cosf(angle)*5 + pos->up.x*3,sinf(angle)*5 + pos->up.y*3,sRandomf());
    }
    
    fireBallsCount++;
}

void LOCrashBalls::addGroundBallCrash(SunnyMatrix *pos)
{
    if (groundBallsCount>=maxCrashCount) return;
    
    groundBallObj->getMatricesForCrash(groundBalls[groundBallsCount].pos);
    float angle ;
    for (short i = 0;i<stonesInGroundModelCount;i++)
    {
        groundBalls[groundBallsCount].pos[i] = groundBalls[groundBallsCount].pos[i] * *pos;
        groundBalls[groundBallsCount].rotationSpeed[i] = -360*3;
        groundBalls[groundBallsCount].angles[i] = sRandomf()*360;
        angle = sRandomf()*2*M_PI;
        groundBalls[groundBallsCount].velocity[i] = SunnyVector3D(cosf(angle)*5 + pos->up.x*3,sinf(angle)*5 + pos->up.y*3,sRandomf());
    }
    
    groundBallsCount++;
}

void LOCrashBalls::addSnowWallCrash(SunnyMatrix *pos)
{
    if (snowWallCount>=maxCrashCount) return;
    
    snowWallObj->getMatricesForCrash(snowWall[snowWallCount].pos);
    float angle ;
    for (short i = 0;i<partsInSnowWall;i++)
    {
        snowWall[snowWallCount].pos[i] = snowWall[snowWallCount].pos[i] * *pos;
        snowWall[snowWallCount].rotationSpeed[i] = -360*3;
        snowWall[snowWallCount].angles[i] = sRandomf()*360;
        angle = sRandomf()*2*M_PI;
        snowWall[snowWallCount].velocity[i] = SunnyVector3D(cosf(angle)*5 + pos->up.x*3,sinf(angle)*5 + pos->up.y*3,sRandomf());
    }
    
    snowWallCount++;
    
    LOTrace::sharedTrace()->addSnowWallPostEffect(pos);
}