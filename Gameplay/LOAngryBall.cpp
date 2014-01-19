//
//  LOAngryBall.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 30.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOAngryBall.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LOTrace.h"
#include "LOSounds.h"
#include "LOEvents.h"

extern SunnyVector3D **sharedPowerField;
extern SunnyVector2D playerPhysicsPoints[];

LOAngryBall::LOAngryBall()
{
}

LOAngryBall::~LOAngryBall()
{
}

void LOAngryBall::attachMoveBody()
{
}

void LOAngryBall::setLayers()
{
    if (isMultiPlayer && myMultiPlayerNumber != multiplayerTargetNum)
    {
        body->layers = LAYER_LOW;
    } else
    {
        body->layers = LAYER_HIGH | LAYER_LOW | LAYER_BLADES | LAYER_ELECTRIC | LAYER_ELECTRIC_BALL;
    }
}

void LOAngryBall::setAnomaly()
{
    velocityScale *= 2;
}

bool LOAngryBall::update()
{
    if (detach)
        return true;
    
    state = (state+1)%30;
    
    if (status == LO_Respawn)
    {
        if (physicsPaused) return false;
        respawnTime -= deltaTime;
        if (respawnTime<=0)
            continueAttaching();
        return false;
    }
    
    if (turnOffTime>0)
    {
        turnOffTime-=deltaTime;
        velocity = SunnyVector2D(0,0);
        return false;
    }
    
    if (chargeInfo.charged)
    {
        chargeInfo.chargedTime+=deltaTime;
        if (chargeInfo.chargedTime>2.0)
        {
            if (chargeInfo.charged==1)
            {
                chargeInfo.chargedPos = player->getPosition() + player->getVelocity() - getPosition();
                chargeInfo.chargedPos.normalize();
                chargeInfo.charged = 2;
                ignoreSnow = -1;
                losPlayAngryPreRush(distanceToPoint(player->getPosition()));
            } else
            {
                SunnyVector2D v = chargeInfo.chargedPos*velocityScale*10;
//#ifndef SUNNY_PHYSICS
//                cpPivotJointSetAnchr2(joint, cpvadd(body->p,cpv(v.x,v.y)));
                jointVelocity = v;
//#endif
                if (chargeInfo.chargedTime>=4)
                {
                    detach = true;
                    SunnyMatrix mat = getIdentityMatrix();
                    mat.pos = position.pos;
                    activeMap->explosion(&mat, LOExplosion_Fire);
                    losPlayFFExplosion((mat.pos-player->position.pos).length());
                    activeMap->explodeAngryballAtLocation(&mat);
                } else
                {
                    ignoreSnow = (ignoreSnow+1)%5;
                    if (!ignoreSnow)
                    {
                        v *= 0.15;
                        SunnyVector2D nv = SunnyVector2D(v.y,-v.x);
                        SunnyVector2D nv2 = nv + v;
                        activeMap->addSnowFromAngryBall(getPosition() + nv/5, nv2);
                        nv2 = -nv + v;
                        activeMap->addSnowFromAngryBall(getPosition() - nv/5, nv2);
                    }
                }
            }
        } else
        {
//#ifndef SUNNY_PHYSICS
            SunnyVector2D v = chargeInfo.chargedPos + SunnyVector2D(1 - sRandomf()*2 - body->position.x,1 - sRandomf()*2 - body->position.y);
//            cpPivotJointSetAnchr2(joint, cpvadd(body->p,cpv(v.x,v.y)));
            jointVelocity = v;
//#endif
        }
        
        chargeInfo.chargedDelay++;
        if (chargeInfo.chargedDelay>=chargeInfo.chargedMaxDelay) chargeInfo.chargedDelay = 0;
        if (chargeInfo.chargedDelay==0)
        {
            chargeInfo.chargedFrame = (chargeInfo.chargedFrame+1)%3;
            chargeInfo.chargedMaxDelay-=0.3;
        }
    } else
    {
        if (isMultiPlayer && myMultiPlayerNumber!=multiplayerTargetNum)
        {
            
        } else
        {    
            if (!prediction)
            {
                prediction = !(sRandomi()%10);
                if (prediction)
                {
                    predictionTime = sRandomf()*2;
                    stopPrediction = sRandomf()*1;
                }
            }
            
            SunnyVector2D v = player->getPosition();
            if (prediction)
            {
                v += player->getVelocity()*prediction;
                stopPrediction-=deltaTime;
                if (stopPrediction<0) prediction = false;
            }
            v = v - SunnyVector2D(position.pos.x,position.pos.y);
            v = v.normalize()*velocityScale + velocityAdd;
            velocityAdd /= 2;
            jointVelocity = v;
        }
    }
//#ifndef SUNNY_PHYSICS
//    cpPivotJointSetAnchr2(joint, cpvadd(body->p,cpv(jointVelocity.x,jointVelocity.y)));
//#endif
    velocity = jointVelocity*3;
    return false;
}

