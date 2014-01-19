//
//  LOSnowflake.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 09.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "LOSnowflake.h"
#include "SunnyOpengl.h"
#include "SunnyVector4D.h"
#include "LookOutEngine.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"

float snowflakeRotationAngle = 0;
float snowflakeBackScale = 0.6;
bool snowflakeBackInc = true;

void LOSnowflake::initWithPosition(SunnyVector2D pos, bool animate)
{
    snowflakeRotationAngle = 0;
    position = pos;
    animated = animate;
}

void LOSnowflake::update()
{
    snowflakeRotationAngle -= deltaTime*30;
    if (snowflakeBackInc)
    {
        snowflakeBackScale += deltaTime/3.0*0.7;
        if (snowflakeBackScale>=1.3)
        {
            snowflakeBackScale = 1.3;
            snowflakeBackInc = false;
        }
    } else
    {
        snowflakeBackScale -= deltaTime/3.0*0.7;
        if (snowflakeBackScale<=0.6)
        {
            snowflakeBackScale = 0.6;
            snowflakeBackInc = true;
        }
    }
}

void LOSnowflake::renderDissapear(SunnyMatrix *MV)
{
    if (dissapearFrame>=2) return;
    float scale = 1.0 + dissapearFrame*0.45;

    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) * *MV;
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,1,1));
    
    m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m;
    SunnyVector2D angle = activeMap->getRotationAngle();
    m = getRotationZMatrix(angle.x*30./180*M_PI) * m;
    scale = 1.0 - fabsf(angle.y)*0.3;
    m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m;
    m = getRotationZMatrix(-42./180*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOSnowflake_VAO+2);
    
    dissapearFrame++;
}

void LOSnowflake::renderOverlay(SunnyMatrix *MV)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) * *MV;
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    
    if (animated)
    {
        float time = -snowflakeRotationAngle/30;
        const float halfPeriod = 0.15;
        if (time<halfPeriod*4)
        {
            float scale = 1;
            float s = fabsf(sinf(time*M_PI_2/halfPeriod))*2;
            glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale*(1+s*0.25),scale*(1+s*0.25)));
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,s));
            SunnyDrawArrays(LOOverlaySnowflake_VAO);
        }
    }
    
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
}

void LOSnowflake::render(SunnyMatrix *MV)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) * *MV;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));

    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0, snowflakeBackScale, snowflakeBackScale));
    SunnyDrawArrays(LOSnowflake_VAO+1);
    
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0, 1, 1));
   
    m = getRotationZMatrix(-snowflakeRotationAngle/180*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOSnowflake_VAO);
    
    dissapearFrame = 0;
}


void LOSnowflake::renderDissapear(SunnyVector2D pos,short frame, float scale2)
{
    if (frame>=2) return;
    float scale = 1.0 + frame*0.45;
    
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    SHTextureBase::bindTexture(objectsTexture);
    
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(pos.x, pos.y, -1));
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,scale2,scale2));
    
    m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m;
    SunnyVector2D angle = activeMap->getRotationAngle();
    m = getRotationZMatrix(angle.x*30./180*M_PI) * m;
    scale = 1.0 - fabsf(angle.y)*0.3;
    m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m;
    m = getRotationZMatrix(-42./180*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOSnowflake_VAO+2);
}

void LOSnowflake::renderOverlay(SunnyVector2D pos, float scale, float alpha)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(pos.x, pos.y, -1));
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,alpha));
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
    
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
}

void LOSnowflake::render(SunnyVector2D pos,float time, float scale, float alpha)
{
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    SHTextureBase::bindTexture(objectsTexture);
    glUniform4fv(uniformTSC_C,1, SunnyVector4D(1,1,1,alpha));
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(pos.x, pos.y, -1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    
    float back = (0.6 + (1+sinf(time))*0.7/2)*scale;
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0, back, back));
    SunnyDrawArrays(LOSnowflake_VAO+1);
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0, scale, scale));
    m = getRotationZMatrix(-time/6*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOSnowflake_VAO);
}
