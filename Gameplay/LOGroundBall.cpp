//
//  LOGroundBall.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 15.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOGroundBall.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOTrace.h"
#include "SunnyShaders.h"

const float groundBallRemoveSize = cellSize*2;
const float customGroundPartLifeLime = 1;

bool LOGroundBall::update()
{
    if (detach)
        return true;
    
    if (status == LO_Respawn)
    {
        if (physicsPaused) return false;
        respawnTime -= deltaTime;
        if (respawnTime<=0)
            continueAttaching();
        return false;
    }
    
    SunnyVector2D v = body->position;
    position.pos.x = v.x;
    position.pos.y = v.y;
    if (position.pos.x<-groundBallRemoveSize || position.pos.y<-groundBallRemoveSize || position.pos.x>mapSize.x+groundBallRemoveSize || position.pos.y>mapSize.y+groundBallRemoveSize)
        return true;
    
    for (short i = 0;i<stonesInGroundModelCount;i++)
        angles[i] += anglesV[i]*deltaTime;
    
    centerAngle -= M_PI*1.5*deltaTime;
    
    if (!physicsPaused)
    {
        smokeDelay--;
        if (smokeDelay<=0)
        {
            smokeDelay = customSmokeDelay;
            LOSmoke::sharedSmoke()->addGroundSmoke(SunnyVector2D(position.pos.x,position.pos.y));// + SunnyVector2D(0.5-sRandomf(),0.5-sRandomf())*2);
        }
        
        for (short i = 0 ;i<smallPartsCount;i++)
        {
            groundParts[i].pos.w -= deltaTime*customGroundPartLifeLime;
            if (groundParts[i].pos.w<=0)
            {
                groundParts[i].pos.x = position.pos.x + (0.5 - sRandomf());
                groundParts[i].pos.y = position.pos.y + (0.5 - sRandomf());
                groundParts[i].vel = velocity*(1+sRandomf())/4;
                groundParts[i].pos.w = 1;
                float size;
                if (sRandomi()%2==0)
                    size = 1.0 + sRandomf()*2;
                else
                    size = 1.0 + sRandomf()*3;
                groundParts[i].pos.z = size*pointsCoeff;
            } else {                
                groundParts[i].pos.x += groundParts[i].vel.x*deltaTime;
                groundParts[i].pos.y += groundParts[i].vel.y*deltaTime;
            }
        }
    }
    return false;
}

void LOGroundBall::renderShadow(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x - lightPosition.x*(0.5/lightPosition.z), position.pos.y - lightPosition.y*(0.5/lightPosition.z), 0));
    m = m* *MV;
    m = getRotationZMatrix(angles[0].x/2) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    
    glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO+2)*4, 4);
}

void LOGroundBall::renderParts()
{
    if (status != LO_Active) return;
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(LOOneGroundPart), &groundParts[0].pos.x);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_POINTS, 0, smallPartsCount);
}

void LOGroundBall::render(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    
    SunnyMatrix m = position **MV;
    m = getScaleMatrix(SunnyVector3D(cellSize,cellSize,cellSize)) * m;
    groundBallObj->render(&m, centerAngle,angles);
}

void LOGroundBall::renderForMenu(SunnyMatrix *m,float centerAngle, SunnyVector2D*angles)
{
    sunnyUseShader(globalShaders[LOS_Textured3D]);
    glUniform4fv(uniform3D_C, 1, SunnyVector4D(1, 1, 1, 1.0));
    groundBallObj->prepareForRender();
    groundBallObj->render(m, centerAngle,angles);
}

void LOGroundBall::initWithParams(SunnyVector2D pos, SunnyVector2D vel)
{
    position.pos.x = pos.x;
    position.pos.y = pos.y;
    
    position.up.x = vel.x;
    position.up.y = vel.y;
    position.up.normalize();
    position.front.x = position.up.y;
    position.front.y = - position.up.x;
    velocity = vel;
    
    frameNum = 0;
    body->collisionType = COLLISION_GROUNDBALL;
    
    respawnTime = currentRespawnTime;
    attachPhysics();
    
    for (short i =0;i<stonesInGroundModelCount;i++)
    {
        angles[i] = SunnyVector2D(sRandomf()*2,sRandomf())*M_PI;
        anglesV[i] = SunnyVector2D(0.5 - sRandomf(),0.5 - sRandomf())*10;
    }
    centerAngle = sRandomf()*2*M_PI;
    smokeDelay = customSmokeDelay;
    
    for (short i = 0 ;i<smallPartsCount;i++)
    {
        groundParts[i].pos.x = pos.x + (0.5 - sRandomf());
        groundParts[i].pos.y = pos.y + (0.5 - sRandomf());
        groundParts[i].vel = vel*(1+sRandomf())/4;
        groundParts[i].pos.w = sRandomf();
        float size;
        if (sRandomi()%2==0)
            size = 1.0 + sRandomf()*3;
        else
            size = 1.0 + sRandomf()*5;
        groundParts[i].pos.z = size*pointsCoeff;
    }
}
