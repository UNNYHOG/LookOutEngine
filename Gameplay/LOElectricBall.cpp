//
//  LOElectricBall.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 04.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "LOElectricBall.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"

const float electricBallRemoveSize = cellSize*2;

bool LOElectricBall::update()
{
    if (detach)
        return true;
    
    if (status == LO_Respawn)
    {
        if (physicsPaused) return false;
        respawnTime -= deltaTime;
        if (respawnTime<=0)
        {
            continueAttaching();
        }
        return false;
    }
    
    position.pos.x = body->position.x;
    position.pos.y = body->position.y;
    if (position.pos.x<-electricBallRemoveSize || position.pos.y<-electricBallRemoveSize || position.pos.x>mapSize.x+electricBallRemoveSize || position.pos.y>mapSize.y+electricBallRemoveSize)
        return true;
    
    return false;
}

LOElectricBall::LOElectricBall()
{
    body->radius = 1.5*cellSize;
    body->layers = LAYER_ELECTRIC_BALL;
    body->collisionType = COLLISION_ELECTRICBALL;
}

void LOElectricBall::renderOverlay(SunnyMatrix *MV)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) * *MV;
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOOverlayElectricity_VAO);
}

void LOElectricBall::render(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    
    SunnyMatrix m = position **MV;
    m = getRotationZMatrix(sRandomf()*2*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+44+sRandomi()%6)*4, 4);
    
    const float scale = 1.0/((116.-50.)/640*mapSize.y);
    SunnyMatrix mat = getIdentityMatrix();
    for (short i = 0 ;i<attackCount;i++)
    {
        SunnyVector2D v = attack[i]->position;
        mat.pos.x = v.x;
        mat.pos.y = v.y;
        mat.up.x = (position.pos.x - mat.pos.x)*scale;
        mat.up.y = (position.pos.y - mat.pos.y)*scale;
        mat.front = mat.up ^ mat.right;
        m = mat **MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+50+sRandomi()%3)*4, 4);
    }
}

void LOElectricBall::addTarget(cpBody * target)
{
    if (attackCount>=maxLOEAttackCount) return;
    attack[attackCount] = target;
    attackCount++;
}

void LOElectricBall::removeTarget(cpBody * target)
{
    for (short i = 0;i<attackCount;i++)
        if (attack[i] == target)
        {
            if (i!=attackCount-1) attack[i] = attack[attackCount-1];
            attackCount--;
            break;
        }
}

void LOElectricBall::initWithParams(SunnyVector2D pos, SunnyVector2D vel)
{
    attackCount = 0;
    position.pos.x = pos.x;
    position.pos.y = pos.y;
    
    position.up.x = vel.x;
    position.up.y = vel.y;
    position.up.normalize();
    position.front.x = position.up.y;
    position.front.y = - position.up.x;
    velocity = vel;
    
    respawnTime = currentRespawnTime; 
    attachPhysics();
}