bool LOAngryBall::updatePosition()
{
//#ifndef SUNNY_PHYSICS
    const float radius = cellSize/2;
    
    const float onePartSize = cellSize/4/4;
    float dist = velocity.length()*deltaTime;
    SunnyVector2D pos = getPosition();
    SunnyVector2D realPos = pos;
    SunnyVector2D v = velocity*deltaTime;
    for (int j = 0;j<6;j++)
    {
        if (v.length()<0.01)
            break;
        
        //bool q = true;
        for (short i = 0;i<playerPhysicsPointsCount;i++)
        {
            SunnyVector2D pp = realPos + playerPhysicsPoints[i]*radius;
            
            short x = pp.x*forceFieldSizeX/mapSize.x;
            short y = forceFieldSizeY - pp.y*forceFieldSizeY/mapSize.y - 1;
            
            if (x>=0 && y>=0 && x<forceFieldSizeX && y<forceFieldSizeY)
            {
                SunnyVector3D force = sharedPowerField[x][y];
                //printf("\n (%d,%d) > force = (%f,%f,%f)",x,y,force.x,force.y,force.z);
                if (force.z>4)
                {
                    float dot = v.x*force.x + v.y*force.y;
                    if (dot<0)
                    {
                        v.x -= dot*force.x;
                        v.y -= dot*force.y;
                        //q = false;
                    }
                } else
                {
                    //v.x += force.x*force.z/8;
                    //v.y += force.y*force.z/8;
                }
            }
        }
        //if (q) realPos = pos;
        
        float len = v.length();
        if (len<0.01)
            break;
        else
        {
            dist = fminf(onePartSize,len);
            realPos = realPos + v*dist/len;
            v -= v*dist/len;
        }
    }
    
    v = realPos;
    
    if (v.x<radius)
        v.x = radius;
    else
        if (v.x>mapSize.x-radius)
            v.x = mapSize.x-radius;
    if (v.y<radius)
        v.y = radius;
    else
        if (v.y>mapSize.y-radius)
            v.y = mapSize.y-radius;
    body->position = v;
    
//    SunnyVector2D v = body->position;
    if (chargeInfo.charged!=2)
    {
        SunnyVector3D rotVec = SunnyVector3D(v.x - position.pos.x, v.y - position.pos.y, 0) ^ SunnyVector3D(0,0,-1);
        rotVec = -rotVec/radius;
        float sinTetaAngle = sin(rotVec.x);
        float cosTetaAngle = cos(rotVec.x);
        float sinPhiAngle = sin(-rotVec.y);
        float cosPhiAngle = cos(-rotVec.y);
        
        position = position * SunnyMatrix(SunnyVector4D(1,0,0,0),SunnyVector4D(0,cosTetaAngle,-sinTetaAngle,0),SunnyVector4D(0,sinTetaAngle,cosTetaAngle,0),SunnyVector4D(0,0,0,1));
        position = position * SunnyMatrix(SunnyVector4D(cosPhiAngle,0,-sinPhiAngle,0),SunnyVector4D(0,1,0,0),SunnyVector4D(sinPhiAngle,0,cosPhiAngle,0),SunnyVector4D(0,0,0,1));
    }
    
    position.pos.x = v.x;
    position.pos.y = v.y;
    position.pos.z = radius;
    
    ignoreTrace = (ignoreTrace+1)%3;
    if (ignoreTrace==0 || chargeInfo.charged==2)
        LOTrace::sharedTrace()->addRandomTrace(SunnyVector2D(position.pos.x,position.pos.y),1.1);
//#endif
    return false;
}

void LOAngryBall::initWithParams(SunnyVector2D pos, short targetNum)
{
    position = getIdentityMatrix();
    position.pos.x = pos.x;
    position.pos.y = pos.y;
    velocity.x = velocity.y = 0;
    
    body->collisionType = COLLISION_ANGRYBALL;
    body->velocity = SunnyVector2D(0,0);
    
    respawnTime = currentRespawnTime*6 + 1;
    attachPhysics();
    velocityAdd = SunnyVector2D(0,0);
    state = sRandomi()%30;
    prediction = false;
    
    velocityScale = 0.5+sRandomf()*0.5;
    ignoreTrace = 0;
    turnOffTime = 0;
    chargeInfo.charged = 0;
    chargeInfo.chargedFrame = -1;
    multiplayerTargetNum = targetNum;
    setLayers();
}

