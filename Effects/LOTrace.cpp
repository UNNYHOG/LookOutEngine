//
//  LOTrace.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 04.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOTrace.h"
#include "SunnyVector4D.h"
#include "LookOutEngine.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "SunnyShaders.h"

extern float *menuOffset;

LOTrace* sharedTrace_ = 0;

LOTrace* LOTrace::sharedTrace()
{
    if (!sharedTrace_) sharedTrace_ = new LOTrace;
    return sharedTrace_;
}

void LOTrace::init()
{
}

void LOTrace::update()
{
    float time = deltaTime/5;
    for (short i = 0;i<playerTracesCount;i++)
    {
        playerTraces[i].alpha -= time;
        if (playerTraces[i].alpha<=0)
        {
            playerTraces[i] = playerTraces[playerTracesCount-1];
            i--;
            playerTracesCount--;
            continue;
        }
    }
    for (short i = 0;i<otherPlayersTracesCount;i++)
    {
        otherPlayersTraces[i].alpha -= time;
        if (otherPlayersTraces[i].alpha<=0)
        {
            otherPlayersTraces[i] = otherPlayersTraces[otherPlayersTracesCount-1];
            i--;
            otherPlayersTracesCount--;
            continue;
        }
    }
    
    time*=2;
    for (short i = 0;i<tracesCount;i++)
    {
        traces[i].alpha -= time;
        if (traces[i].alpha<=0)
        {
            traces[i] = traces[tracesCount-1];
            i--;
            tracesCount--;
            continue;
        }
    }
    
    time = deltaTime*0.1;
    for (short i = 0;i<postsCount;i++)
    {
        posts[i].alpha -= time;
        if (posts[i].alpha<=0)
        {
            for (short j = i;j<postsCount-1;j++)
                posts[j] = posts[j+1];
            postsCount--;
        }
    }
}

void LOTrace::render(SunnyMatrix *MV)
{
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(MV->front.x));

    if (menuMode)
    {
        for (short i = 0;i<tracesCount;i++)
        {
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(traces[i].position.x-*menuOffset, traces[i].position.y,traces[i].scale,traces[i].scale));
            glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,traces[i].alpha));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOTraces_VAO+traces[i].traceNum)*4, 4);
        }
    } else
    {
        for (short i = 0;i<tracesCount;i++)
        {
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(traces[i].position.x, traces[i].position.y,traces[i].scale,traces[i].scale));
            glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,traces[i].alpha*0.7));
            glDrawArrays(GL_TRIANGLE_STRIP, (LOTraces_VAO+traces[i].traceNum)*4, 4);
        }
    }

    for (short i = 0;i<playerTracesCount;i++)
    { 
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(playerTraces[i].position.x, playerTraces[i].position.y,playerTraces[i].scale,playerTraces[i].scale));
        glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,playerTraces[i].alpha*0.7));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOTraces_VAO+playerTraces[i].traceNum)*4, 4);
    }
    
    for (short i = 0;i<otherPlayersTracesCount;i++)
    {
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(otherPlayersTraces[i].position.x, otherPlayersTraces[i].position.y,otherPlayersTraces[i].scale,otherPlayersTraces[i].scale));
        glUniform4fv(uniformTSC_C, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,otherPlayersTraces[i].alpha*0.7));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOTraces_VAO+otherPlayersTraces[i].traceNum)*4, 4);
    }
}

void LOTrace::renderPosts(SunnyMatrix *MV)
{
    sunnyUseShader(globalShaders[LOS_TexturedC]);
    
    SunnyMatrix m;
    for (short i = 0; i<postsCount; i++)
    {
        m = posts[i].position * *MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedC], 1, GL_FALSE, &(m.front.x));
        glUniform4fv(uniformA_A,1,SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,posts[i].alpha*0.7));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOTraces_VAO+posts[i].num)*4, 4);
    }
}

void LOTrace::addRandomTrace(SunnyVector2D pos, float scale, float alpha)
{
    if (tracesCount>=maxTracesCount) return;
    
    traces[tracesCount].position = pos;
    traces[tracesCount].traceNum = sRandomi()%3;
    traces[tracesCount].alpha = alpha;
    traces[tracesCount].scale = scale;
    tracesCount++;
}

