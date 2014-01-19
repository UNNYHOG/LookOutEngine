//
//  LOObject.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "LOObject.h"
#include "SunnyOpenGl.h"
#include "LOGlobalVar.h"
#include "LookOutEngine.h"

float currentRespawnTime = customRespawnTime;

//static GLuint loObjectVAO = 0;
//static GLuint loObjectVBO = 0;

LOObject::LOObject()
{
    init();
}

LOObject::~LOObject()
{
    clear();
}

void LOObject::init()
{
    position = getIdentityMatrix();
    position.pos = SunnyVector4D(mapSize.x/2,mapSize.y/2,0,1);
    velocity = SunnyVector2D(0,0);
    frameNum = 0;
    
    body = scCreateCircle(cellSize/2);
    body->layers = LAYER_HIGH | LAYER_ELECTRIC | LAYER_ELECTRIC_BALL;
    
    detach = false;
    respawnTime = 0;
}

void LOObject::setPosition(SunnyVector2D pos)
{
    body->position = pos;
    position.pos.x = pos.x;
    position.pos.y = pos.y;
}

void LOObject::setPosition(SunnyMatrix mat)
{
    position = mat;
    body->position = SunnyVector2D(mat.pos.x, mat.pos.y);
}

void LOObject::continueAttaching()
{
    scActivateCircle(body);
    status = LO_Active;
}

void LOObject::attachPhysics()
{
    detach = false;
    body->position = getPosition();
    body->velocity = velocity;
    if (respawnTime<=0)
    {
        continueAttaching();
    } else
        status = LO_Respawn;
}

void LOObject::detachPhysics()
{
    if (status == LO_Respawn) return;
    scDeactivateCircle(body);
}

void LOObject::clear()
{
    scDeactivateCircle(body);
    delete body;
    body = 0;
}

void LOObject::initWithParams(SunnyVector2D pos, SunnyVector2D vel)
{
    init();
}

void LOObject::exchange(LOObject * object)
{
    SC_Circle * bufBody = body;
    body = object->body;
    object->body = bufBody;
    
    position = object->position;
    velocity = object->velocity;
    detach = object->detach;
    
    respawnTime = object->respawnTime;
    status = object->status;
    
    skipFrame = object->skipFrame;
}

void LOObject::renderBottomLayer()
{
    if (status != LO_Active) return;
    glPushMatrix();
    glMultMatrixf(&position[0][0]);
    //glTexCoordPointer(2, GL_FLOAT, 0, layerTexCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
}

void LOObject::renderTopLayerWithRotation()
{
    if (status != LO_Active) return;
    glPushMatrix();
    glTranslatef(position.pos.x, position.pos.y, 0);
    SunnyVector2D angle = activeMap->getRotationAngle();
    glRotatef(angle.x*30, 0, 0, 1);
    float scale = 1.0 - fabsf(angle.y)*0.3;
    glScalef(scale, scale, 1.0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
}

void LOObject::render()
{
    //glColor4f(1, 1, 1, 1);
    if (status != LO_Active) return;
    
    glPushMatrix();
    glMultMatrixf(&position[0][0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
}

void LOObject::renderPhysics()
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(1, 0, 0, 0.3);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, &position.pos.x);
    glDrawArrays(GL_POINTS, 0, 1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_TEXTURE_2D);
}

void LOObject::setVelocity(SunnyVector2D vel)
{
    velocity = vel;
    body->velocity = vel;
}

SunnyVector2D LOObject::getPosition()
{
    return SunnyVector2D(position.pos.x,position.pos.y);
}

SunnyVector2D LOObject::getVelocity()
{
    return velocity;
}

bool LOObject::update()
{
    return false;
}

bool LOObject::updatePosition()
{
    return false;
}

float LOObject::distanceToPoint(SunnyVector2D p)
{
    return (p - SunnyVector2D(position.pos.x,position.pos.y)).length();
}

float LOObject::distanceSqToPoint(SunnyVector2D p)
{
    return (p - SunnyVector2D(position.pos.x,position.pos.y)).lengthSq();
}
