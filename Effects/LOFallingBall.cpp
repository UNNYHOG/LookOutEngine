//
//  LOFallingBall.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 15.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOFallingBall.h"
#include "iPhone_Strings.h"
#include "SunnyVector3D.h"
#include "LOGlobalVar.h"
#include "LOTrace.h"
#include "SunnyShaders.h"

const short maxFallingBallsCount = 5;

struct LOFallingBallstruct
{
    SunnyMatrix pos;
    SunnyVector3D vel;
    float angle;
    float rotationSpeed;
    bool ignoreTrace;
} fallingFireballs[maxFallingBallsCount], fallingGroundballs[maxFallingBallsCount];

short fallingFireBallsCount = 0;
short fallingGroundBallsCount = 0;

void LOFallingBall::addFallingBallFromGroundball(SunnyMatrix *pos, SunnyVector2D vel)
{
    if (fallingGroundBallsCount >= maxFallingBallsCount) return;
    
    fallingGroundballs[fallingGroundBallsCount].pos = *pos;
    fallingGroundballs[fallingGroundBallsCount].pos.pos.z = 1.0;
    fallingGroundballs[fallingGroundBallsCount].vel = SunnyVector3D(vel.x, vel.y, 0.0);
    fallingGroundballs[fallingGroundBallsCount].angle = sRandomf()*360;
    fallingGroundballs[fallingGroundBallsCount].rotationSpeed = 360;
    fallingGroundBallsCount++;
}

void LOFallingBall::addFallingBallFromFireball(SunnyMatrix *pos, SunnyVector2D vel)
{
    if (fallingFireBallsCount >= maxFallingBallsCount) return;
    
    fallingFireballs[fallingFireBallsCount].pos = *pos;
    fallingFireballs[fallingFireBallsCount].pos.pos.z = 1.0;
    fallingFireballs[fallingFireBallsCount].vel = SunnyVector3D(vel.x, vel.y, 0.0);
    fallingFireballs[fallingFireBallsCount].angle = sRandomf()*360;
    fallingFireballs[fallingFireBallsCount].rotationSpeed = 360;
    fallingFireBallsCount++;
}

void LOFallingBall::render(SunnyMatrix *MV)
{
    SunnyMatrix m;
    for (short i = 0;i<fallingGroundBallsCount;i++)
    {
        m = fallingGroundballs[i].pos * *MV;
        m = getRotationXMatrix(fallingGroundballs[i].angle*M_PI/180) * m;
        m = getScaleMatrix(SunnyVector3D(cellSize, cellSize,cellSize))*m;
        glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
        groundBallObj->renderCenterBall();
    }
    if (fallingFireBallsCount)
    {
        fireBallObj->prepareForRender();
        for (short i = 0;i<fallingFireBallsCount;i++)
        {
            m = fallingFireballs[i].pos * *MV;
            m = getRotationXMatrix(fallingFireballs[i].angle*M_PI/180) * m;
            m = getScaleMatrix(SunnyVector3D(cellSize, cellSize, cellSize)) * m;
            fireBallObj->renderOriginal(&m);
        }
    }
}

void LOFallingBall::renderShadows(SunnyMatrix *MV)
{
    if (fallingGroundBallsCount || fallingFireBallsCount)
    {
        SunnyMatrix m;
        for (short i = 0;i<fallingGroundBallsCount;i++)
        if (fallingGroundballs[i].pos.pos.z<=0)
        {
            m = fallingGroundballs[i].pos * *MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, cellSize/3, 1, 1));
            glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1 - fabsf(1+fallingGroundballs[i].pos.pos.z*1.4)));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO+6)*4, 4);
        }
        
        for (short i = 0;i<fallingFireBallsCount;i++)
        if (fallingFireballs[i].pos.pos.z<=0)
        {
            m = fallingFireballs[i].pos * *MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, cellSize/4, 0.8, 0.8));
            glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1 - fabsf(1+fallingFireballs[i].pos.pos.z*1.3)));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO+6)*4, 4);
        }
        
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(MV->front.x));

        for (short i = 0;i<fallingGroundBallsCount;i++)
        {
            if (fallingGroundballs[i].pos.pos.z<=0)
            {
                float scale = 1.0 + fallingGroundballs[i].pos.pos.z*0.1;
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(fallingGroundballs[i].pos.pos.x - lightPosition.x*(fallingGroundballs[i].pos.pos.z/4 + 0.4),
                                                             fallingGroundballs[i].pos.pos.y - lightPosition.y*(fallingGroundballs[i].pos.pos.z/4 + 0.4), scale, scale));
                glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1.0+fallingGroundballs[i].pos.pos.z/2));
            }
            else
            {
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(fallingGroundballs[i].pos.pos.x - lightPosition.x*(fallingGroundballs[i].pos.pos.z+0.4),
                                                             fallingGroundballs[i].pos.pos.y - lightPosition.y*(fallingGroundballs[i].pos.pos.z+0.4), 1, 1));
                glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1.0));
            }
            glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO)*4, 4);
        }
        
        for (short i = 0;i<fallingFireBallsCount;i++)
        {
            if (fallingFireballs[i].pos.pos.z<=0)
            {
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(fallingFireballs[i].pos.pos.x - lightPosition.x*0.2, fallingFireballs[i].pos.pos.y - lightPosition.y*0.2, cellSize*0.6, cellSize*0.6));
                glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1.0+fallingFireballs[i].pos.pos.z/2));
            }
            else
            {
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(fallingFireballs[i].pos.pos.x - lightPosition.x*(fallingFireballs[i].pos.pos.z+0.1),
                                                             fallingFireballs[i].pos.pos.y - lightPosition.y*(fallingFireballs[i].pos.pos.z+0.1), cellSize*0.6, cellSize*0.6));
                glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1.0));
            }
            glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO)*4, 4);
        }
    }
}