void LOTrace::addRandomPlayerTrace(SunnyVector2D pos, float scale)
{
    if (playerTracesCount>=maxPlayerTracesCount) return;
    
    playerTraces[playerTracesCount].position = pos;
    playerTraces[playerTracesCount].traceNum = sRandomi()%3;
    playerTraces[playerTracesCount].alpha = 1.0;
    playerTraces[playerTracesCount].scale = scale;
    playerTracesCount++;
}

void LOTrace::addRandomOtherPlayerTrace(SunnyVector2D pos, float scale)
{
    if (otherPlayersTracesCount>=maxPlayerTracesCount*maxPlayersCount) return;
    
    otherPlayersTraces[otherPlayersTracesCount].position = pos;
    otherPlayersTraces[otherPlayersTracesCount].traceNum = sRandomi()%3;
    otherPlayersTraces[otherPlayersTracesCount].alpha = 1.0;
    otherPlayersTraces[otherPlayersTracesCount].scale = scale;
    otherPlayersTracesCount++;
}

void LOTrace::addSnowWallPostEffect(SunnyMatrix* pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 14;
    posts[postsCount].alpha = 1;
    postsCount++;
}

void LOTrace::addTeleportationPostEffect(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 15;
    posts[postsCount].alpha = 1;
    postsCount++;
}

void LOTrace::addRandomPartEffect(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 6 + sRandomi()%3;
    posts[postsCount].alpha = 1;
    postsCount++;
}

void LOTrace::addIceTrace(SunnyVector2D pos, short direction)
{
    if (postsCount>=maxPostsCount) return;
    SunnyMatrix mat = getIdentityMatrix();
    mat.pos.x = pos.x;mat.pos.y = pos.y;
    if (direction!=4)
    {
        float angle;
        if (direction==1)
            angle = M_PI_2;
        else
            if (direction==3)
                angle = -M_PI_2;
            else angle = M_PI;
        mat.front.x = cosf(angle);
        mat.front.y = sinf(angle);
        mat.up.x = -mat.front.y;
        mat.up.y = mat.front.x;
    }
    
    posts[postsCount].position = mat;
    posts[postsCount].num = 13;
    posts[postsCount].alpha = 0.5;
    postsCount++;
}

void LOTrace::addFireTrace(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 3;
    posts[postsCount].alpha = 1;
    postsCount++;
}

void LOTrace::addWaterTrace(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 4;
    posts[postsCount].alpha = 1;
    postsCount++;
}

void LOTrace::addAngryBallTrace(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 12;
    posts[postsCount].alpha = 2;
    postsCount++;
}

void LOTrace::addAngryBallRespTrace(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 11;
    posts[postsCount].alpha = 2;
    postsCount++;
}

void LOTrace::addGroundTrace(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 5;
    posts[postsCount].alpha = 1;
    postsCount++;
}

void LOTrace::addGroundWaterTrace(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 9;
    posts[postsCount].alpha = 1;
    postsCount++;
}

void LOTrace::addElectricTrace(SunnyMatrix *pos)
{
    if (postsCount>=maxPostsCount) return;
    posts[postsCount].position = *pos;
    posts[postsCount].num = 10;
    posts[postsCount].alpha = 1;
    postsCount++;    
}

void LOTrace::addGroundPartTrace(SunnyVector2D pos)
{
    if (tracesCount>=maxTracesCount) return;
    
    traces[tracesCount].position = pos;
    traces[tracesCount].traceNum = 9;
    traces[tracesCount].alpha = 1;
    traces[tracesCount].scale = 1.0;
    tracesCount++;
}

void LOTrace::reset()
{
    tracesCount = 0;
    postsCount = 0;
    playerTracesCount = 0;
    otherPlayersTracesCount = 0;
}

void LOTrace::clear()
{
//    if (verticesVAO) glDeleteVertexArrays(1, &verticesVAO);
//    if (verticesVBO) glDeleteBuffers(1, &verticesVBO);
}

LOTrace::LOTrace()
{
    init();
}

LOTrace::~LOTrace()
{
    sharedTrace_ = 0;
    clear();
}