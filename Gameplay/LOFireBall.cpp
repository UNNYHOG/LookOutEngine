//
//  LOFireBall.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 14.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOFireBall.h"
#include "LOGlobalVar.h"
#include "LOSmoke.h"
#include "iPhone_Strings.h"
#include "LOSounds.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"

const float fireBallRemoveSize = 200./2.0/320.*mapSize.y;
const float anomaledFireBallSize = 1.5;

bool LOFireBall::update()
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
            losAddFireBallLaunchSound(body,distanceToPoint(player->getPosition()));
        }
        return false;
    }
    
    frameNum = (frameNum+1)%20;
    
    layerFrame = (layerFrame+1)%2;

    SunnyVector2D v = body->position;
    position.pos.x = v.x;
    position.pos.y = v.y;
    if (position.pos.x<-fireBallRemoveSize || position.pos.y<-fireBallRemoveSize || position.pos.x>mapSize.x+fireBallRemoveSize || position.pos.y>mapSize.y+fireBallRemoveSize)
        return true;
    
    if (!physicsPaused)
    {
        smokeDelay--;
        if (smokeDelay<=0)
        {
            smokeDelay = customSmokeDelay;
            LOSmoke::sharedSmoke()->addRandomSmoke(SunnyVector2D(position.pos.x,position.pos.y)-velocity*0.1);// + SunnyVector2D(0.5-sRandomf(),0.5-sRandomf())*2);
        }
    }
    
    if (sRandomi()%10==0)
    {
        SunnyVector2D p = SunnyVector2D(position.pos.x, position.pos.y) + SunnyVector2D(0.5-sRandomf(),0.5-sRandomf())*cellSize/2;
        activeMap->mapInfo.addSpark(p);
    }
    
    rotationAngle -= deltaTime*360;
    return false;
}

LOFireBall::LOFireBall()
{
//    anomaled = 0;
    body->collisionType = COLLISION_FIREBALL;
}

void LOFireBall::renderForMenu(SunnyMatrix *m, short frameNum)
{
    frameNum = frameNum%20;
    SHTextureBase::bindTexture(objectsTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m->front.x));
    SunnyDrawArrays(LOObject_VAO+frameNum);
    SunnyVector2D angle = activeMap->getRotationAngle();
    float scale = 1.0 - fabsf(angle.y)*0.3;
    SunnyMatrix m1 = getRotationZMatrix((angle.x*30-42)*M_PI/180)**m;
    m1 = getScaleMatrix(SunnyVector3D(scale, scale, 1))*m1;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m1.front.x));
    SunnyDrawArrays(LOObject_VAO+41+sRandomi()%2);
}

void LOFireBall::renderOverlayForMenu(SunnyMatrix *m)
{
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1,1));
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m->front.x));
    SunnyDrawArrays(LOOverlayFireball_VAO);
}

void LOFireBall::render(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    
    SunnyMatrix m = position**MV;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOObject_VAO+frameNum);
    if (anomaled)
        glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+55+frameNum%2)*4, 4);
    SunnyVector2D angle = activeMap->getRotationAngle();
    float scale = 1.0 - fabsf(angle.y)*0.3;
    float size = 18./1920*mapSize.x;
    m = getTranslateMatrix(SunnyVector3D(position.pos.x + position.up.x*size, position.pos.y + position.up.y*size, 0)) **MV;
    m = getRotationZMatrix((angle.x*30-42)*M_PI/180)*m;
    m = getScaleMatrix(SunnyVector3D(scale, scale, 1))*m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOObject_VAO+41+layerFrame);
}

void LOFireBall::renderBall(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    
    SunnyMatrix m = position * *MV;
    m = getRotationXMatrix(rotationAngle/180*M_PI) * m;
    m = getScaleMatrix(SunnyVector3D(cellSize,cellSize,cellSize)) * m;
    fireBallObj->render(&m);
}

void LOFireBall::renderOverlay(SunnyMatrix *MV)
{
    float size = mapSize.x/1920;
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x + position.up.x*size*22 + position.front.x*size*6, position.pos.y + position.up.y*size*22 + position.front.y*size*6, 0)) * *MV;
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOOverlayFireball_VAO);
}

void LOFireBall::renderLight(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    SunnyMatrix m = position **MV;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOSmoke_VAO+6);
}

void LOFireBall::applyAnomaly()
{
    anomaled = 1;
    
    velocity *= 2;
    body->velocity = velocity;
}

void LOFireBall::initWithParams(SunnyVector2D pos, SunnyVector2D vel)
{
    anomaled = 0;
    
    position.pos.x = pos.x;
    position.pos.y = pos.y;
    
    position.up.x = vel.x;
    position.up.y = vel.y;
    position.up.normalize();
    position.front.x = position.up.y;
    position.front.y = - position.up.x;
    velocity = vel;
    
    frameNum = sRandomi()%20;    
    respawnTime = currentRespawnTime; 
    attachPhysics();
    
    smokeDelay = customSmokeDelay;
    layerFrame = 0;
    
    rotationAngle = sRandomf()*360;
}
