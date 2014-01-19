//
//  LOFireBall.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 14.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOAnomalyBall.h"
#include "LOGlobalVar.h"
#include "LOSmoke.h"
#include "iPhone_Strings.h"
#include "LOSounds.h"

const float anomalyBallRemoveSize = mapSize.y/6;

bool LOAnomalyBall::update()
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
    
    frameNum = (frameNum+1)%41;
    
    SunnyVector2D v = body->position;
    position.pos.x = v.x;
    position.pos.y = v.y;
    if (position.pos.x<-anomalyBallRemoveSize || position.pos.y<-anomalyBallRemoveSize || position.pos.x>mapSize.x+anomalyBallRemoveSize || position.pos.y>mapSize.y+anomalyBallRemoveSize)
        return true;
    
    if (!physicsPaused)
    {
        LOSmoke::sharedSmoke()->addAnomalySmoke(SunnyVector2D(position.pos.x,position.pos.y));
        LOSmoke::sharedSmoke()->addAnomalySmoke(SunnyVector2D(position.pos.x,position.pos.y));
        if (smokeDelay)
            LOSmoke::sharedSmoke()->addAnomalySmoke(SunnyVector2D(position.pos.x,position.pos.y));
    }
    
    rotationAngle -= deltaTime*90;
    return false;
}

void LOAnomalyBall::render(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    
    const float scale = 0.8;
    
    static const float period = 1;
    
    float pt = realPlayingTime;
    int time = pt/period;
    float t = pt - time*period;
    
    SunnyMatrix m1 = position **MV;
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,scale,scale));
    
    //glUniform1f(uniformTSA_A, (period-t)/period);
    glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,(period-t)/period));
    SunnyMatrix m = getRotationZMatrix(rotationAngle*M_PI/180) * m1;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+54)*4, 4);

    pt+=0.3*period;
    time = pt/period;
    t = pt - time*period;
    m1 = getTranslateMatrix(SunnyVector3D(0, -cellSize/2, 0)) * m1;
//    glUniform1f(uniformTSA_A, (period-t)/period*0.7);
    glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, (period-t)/period*0.7));
    
    m = getRotationZMatrix(-1.5*rotationAngle*M_PI/180) * m1;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+54)*4, 4);
    
    pt+=0.3*period;
    time = pt/period;
    t = pt - time*period;
    m1 = getTranslateMatrix(SunnyVector3D(0, -cellSize/3, 0)) * m1;
    //glUniform1f(uniformTSA_A, (period-t)/period*0.4);
    glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,(period-t)/period*0.4));
    m = getRotationZMatrix(2*rotationAngle*M_PI/180) * m1;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+54)*4, 4);
    
    //glUniform1f(uniformTSA_A, 1);
}

void LOAnomalyBall::renderElectricity()
{
//    short time = sRandomi()%6;
//    if (time<=2)
//    {
//        glPushMatrix();
//        glTranslatef(position.pos.x, position.pos.y, 0);
//        glRotatef(360*sRandomf(), 0, 0, 1);
//        glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+57+time)*4, 4);
//        glPopMatrix();
//    }
}

void LOAnomalyBall::initWithParams(SunnyVector2D pos, SunnyVector2D vel)
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
    body->collisionType = COLLISION_ANOMALYBALL;
    respawnTime = currentRespawnTime; 
    attachPhysics();
    
//    smokeDelay = customSmokeDelay/2;
//    layerFrame = 0;
    if (vel.length()>1.25) smokeDelay = 1;
    else smokeDelay = 0;
    
    rotationAngle = sRandomf()*360;
}