void LOFallingBall::update()
{
    for (short i = 0;i<fallingGroundBallsCount;i++)
    {
        fallingGroundballs[i].angle -= deltaTime*fallingGroundballs[i].rotationSpeed;
        fallingGroundballs[i].pos.pos.x += fallingGroundballs[i].vel.x*deltaTime;
        fallingGroundballs[i].pos.pos.y += fallingGroundballs[i].vel.y*deltaTime;
        if (fallingGroundballs[i].pos.pos.z>0)
        {
            fallingGroundballs[i].pos.pos.z += (fallingGroundballs[i].vel.z - 2*deltaTime)*deltaTime;
            fallingGroundballs[i].vel.z -= 4*deltaTime;
        } else
            if (fallingGroundballs[i].pos.pos.z>-cellSize*1.5)
            {
                fallingGroundballs[i].vel *= 0.95;
                fallingGroundballs[i].rotationSpeed *= 0.95;
                fallingGroundballs[i].pos.pos.z -= deltaTime/2;
                fallingGroundballs[i].ignoreTrace = !fallingGroundballs[i].ignoreTrace;
                if (!fallingGroundballs[i].ignoreTrace)
                    LOTrace::sharedTrace()->addRandomTrace(SunnyVector2D(fallingGroundballs[i].pos.pos.x,fallingGroundballs[i].pos.pos.y),0.8);
            } else 
            {
                if (i!=fallingGroundBallsCount-1)
                    fallingGroundballs[i] = fallingGroundballs[fallingGroundBallsCount-1];
                i--;
                fallingGroundBallsCount--;
                continue;
            }
    }
    
    for (short i = 0;i<fallingFireBallsCount;i++)
    {
        fallingFireballs[i].angle -= deltaTime*fallingFireballs[i].rotationSpeed;
        fallingFireballs[i].pos.pos.x += fallingFireballs[i].vel.x*deltaTime;
        fallingFireballs[i].pos.pos.y += fallingFireballs[i].vel.y*deltaTime;
        if (fallingFireballs[i].pos.pos.z>0)
        {
            fallingFireballs[i].pos.pos.z += (fallingFireballs[i].vel.z - 2*deltaTime)*deltaTime;
            fallingFireballs[i].vel.z -= 4*deltaTime;
        } else
        if (fallingFireballs[i].pos.pos.z>-cellSize*1.5)
        {
            fallingFireballs[i].vel *= 0.95;
            fallingFireballs[i].rotationSpeed *= 0.95;
            fallingFireballs[i].pos.pos.z -= deltaTime/2;
            fallingFireballs[i].ignoreTrace = !fallingFireballs[i].ignoreTrace;
            if (!fallingFireballs[i].ignoreTrace)
                LOTrace::sharedTrace()->addRandomTrace(SunnyVector2D(fallingFireballs[i].pos.pos.x,fallingFireballs[i].pos.pos.y),0.8);
        } else 
        {
            if (i!=fallingFireBallsCount-1)
                fallingFireballs[i] = fallingFireballs[fallingFireBallsCount-1];
            i--;
            fallingFireBallsCount--;
            continue;
        }
    }
}

void LOFallingBall::reset()
{
    fallingFireBallsCount = 0;
    fallingGroundBallsCount = 0;
}