void LOAngryBall::initBufWithParams(LOBufAngryBall pos)
{
    position = getIdentityMatrix();
    position.pos.x = pos.position.x;
    position.pos.y = pos.position.y;
    velocity.x = velocity.y = 0;
    type = pos.type;
    body->collisionType = COLLISION_ANGRYBALL;
    body->velocity = SunnyVector2D(0,0);
    
    respawnTime = 0;
    //respawnTime = currentRespawnTime*6 + 1;
    attachPhysics();
    
    velocityAdd = SunnyVector2D(0,0);
    state = sRandomi()%30;
    prediction = false;
    
    velocityScale = 0.5+sRandomf()*0.5;
    ignoreTrace = 0;
    turnOffTime = 0;
    chargeInfo.charged = 0;
    chargeInfo.chargedFrame = -1;
    multiplayerTargetNum = 0;
    setLayers();
}

void LOAngryBall::exchange(LOAngryBall * object)
{
    cpBody * bufBody = body;
    body = object->body;
    object->body = bufBody;
    
    position = object->position;
    velocity = object->velocity;
    detach = object->detach;
    
    respawnTime = object->respawnTime;
    status = object->status;
    state = object->state;
    turnOffTime = object->turnOffTime;
    type = object->type;
    chargeInfo.charged = object->chargeInfo.charged;
}

void LOAngryBall::renderLight(SunnyMatrix *MV)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) * *MV;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    if (turnOffTime>0)
    {
        if (rand()%3) return;
        m = getRotationZMatrix(sRandomf()*2*M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform1f(uniformTSA_A, 1);
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+10+sRandomi()%3)*4, 4);
        return;
    }
    
    if (state<5)
    {
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (respawnTime>currentRespawnTime*6)
        {
            glUniform1f(uniformTSA_A, currentRespawnTime*6+1-respawnTime);
            glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+16)*4, 4);
            glUniform1f(uniformTSA_A, 1);
        }
        else
            glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+9)*4, 4);
    }
    
    if (respawnTime>0 && respawnTime<currentRespawnTime*6)
    {
        chargeInfo.chargedFrame++;
        if (chargeInfo.chargedFrame==0)
        {
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+17)*4, 4);
        } else
        if (chargeInfo.chargedFrame==1)
        {
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 2, 2));
            glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+17)*4, 4);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        } else
        if (chargeInfo.chargedFrame==2)
        {
            SunnyMatrix m = getIdentityMatrix();
            m.pos = position.pos;
            LOTrace::sharedTrace()->addAngryBallRespTrace(&m);
            chargeInfo.chargedFrame = 11;
            losAddAngryBallDigSound(body, distanceToPoint(player->getPosition()));
        } else
        {
            if (chargeInfo.chargedFrame%10==0)
            {
                activeMap->addSnowFromAngryBall(getPosition(), 2*SunnyVector2D(1 - 2*sRandomf(),1 - 2*sRandomf()));
            }
        }
    }
    
    if (chargeInfo.charged && chargeInfo.chargedDelay==0)
    {
        m = getRotationZMatrix(sRandomf()*2*M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+13+chargeInfo.chargedFrame)*4, 4);
    }
}

void LOAngryBall::chargeTheBall()
{
    if (chargeInfo.charged) return;

    LOEvents::OnAngryBallCollide(LOD_ElectricBall);
    chargeInfo.chargedPos = body->position;
    chargeInfo.charged = 1;
    chargeInfo.chargedTime = 0;
    chargeInfo.chargedMaxDelay = 5;
    chargeInfo.chargedDelay = -1;
    chargeInfo.chargedFrame = -1;
    
    losAddAngryBallLaunchSound(body,distanceToPoint(player->getPosition()));
}

void LOAngryBall::render(SunnyMatrix *MV)
{
    SunnyMatrix m = position **MV;
    if (status != LO_Active)
    {
        m = getTranslateMatrix(SunnyVector3D(0,0,-respawnTime/2)) * m;
        m = getRotationZMatrix(sinf(realPlayingTime*10+state)*30*M_PI/180) * m;
    }
    m = getScaleMatrix(SunnyVector3D(cellSize,cellSize,cellSize)) * m;
    
    glUniformMatrix4fv(globalModelview[LOS_Textured3D], 1, GL_FALSE, &(m.front.x));
    angryBallObj->renderObject(0);
    if ((state>=5 || turnOffTime>0) && !chargeInfo.charged)
        angryBallObj->renderObject(1);
}

