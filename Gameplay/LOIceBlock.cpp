//
//  LOIceBlock.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 15.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOIceBlock.h"
#include "SunnyOpengl.h"
#include "iPhone_Strings.h"

#include "LOGlobalVar.h"
#include "LookOutEngine.h"
#include "LOSounds.h"
#include "LOTrace.h"

const float iceBlockSpeed = cellSize*4;

void LOIceBlock::renderShadow()
{
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(curPosition.x, curPosition.y,1,1));
    glDrawArrays(GL_TRIANGLE_STRIP, LOIceBlock_VAO*4, 4);
}

void LOIceBlock::createPhysics()
{
    body = scCreateStaticBox(SunnyVector2D(1.5*cellSize,1.5*cellSize),SunnyVector2D(0,0),false,true,true);
    body->data = this;
    body->layers = LAYER_BLADES;
    body->collisionType = COLLISION_ICE_BLOCK;
    
    insideMatrix = getIdentityMatrix();
    float x;
    x = sRandomf()*M_PI*2;insideMatrix = insideMatrix * getRotationXMatrix(x);
    x = sRandomf()*M_PI*2;insideMatrix = insideMatrix * getRotationYMatrix(x);
    x = sRandomf()*M_PI*2;insideMatrix = insideMatrix * getRotationZMatrix(x);
}

void LOIceBlock::clearPhysics()
{
    scDeactivateStaticBox(body);
    delete body;
}

void LOIceBlock::render(SunnyMatrix *MV)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(curPosition.x, curPosition.y, 0))**MV;
    iceBlockModel->renderWithMatrices(&m,&insideMatrix);
}

void renderShadow()
{
    
}

void LOIceBlock::update()
{
    if (!isMoving) return;
    
    float moveDist = iceBlockSpeed*deltaTime;
    movingDistance-=moveDist;
    if (movingDistance<0)
        moveDist += movingDistance;
    
    LOTrace::sharedTrace()->addIceTrace(curPosition, isMoving);
    
    if (isMoving == 1)
    {
        body->position.x-=moveDist;
    } else
        if (isMoving == 2)
        {
            body->position.y+=moveDist;
        } else
            if (isMoving == 3)
            {
                body->position.x+=moveDist;
            } else
            {
                body->position.y-=moveDist;
            }
    
    curPosition.x = body->position.x;
    curPosition.y = body->position.y;
    
    if (movingDistance<=0)
    {
        isMoving = 0;
        if (hasContact)
            tryToMoveFromPoint(lastContactPosition);
    }
}

void LOIceBlock::reset()
{
    body->position.x = position.x;
    body->position.y = position.y;
    isMoving = 0;
    curPosition = position;
}

void LOIceBlock::tryToMoveFromPoint(SunnyVector2D point)
{
    lastContactPosition = point;
    hasContact = true;
    if (isMoving) return;
    
    movingDistance = cellSize/2;
    SunnyVector2D nowPos = SunnyVector2D(body->position.x, body->position.y);
    SunnyVector2D dist = nowPos - point;
    if (fabsf(dist.x) > fabsf(dist.y))
    {
        if (dist.x>0)
        {
            if (!player->isHardPowerForDirection(3)) return;
            isMoving = 3;
        }
        else
        {
            if (!player->isHardPowerForDirection(1)) return;
            isMoving = 1;
        }
    } else
    {
        if (dist.y>0)
        {
            if (!player->isHardPowerForDirection(2)) return;
            isMoving = 2;
        }
        else
        {
            if (!player->isHardPowerForDirection(4)) return;
            isMoving = 4;
        }
    }
    
    if (!activeMap->canMoveIceBlock(nowPos, isMoving))
    {
        isMoving = 0;
        losPlayIceBlockAndIceBlock();
    } else
        losPlayIceBlockMoved();
}

void LOIceBlock::removeContact()
{
    hasContact = false;
}