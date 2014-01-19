//
//  LOWaterBall.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 14.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOWaterBall.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "iPhone_Strings.h"
#include "SunnyShaders.h"

SunnyVector3D externWaterBallsPositions[maxWaterBalls];

const float waterBallRemoveSize = cellSize*2;

bool LOWaterBall::update()
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
    
    frameNum = (frameNum+1)%18;

    SunnyVector2D v = body->position;
    position.pos.x = v.x;
    position.pos.y = v.y;
    if (position.pos.x<-waterBallRemoveSize || position.pos.y<-waterBallRemoveSize || position.pos.x>mapSize.x+waterBallRemoveSize || position.pos.y>mapSize.y+waterBallRemoveSize)
        return true;
    return false;
}

LOWaterBall::LOWaterBall()
{
    anomaled = 0;
}

void LOWaterBall::renderOverlay(SunnyMatrix *MV)
{
    if (!skipFrame) return;
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) * *MV;
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
}

void LOWaterBall::setPositionForNum(short num)
{
    externWaterBallsPositions[num] = SunnyVector3D(position.pos.x,position.pos.y,anomaled);
}

void LOWaterBall::renderForMenu(SunnyMatrix *m)
{
    const float rx = mapSize.x/mapScale;
    const float ry = mapSize.y/mapScale;
    const float sx = screenSize.x/rx/texturebufferWidth;
    const float sy = screenSize.y/ry/texturebufferHeight;
    
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedA_Tex]);
    glUniform1f(uniformA_Tex_A, 1);
    
    glDisable(GL_BLEND);
    SHTextureBase::resetActiveTexture();
    glBindTexture(GL_TEXTURE_2D, textureForRender);
    {
        glUniformMatrix4fv(globalModelview[LOS_TexturedA_Tex], 1, GL_FALSE, &(m->front.x));
        glUniform2fv(uniformA_Tex_Tex, 1, SunnyVector2D((m->pos.x)*sx, (m->pos.y)*sy));
        glDrawArrays(GL_TRIANGLE_FAN, LOWaterBall_VAO*4, 22);
    }
    
    SHTextureBase::bindTexture(objectsTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    
    glEnable(GL_BLEND);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    glUniform1f(uniformTSA_A, 1);
    {
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m->front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+43)*4, 4);
    }
}

void LOWaterBall::render(SunnyMatrix *MV, short waterBallsCount)
{
    const float rx = mapSize.x/mapScale;
    const float ry = mapSize.y/mapScale;
    const float sx = screenSize.x/rx/texturebufferWidth;
    const float sy = screenSize.y/ry/texturebufferHeight;
    
    sunnyUseShader(globalShaders[LOS_TexturedA_Tex]);
    glUniform1f(uniformA_Tex_A, 1);
    
    SHTextureBase::resetActiveTexture();
    glBindTexture(GL_TEXTURE_2D, textureForRender);
    SunnyMatrix m;
    for (short i = 0;i<waterBallsCount;i++)
    {
        m = getTranslateMatrix(SunnyVector3D(externWaterBallsPositions[i].x, externWaterBallsPositions[i].y, 0)) * *MV;
        
        glUniformMatrix4fv(globalModelview[LOS_TexturedA_Tex], 1, GL_FALSE, &(m.front.x));
        glUniform2fv(uniformA_Tex_Tex, 1, SunnyVector2D((externWaterBallsPositions[i].x)*sx, (externWaterBallsPositions[i].y)*sy));
        glDrawArrays(GL_TRIANGLE_FAN, LOWaterBall_VAO*4, 22);
    }
    
    SHTextureBase::bindTexture(objectsTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    
    glEnable(GL_BLEND);
    bindGameGlobal();
    for (short i = 0;i<waterBallsCount;i++)
    {
        m = getTranslateMatrix(SunnyVector3D(externWaterBallsPositions[i].x, externWaterBallsPositions[i].y, 0)) * *MV;
        
        if (externWaterBallsPositions[i].z>0.5)
        {
            m = getRotationZMatrix(sin(realPlayingTime)*M_PI/180) * m;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+14)*4, 4);
            glUniform1f(uniformTSA_A, 0.7);
            m = getRotationZMatrix(sin(1.2*realPlayingTime)*M_PI/180) * m;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+15)*4, 4);
            glUniform1f(uniformTSA_A, 1);
        } else
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+43)*4, 4);
    }
}

void LOWaterBall::render(SunnyMatrix *MV)
{
    if (status != LO_Active) return;
    
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) **MV;
    
    if (anomaled)
    {
        m = getRotationZMatrix(sin(realPlayingTime)*M_PI/180) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+14)*4, 4);
        
        glUniform1f(uniformTSA_A, 0.7);
        m = getRotationZMatrix(sin(1.2*realPlayingTime)*M_PI/180) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));

        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+15)*4, 4);
        glUniform1f(uniformTSA_A, 1);
    } else
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOWaterBall_VAO+frameNum)*4, 4);
}

void LOWaterBall::renderCharge(SunnyMatrix *MV)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) **MV;
    m = getRotationZMatrix(M_PI*2*sRandomf()) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+10+sRandomi()%3)*4, 4);
}

void LOWaterBall::renderShadow()
{
    if (status != LO_Active) return;
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(position.pos.x - lightPosition.x*(0.5/lightPosition.z), position.pos.y - lightPosition.y*(0.5/lightPosition.z), 1,1));
    glDrawArrays(GL_TRIANGLE_STRIP, (LOShadows_VAO+3)*4, 4);
}

void LOWaterBall::initWithParams(SunnyVector2D pos, SunnyVector2D vel)
{
    anomaled = 0;
    
    skipFrame = false;
    position.pos.x = pos.x;
    position.pos.y = pos.y;
    
    position.up.x = vel.x;
    position.up.y = vel.y;
    position.up.normalize();
    position.front.x = position.up.y;
    position.front.y = - position.up.x;
    velocity = vel;
    
    frameNum = 0;
    body->collisionType = COLLISION_WATERBALL;
    
    respawnTime = currentRespawnTime;
    attachPhysics();
}
