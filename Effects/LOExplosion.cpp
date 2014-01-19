//
//  LOExplosion.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "LOExplosion.h"
#include "SunnyOpenGl.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LookOutEngine.h"
#include "LOTrace.h"
#include "SunnyShaders.h"

SunnyModelObj * explosionObj = 0;
unsigned short fireObjects[10+3];
unsigned short groundObjects[6];
unsigned short waterObjects[4];
unsigned short snowObjects[7];
unsigned short steamObjects[3];
unsigned short anomalyObjects[5];
unsigned short teleportationObject;
unsigned short shieldObjects[5];
unsigned short lifeObjects[4];
unsigned short speedObjects[2];

const int explosionFramesCount = 40;
const int explosionWaterFramesCount = 60;
const int explosionSnowFramesCount = 45;
const int explosionTeleportationFramesCount = 12;
const int explosionShieldFramesCount = 10;
const int explosionLifeFramesCount = 20;
const int explosionSpeedFramesCount = 20;

void LOExplosion::applySound()
{
/*    switch (explosionType) {
        case LOExplosion_Fire:
            losPlayFFExplosion((mat1->pos-player->position.pos).length());
            break;
            
        default:
            break;
    }*/
}

void LOExplosion::initWithParams(SunnyMatrix *pos, LOExplosionType type)
{
    explosionType = type;
    //explosionType = LOExplosion_Snow;
    
    frameNum = 0;
    position = *pos;

    frameNumFloat = frameNum;
    
    if (explosionType == LOExplosion_Shield) return;

    if (explosionType == LOExplosion_Speed)
    {
        frameNum = -1;
        for (short i = 0;i<3;i++)
        {
            beamPosition[i] = SunnyVector4D(0,0,0.01,1);
            float angle = sRandomf()*M_PI*2/3 + M_PI/6;
            splashVelocity[i] = SunnyVector3D(cosf(angle)*2,-sinf(angle)*2,1);
        }
    } else
    if (explosionType == LOExplosion_Life)
    {
        for (short i = 0;i<5;i++)
        {
            beamPosition[i] = SunnyVector4D(0.5-sRandomf(),0.5-sRandomf(),1,1+sRandomf()*0.5);
            float angle = sRandomf()*M_PI/4*3 + M_PI/8;
            splashVelocity[i] = SunnyVector3D(cosf(angle)*4,sinf(angle)*4,30./explosionLifeFramesCount)*(1+sRandomf()*0.5);
        }
        
        for (short i = 5;i<10;i++)
        {
            beamPosition[i] = SunnyVector4D(0,0,2,12+sRandomf()*3);
            float angle = sRandomf()*M_PI/4*3 + M_PI/8;
            splashVelocity[i] = SunnyVector3D(cosf(angle),sinf(angle),2*30./explosionLifeFramesCount)*(1+sRandomf()*0.5);
        }
    } else
    if (explosionType == LOExplosion_Teleportation || explosionType == LOExplosion_TeleportationPost)
    {
        frameNum = -1;
    } else
    if (explosionType == LOExplosion_Water)
    {
        float angle;
        for (short i = 0;i<15;i++)
        {
            beamPosition[i].x = 0.5-sRandomf();
            beamPosition[i].y = 0.5-sRandomf();
            beamPosition[i].z = 1.0;
            beamPosition[i].w = 1.0;
            angle = sRandomf()*2*M_PI;
            splashVelocity[i].x = 2*cosf(angle);
            splashVelocity[i].y = 2*sinf(angle) + 3;
            splashVelocity[i].z = sRandomf();
        }
    } else 
    if (explosionType == LOExplosion_Snow)
    {
        float angle;
        for (short i = 0;i<7;i++)
        {
            beamPosition[i].x = 0.5-sRandomf();
            beamPosition[i].y = 0.5-sRandomf();
            beamPosition[i].z = 1.0;
            beamPosition[i].w = 1.0;
            angle = sRandomf()*2*M_PI;
            splashVelocity[i].x = cosf(angle)*(1+i/4);
            splashVelocity[i].y = sinf(angle)*(1+i/4)+3;
            splashVelocity[i].z = sRandomf();
        }
        
        for (short i = 7;i<10;i++)
        {
            beamPosition[i].x = 0.5-sRandomf();
            beamPosition[i].y = 0.5-sRandomf();
            beamPosition[i].z = 1.0;
            beamPosition[i].w = 1.0;
            angle = sRandomf()*2*M_PI;
            splashVelocity[i].x = cosf(angle)*2;
            splashVelocity[i].y = sinf(angle)*2+3;
            splashVelocity[i].z = sRandomf();
        }
    } else
    if (explosionType == LOExplosion_Anomaly)
    {
        cloudRotations = 360*SunnyVector4D(sRandomf(),sRandomf(),sRandomf(),sRandomf());
        for (short i = 0; i<3; i++)
            cloudPosition[i] = SunnyVector2D(0.5 - sRandomf(),0.5 - sRandomf());
        beamPosition[0] = SunnyVector4D(0,0,60.,0.5+sRandomf()*0.5);
        beamPosition[1] = SunnyVector4D(0,0,-60.,0.5+sRandomf()*0.5);
    } else
    {
        cloudRotations = 360*SunnyVector4D(sRandomf(),sRandomf(),sRandomf(),sRandomf());
        for (short i = 0; i<3; i++)
        {
            cloudPosition[i] = SunnyVector2D(0.5 - sRandomf(),0.5 - sRandomf())*2;
            beamPosition[i] = SunnyVector4D(0,0,sRandomf()*360,0.5+sRandomf()*0.5);
        }
        
        beamPosition[3] = SunnyVector4D(0,0,90. + sRandomf()*180,0.5+sRandomf()*0.5);
        beamPosition[4] = SunnyVector4D(0,0,90. + sRandomf()*180,0.5+sRandomf()*0.5);
        
        beamPosition[5].x = sRandomf()*2*M_PI;
    }
}

bool LOExplosion::update(float deltaTime)
{
    frameNumFloat += 30*deltaTime;
    frameNum = frameNumFloat;
    
    if (explosionType == LOExplosion_Speed)
    {
        bool q = true;
        for (short i = 0;i<3;i++)
        {
//            if (beamPosition[i].z>0)
            {
                beamPosition[i].x += splashVelocity[i].x*deltaTime;
                beamPosition[i].y += splashVelocity[i].y*deltaTime;
                beamPosition[i].z += (splashVelocity[i].z - 3*deltaTime)*deltaTime;
                //splashVelocity[i].z -= 6*deltaTime;
                //q = false;
                beamPosition[i].z += splashVelocity[i].z*deltaTime;
                beamPosition[i].w -= deltaTime;
            } //else
            {
                //beamPosition[i].w -= deltaTime;
                if (beamPosition[i].w>0)
                    q = false;
            }
        }
        if (frameNum >= explosionSpeedFramesCount)
            return q;
    } else
    if (explosionType == LOExplosion_Life)
    {
        if (frameNum >= explosionLifeFramesCount)
            return true;
        
        for (short i = 0;i<5;i++)
        {
            beamPosition[i].x += splashVelocity[i].x*deltaTime;
            beamPosition[i].y += splashVelocity[i].y*deltaTime;
            beamPosition[i].z -= splashVelocity[i].z*deltaTime;
            beamPosition[i].w += deltaTime;
        }
        for (short i = 5;i<10;i++)
        {
            beamPosition[i].x += splashVelocity[i].x*deltaTime*8*2;
            beamPosition[i].y += splashVelocity[i].y*deltaTime*8*2;
            beamPosition[i].z -= splashVelocity[i].z*deltaTime;
            beamPosition[i].w -= 14*deltaTime*30./explosionLifeFramesCount*2;
        }
    } else
    if (explosionType == LOExplosion_Shield)
    {
        if (frameNum >= explosionShieldFramesCount)
            return true;
    } else
    if (explosionType == LOExplosion_Teleportation || explosionType == LOExplosion_TeleportationPost)
    {
        if (frameNum >= explosionTeleportationFramesCount)
            return true;
    } else
    if (explosionType == LOExplosion_Water)
    {
        if (frameNum >= explosionWaterFramesCount)
            return true;

        for (short i = 0;i<15;i++)
        {
            if (beamPosition[i].z>0)
            {
                beamPosition[i].x += splashVelocity[i].x*deltaTime;
                beamPosition[i].y += splashVelocity[i].y*deltaTime;
                beamPosition[i].z += (splashVelocity[i].z - 2*deltaTime)*deltaTime;
                splashVelocity[i].z -= 4*deltaTime;
            } else 
            {
                beamPosition[i].w -= deltaTime;
            }
        }
    }
    else
    if (explosionType == LOExplosion_Snow)
    {
        bool q = true;
        for (short i = 0;i<7;i++)
        {
            beamPosition[i].x += splashVelocity[i].x*deltaTime;
            beamPosition[i].y += splashVelocity[i].y*deltaTime;
            beamPosition[i].z += splashVelocity[i].z*deltaTime;
            beamPosition[i].w -= deltaTime;
        }

        for (short i = 7;i<10;i++)
        {
            if (beamPosition[i].z>0)
            {
                beamPosition[i].x += splashVelocity[i].x*deltaTime;
                beamPosition[i].y += splashVelocity[i].y*deltaTime;
                beamPosition[i].z += (splashVelocity[i].z - 3*deltaTime)*deltaTime;
                splashVelocity[i].z -= 6*deltaTime;
                q = false;
            } else 
            {
                beamPosition[i].w -= deltaTime;
                if (beamPosition[i].w>0)
                    q = false;
            }
        }
//        if (frameNum >= explosionSnowFramesCount)
//            return true;
        return q;
    }
    else
    if (explosionType == LOExplosion_Anomaly)
    {
        if (frameNum >= explosionFramesCount) return true;
        cloudRotations.x += 1;
        cloudRotations.y -= 2;
        cloudRotations.z -= 2.5;
        cloudRotations.w -= 3;
    }
    else
    {
        if (frameNum >= explosionFramesCount) return true;
        cloudRotations.x += 1;
        cloudRotations.y -= 2;
        cloudRotations.z -= 2.5;
        cloudRotations.w -= 3;
    }
    return false;
}

void LOExplosion::prepareForRender()
{
    explosionObj->prepareForRender(isVAOSupported);
    glDisableVertexAttribArray(2);
}

void LOExplosion::renderOverlay(SunnyMatrix *MV)
{
    if (explosionType == LOExplosion_Life)
    {
        SunnyMatrix m1 = position * *MV;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m1.front.x));
        if (frameNum==0)
        {
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
            glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1,1));
            SunnyDrawArrays(LOOverlayFireball_VAO);
        } else
            if (frameNum==1)
            {
                glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
                glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,2,2));
                SunnyDrawArrays(LOOverlayFireball_VAO);
            }
        
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1,1));
        SunnyMatrix m;
        float scale;
        for (short i = 5;i<10;i++)
        {
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,fminf(beamPosition[i].z,1)));
            m = getIdentityMatrix();
            m.up = SunnyVector4D(splashVelocity[i].x,splashVelocity[i].y,0,0);
            m.front = m.up ^ m.right;
            m.pos = SunnyVector4D(beamPosition[i].x,beamPosition[i].y,0,1);
            scale = fmaxf(beamPosition[i].w, 1);
            m = getScaleMatrix(SunnyVector3D(1,scale,1)) * getTranslateMatrix(SunnyVector3D(0,scale/4,0))* m * m1;
            glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
            //glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,fminf(1.0,beamPosition[i].z)));
            SunnyDrawArrays(LOOverlayLifeSpell_VAO);
        }
    } else
    if (explosionType == LOExplosion_Shield)
    {
        //SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) * *MV;
        SunnyMatrix m = position **MV;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1, cloudPosition[0].y));
        float scale = 2.5*cloudPosition[0].x;
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,cloudPosition[2].x,2*scale,scale));
        SunnyDrawArrays(LOOverlaySnowflake_VAO);
        SunnyDrawArrays(LOOverlaySnowflake_VAO);
        
        /*glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1, cloudPosition[1].y));
        scale = 2.5*cloudPosition[1].x;
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,cloudPosition[2].y,scale,scale));
        SunnyDrawArrays(LOOverlaySnowflake_VAO);*/
    } else
    if (explosionType == LOExplosion_Teleportation)
    {
        if (frameNum>=2)
        {
            SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) * *MV;
            glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
            float scale = 1.0 - ((float)(frameNumFloat-2))/(explosionTeleportationFramesCount-2);
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,scale));
            scale = (1 + (1-scale));
            glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
            SunnyDrawArrays(LOSpellTeleport_VAO);
        }
    } else
    if (explosionType == LOExplosion_TeleportationPost)
    {
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) * *MV;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        float scale = 1.0 - ((float)frameNumFloat)/(explosionTeleportationFramesCount);
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,243./255,67./255,scale));
        scale = (1 + (1-scale));
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    } else
    if (explosionType == LOExplosion_Fire)
    {
        if (frameNum>5) return;
        
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x, position.pos.y, 0)) * *MV;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        
        if (frameNum<=2)
        {
            glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
            glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,2,2));
            SunnyDrawArrays(LOOverlayFireball_VAO);
            if (frameNum==1)
            {
                glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,3,3));
                SunnyDrawArrays(LOOverlayFireball_VAO);
            }
        } else
        {
            glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,2,2));
            if (frameNum==3)
                glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,0.6));
            else
                glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,0.2));
            SunnyDrawArrays(LOOverlayFireball_VAO);
        }
    }
}

void LOExplosion::renderShadow(SunnyMatrix *MV)
{
    if (explosionType == LOExplosion_Fire || explosionType == LOExplosion_Ground)
    {
        sunnyUseShader(globalShaders[LOS_TexturedTSC]);
        float alpha = 2*(1.0 - (float)frameNumFloat/explosionFramesCount);
        glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, alpha));
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x-1, position.pos.y-1, 0)) **MV;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSmoke_VAO+12);
    } else
        if (explosionType == LOExplosion_Steam)
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSC]);
            float alpha = sinf((1.0 - (float)frameNumFloat/explosionFramesCount)*M_PI_4);
            glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, alpha));
            SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.pos.x-1, position.pos.y-1, 0)) **MV;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
            SunnyDrawArrays(LOSmoke_VAO+12);
        } else
            if (explosionType == LOExplosion_Snow)
            {
                SunnyMatrix m1,m;
                sunnyUseShader(globalShaders[LOS_TexturedTSC]);
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,1,1));
                for (short i = 7;i<10;i++)
                if (beamPosition[i].z>0)
                {
                    float move = beamPosition[i].z;
                    float scale = 1 + beamPosition[i].z*0.5;
                    m1 = position;
                    m1.pos.x -= move;
                    m1.pos.y -= move;
                    m1 = m1 **MV;
                    m = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, 0)) * m1;
                    m = getScaleMatrix(SunnyVector3D(scale,scale,scale)) * m;
                    glUniform4fv(uniformTSC_C, 1, SunnyVector4D(73./255.*globalColor.x, 86./255.*globalColor.y, 122./255.*globalColor.z, 0.5));
                    //glUniform1f(uniformTSC_C, 1);
                    if (i==7)
                    {
                        m = getRotationZMatrix(M_PI_2) * m;
                        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
                        SunnyDrawArrays(LOSnowExplosionShadow_VAO);
                    }
                    else
                    {
                        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
                        SunnyDrawArrays(LOSnowExplosionShadow_VAO+1);
                    }
                }
            }
}

void LOExplosion::renderUnderPlayer(SunnyMatrix *MV)
{
    sunnyUseShader(globalShaders[LOS_Textured3DA]);
    
    if (explosionType == LOExplosion_Speed)
    {
        SunnyMatrix m1 = position **MV;
        SunnyMatrix m;
        if (frameNum==0)
        {
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
            m = getScaleMatrix(SunnyVector3D(1,0.5,1)) * m1;
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            explosionObj->renderObject(speedObjects[0]);
        } else
            if (frameNum==1)
            {
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                m = getScaleMatrix(SunnyVector3D(1,1,1)) * m1;
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                explosionObj->renderObject(speedObjects[0]);
            } else
                if (frameNum==2)
                {
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                    m = getScaleMatrix(SunnyVector3D(1,2,1)) * m1;
                    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                    explosionObj->renderObject(speedObjects[0]);
                } else
                {
                    float scale;
                    for (short i =0;i<3;i++)
                    {
                        scale = 1 + beamPosition[i].z*0.5;
                        m = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z)) * m1;
                        m = getScaleMatrix(SunnyVector3D(scale,scale,scale))*m;
                        glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,beamPosition[i].w));
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        
                        explosionObj->renderObject(snowObjects[i]);
                    }
                    
                    if (frameNum<explosionSpeedFramesCount)
                    {
                        glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1.0 - ((float)(frameNumFloat-2))/(explosionSpeedFramesCount-2)));
                        scale = (frameNumFloat-2)*0.05+1;
                        m = getRotationZMatrix((frameNumFloat-2)*0.3) * getScaleMatrix(SunnyVector3D(scale,scale,scale)) * getTranslateMatrix(SunnyVector3D(0,-(frameNumFloat-2)*0.1,0)) * m1;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        explosionObj->renderObject(speedObjects[1]);
                    }
                }
    } else
    if (explosionType == LOExplosion_Snow)
    {
        SunnyMatrix m = position **MV;
        
        SunnyMatrix m1;
        float scale;
        //1
        for (short i = 7; i<10; i++)
            if (beamPosition[i].z<=0)
            {
                scale = 1 + beamPosition[i].z*0.5;
                m1 = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z)) * m;
                m1 = getScaleMatrix(SunnyVector3D(scale,scale,scale))*m1;
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,beamPosition[i].w));
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                if (i==7)
                    explosionObj->renderObject(snowObjects[4]);
                else
                    explosionObj->renderObject(snowObjects[5]);
            }
    } else
        if (explosionType == LOExplosion_Water)
        {
            SunnyMatrix m = position **MV;
            SunnyMatrix m1;
            
            float scale;
            //1
            for (short i = 0; i<15; i++)
                if (beamPosition[i].z<0)
                {
                    scale = 0.5 + beamPosition[i].z*0.5;
                    m1 = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z)) * m;
                    m1 = getScaleMatrix(SunnyVector3D(scale,scale,scale))*m1;
                    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                    
                    
                    if (beamPosition[i].w==1)
                    {
                        scale = 0.8 + 0.4*sRandomf();
                        glUniform4fv(uniform3D_A, 1, SunnyVector4D(scale*globalColor.x,scale*globalColor.y,scale*globalColor.z,1));
                    }
                    else
                        glUniform4fv(uniform3D_A, 1, SunnyVector4D(0.8*globalColor.x,0.8*globalColor.y,0.9*globalColor.z,beamPosition[i].w));
                    explosionObj->renderObject(waterObjects[i/4]);
                }
        }
}

void LOExplosion::render(SunnyMatrix *MV)
{
    SunnyMatrix m1,m;
    m1 = position **MV;
    
    if (explosionType == LOExplosion_TeleportationPost || explosionType == LOExplosion_Speed) return;
    
    if (explosionType == LOExplosion_Life)
    {
        if (frameNum<2)
        {
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
            explosionObj->renderObject(lifeObjects[frameNum]);
        } else
        {
            SunnyMatrix m;
            for (short i = 0;i<5;i++)
            {
                m = getScaleMatrix(SunnyVector3D(beamPosition[i].w,beamPosition[i].w,1)) * getTranslateMatrix(SunnyVector3D(beamPosition[i].x,beamPosition[i].y,0)) * m1;
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,beamPosition[i].z));
                explosionObj->renderObject(lifeObjects[2]);
            }
        }
        
        SunnyMatrix m;
        float scale;
        for (short i = 5;i<10;i++)
        {
            m = getIdentityMatrix();
            m.up = SunnyVector4D(splashVelocity[i].x,splashVelocity[i].y,0,0);
            m.front = m.up ^ m.right;
            m.pos = SunnyVector4D(beamPosition[i].x,beamPosition[i].y,0,1);
            scale = fmaxf(beamPosition[i].w, 1);
            m = getScaleMatrix(SunnyVector3D(1,scale,1)) * getTranslateMatrix(SunnyVector3D(0,scale/4,0))* m * m1;
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,fminf(1.0,beamPosition[i].z)));
            explosionObj->renderObject(lifeObjects[3]);
        }
    } else
    if (explosionType == LOExplosion_Shield)
    {
        float scale = 0.6 + 1.4*(frameNumFloat+1)/explosionShieldFramesCount;
        cloudPosition[0].x = scale;
        if (scale>1.5)
            cloudPosition[0].y = (2.0 - scale)*2;
        else
            cloudPosition[0].y = 1;
        glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,cloudPosition[0].y));
        
        cloudPosition[2].x = 16./1.4*(scale-0.6)/640*mapSize.y;
        SunnyMatrix m = getScaleMatrix(SunnyVector3D(scale,scale,1)) * getTranslateMatrix(SunnyVector3D(0,cloudPosition[2].x,0)) * m1;
        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
        explosionObj->renderObject(shieldObjects[4]);
        
        if (frameNum==0)
        {
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
            explosionObj->renderObject(shieldObjects[0]);
            //cloudPosition[1] = SunnyVector2D(1,1);
            //cloudPosition[2].y = 0;
        } else
            if (frameNum==1)
            {
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
               // m1 = getTranslateMatrix(SunnyVector3D(0,10./640*mapSize.y,0)) * m1;
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                explosionObj->renderObject(shieldObjects[1]);
                //cloudPosition[1] = SunnyVector2D(1,0.5);
                //cloudPosition[2].y = 10./640*mapSize.y;
            } else
                if (frameNum==2)
                {
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                    //m1 = getTranslateMatrix(SunnyVector3D(0,22./640*mapSize.y,0)) * m1;
                    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                    explosionObj->renderObject(shieldObjects[2]);
                    //cloudPosition[1] = SunnyVector2D(1,0.5);
                    //cloudPosition[2].y = 22./640*mapSize.y;
                } else
                    if (frameNum==3)
                    {
                        float alpha = 1.0 - ((float)(frameNumFloat-3))/(explosionShieldFramesCount-3);
                        cloudPosition[0].y = alpha*0.5;
                        glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,alpha));
                        alpha = (1 + (1-alpha));
                        //cloudPosition[1].x = alpha;
                        cloudPosition[2].y = ((frameNumFloat-3)*3)/640*mapSize.y;
                        m1 = getScaleMatrix(SunnyVector3D(alpha,alpha,1)) * getTranslateMatrix(SunnyVector3D(0,cloudPosition[2].y,0)) * m1;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                        explosionObj->renderObject(shieldObjects[3]);
                        //cloudPosition[2].y += 34./640*mapSize.y;
                    }
    } else
    if (explosionType == LOExplosion_Teleportation)
    {
        //flash
        if (frameNum==0)
        {
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
            explosionObj->renderObject(fireObjects[0]);
        }
        else
            if (frameNum==1)
            {
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                explosionObj->renderObject(fireObjects[1]);
            }
            else
            {
                float scale = 1.0 - ((float)(frameNumFloat-2))/(explosionTeleportationFramesCount-2);
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,scale));
                scale = (1 + (1-scale));
                m = getScaleMatrix(SunnyVector3D(scale,scale,scale)) * m1;
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                explosionObj->renderObject(teleportationObject);
            }
    } else
    if (explosionType == LOExplosion_Water)
    {
        float scale;
        //1
        for (short i = 0; i<15; i++)
        if (beamPosition[i].z>=0)
        {
            m = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z)) * m1;
            scale = 0.5 + beamPosition[i].z*0.5;
            m = getScaleMatrix(SunnyVector3D(scale,scale,scale)) * m;
            if (beamPosition[i].w==1)
            {
                scale = 0.8 + 0.4*sRandomf();
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(scale*globalColor.x,scale*globalColor.y,scale*globalColor.z,1));
            }
            else
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(0.8*globalColor.x,0.8*globalColor.y,0.9*globalColor.z,1));
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            explosionObj->renderObject(waterObjects[i/4]);
        }
    } else
    if (explosionType == LOExplosion_Snow)
    {        
        float scale;
        //1
        for (short i = 0; i<10; i++)
        if (beamPosition[i].z>0)
        {
            scale = 1 + beamPosition[i].z*0.5;
            m = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z)) * m1;
            m = getScaleMatrix(SunnyVector3D(scale,scale,scale)) * m;
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x, globalColor.y, globalColor.z, beamPosition[i].w));

            short num;
            if (i==7) num = 4;
            else if (i>7) num = 5;
            else
            {
                num = (i+1)/1.6;
                if (num>3) num = 3;
            }
            explosionObj->renderObject(snowObjects[num]);
        }
    } else
    {
        if (explosionType == LOExplosion_Ground)
        {
            m1 = getScaleMatrix(SunnyVector3D(1.5, 1.5, 1.0)) *m1;
            //flash
            if (frameNum==0)
            {
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                explosionObj->renderObject(groundObjects[0]);
            }
            else
                if (frameNum==1)
                {
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));
                    explosionObj->renderObject(groundObjects[1]);
                }
                else
                {
                    float scale = 1.0 + (float)frameNumFloat/explosionFramesCount*0.5;
                    m1 = getScaleMatrix(SunnyVector3D(scale,scale,1)) *m1;
                    float alpha = 1.0 - (float)frameNumFloat/explosionFramesCount;
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,alpha));
                    //1 layer
                    m = getRotationZMatrix(cloudRotations.x*M_PI/180) * m1;
                    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                    explosionObj->renderObject(groundObjects[2]);
                    
                    //3 layer
                    for (short i = 0; i<2; i++) 
                    {
                        m = getRotationZMatrix(beamPosition[i].z*M_PI/180) * m1;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        explosionObj->renderObject(groundObjects[3+i]);
                    }
                    
                    //4 layer
                    for (short i = 0;i<3;i++)   
                    {
                        m = getTranslateMatrix(SunnyVector3D(cloudPosition[i].x, cloudPosition[i].y, 0)) *m1;
                        m = getRotationZMatrix(cloudRotations[i+1]*M_PI/180) * m;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        explosionObj->renderObject(groundObjects[5]);
                    }
                }
        } else
        if (explosionType == LOExplosion_Steam)
        {
            float scale;
            if (frameNum==0)
                scale = 0.2;
            else
                if (frameNum==1)
                    scale = 0.7;
            else
                scale = 1.0 + 0.5*frameNumFloat/explosionFramesCount;
            
            m1 = getScaleMatrix(SunnyVector3D(scale,scale,1)) *m1;
            if (frameNumFloat>explosionFramesCount/2)
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1.0 - 2*fabsf(explosionFramesCount/2. - frameNumFloat)/explosionFramesCount));
            else
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1.0));

            m = getRotationZMatrix(cloudRotations.x*M_PI/180) * m1;
            glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
            explosionObj->renderObject(steamObjects[1]);
            
            if (frameNum<=10)
            {
                if (frameNum<5)
                {
                    if (frameNum>=2)
                        scale = 1.0 + frameNumFloat*0.6/4;
                    m = getScaleMatrix(SunnyVector3D(scale, scale, scale)) *m1;
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1.0));
                } else
                {
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,(10.-frameNumFloat)/(10.-4.)));
                    m = m1;
                }
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                explosionObj->renderObject(steamObjects[0]);
            }
            
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1.0 - 2*fabsf(explosionFramesCount/2. - frameNumFloat)/explosionFramesCount));
            for (short i = 0;i<3;i++)
            {
                m = getTranslateMatrix(SunnyVector3D(cloudPosition[i].x, cloudPosition[i].y, 0))*m1;
                m = getRotationZMatrix(cloudRotations[i+1]*M_PI/180) * m;
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                explosionObj->renderObject(steamObjects[2]);
            }
        } else
        if (explosionType == LOExplosion_Anomaly)
        {
            float scale = 1.0 + (float)frameNumFloat/explosionFramesCount*0.5;
            m1 = getScaleMatrix(SunnyVector3D(scale,scale,1)) *m1;
            float alpha = 1.0 - (float)frameNumFloat/explosionFramesCount;
            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,alpha));
            
            //1 layer = was removed before es2
            
            //4 layer
            for (short i = 0;i<3;i++)
            {
                m = getTranslateMatrix(SunnyVector3D(cloudPosition[i].x, cloudPosition[i].y, 0))*m1;
                m = getRotationZMatrix(cloudRotations[i+1]*M_PI/180) * m;
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                explosionObj->renderObject(anomalyObjects[i%2+3]);
            }
            
            //2 layer
            for (short i = 0; i<2; i++)
            {
                float newAlpha = alpha * beamPosition[i].w;
                beamPosition[i].x = -sinf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                beamPosition[i].y =  cosf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,newAlpha));
                m = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, 0))*m1;
                m = getRotationZMatrix(beamPosition[i].z*M_PI/180) * m;
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                explosionObj->renderObject(anomalyObjects[i%2+1]);
            }
        } else
        {
            //flash
            if (frameNum==0)
            {
                glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));

                explosionObj->renderObject(fireObjects[0]);
                for (short i = 0;i<2;i++)
                {
                    SunnyVector2D p = SunnyVector2D(position.pos.x, position.pos.y) + SunnyVector2D(0.5-sRandomf(),0.5-sRandomf())*cellSize*1.8;
                    activeMap->mapInfo.addSpark(p,SunnyVector2D(position.up.x,position.up.y)*1.3);
                }
            }
            else
                if (frameNum==1)
                {
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1));
                    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m1.front.x));

                    explosionObj->renderObject(fireObjects[1]);
                    for (short i = 0;i<3;i++)
                    {
                        SunnyVector2D p = SunnyVector2D(position.pos.x, position.pos.y) + SunnyVector2D(0.5-sRandomf(),0.5-sRandomf())*cellSize*1.8;
                        activeMap->mapInfo.addSpark(p,SunnyVector2D(position.up.x,position.up.y)*1.3);
                    }
                }
                else
                {
                    float scale = 1.0 + (float)frameNumFloat/explosionFramesCount*0.5;
                    m1 = getScaleMatrix(SunnyVector3D(scale,scale,1)) *m1;
                    float alpha = 1.0 - (float)frameNumFloat/explosionFramesCount;
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,alpha));
                    
                    //1 layer
                    m = getRotationZMatrix(cloudRotations.x*M_PI/180) * m1;
                    glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                    explosionObj->renderObject(fireObjects[2]);
                    
                    //2 layer
                    for (short i = 0; i<3; i++) 
                    {
                        float newAlpha = alpha * beamPosition[i].w;
                        beamPosition[i].x = -sinf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                        beamPosition[i].y =  cosf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                        glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,newAlpha));
                        m = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, 0)) * m1;
                        m = getRotationZMatrix(beamPosition[i].z*M_PI/180) * m;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        explosionObj->renderObject(fireObjects[3]);
                    }
                    glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,alpha));
                    
                    //3 layer
                    for (short i = 3; i<5; i++) 
                    {
                        beamPosition[i].x = -sinf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                        beamPosition[i].y =  cosf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                        m = getTranslateMatrix(SunnyVector3D(beamPosition[i].x, beamPosition[i].y, 0)) * m1;
                        m = getRotationZMatrix(beamPosition[i].z*M_PI/180) * m;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        explosionObj->renderObject(fireObjects[i+1]);
                    }
                    
                    //4 layer
                    for (short i = 0;i<3;i++)
                    {
                        m = getTranslateMatrix(SunnyVector3D(cloudPosition[i].x, cloudPosition[i].y, 0)) * m1;
                        m = getRotationZMatrix(cloudRotations[i+1]*M_PI/180) * m;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        explosionObj->renderObject(fireObjects[6+i]);
                    }
                    
                    //new layer
                    if (frameNum<=5)
                    {
                        m = getRotationZMatrix(beamPosition[5].x) * m1;
                        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
                        if (frameNum==5)
                        {
                            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,0.5));
                            explosionObj->renderObject(fireObjects[12]);
                        }
                        else
                        {
                            glUniform4fv(uniform3D_A, 1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z,1.0));
                            explosionObj->renderObject(fireObjects[10+frameNum-2]);
                        }
                    }
                }
        }
    }
}

void LOExplosion::render1()
{
    if (explosionType == LOExplosion_Water)
    {
        //glColor4f(1, 1, 1, 1);
        glPushMatrix();
        glMultMatrixf(&position[0][0]);
        
        float scale;
        //1
        for (short i = 0; i<15; i++)
        {
            glPushMatrix();
            glTranslatef(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z);
            scale = 0.5 + beamPosition[i].z*0.5;
            glScalef(scale, scale, scale);
            if (beamPosition[i].w==1)
            {
                scale = 0.8 + 0.4*sRandomf();
                glColor4f(scale, scale, scale, 1.0);
            }
            else
                glColor4f(0.8, 0.8, 0.9, beamPosition[i].w);
            glDrawArrays(GL_TRIANGLE_STRIP, (16 + i/4)*4, 4);
            glPopMatrix();
        }
        
        glPopMatrix();
    } else
    if (explosionType == LOExplosion_Snow)
    {
        glPushMatrix();
        glMultMatrixf(&position[0][0]);
        
        float scale;
        //1
        for (short i = 0; i<9; i++)
        {
            glPushMatrix();
            glTranslatef(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z);
            scale = 0.5 + beamPosition[i].z*0.5;
            glScalef(scale, scale, scale);
            glColor4f(1, 1, 1, beamPosition[i].w);
            glDrawArrays(GL_TRIANGLE_STRIP, (20 + i/3)*4, 4);
            glPopMatrix();
        }
        
        for (short i = 9; i<15; i++)
        {
            glPushMatrix();
            glTranslatef(beamPosition[i].x, beamPosition[i].y, beamPosition[i].z);
            scale = 0.5 + beamPosition[i].z*0.5;
            glScalef(scale, scale, scale);
            glColor4f(1, 1, 1, beamPosition[i].w);
            glDrawArrays(GL_TRIANGLE_STRIP, 23*4, 4);
            glPopMatrix();
        }
        
        glPopMatrix();
    } else
    {
        glPushMatrix();
        glMultMatrixf(&position[0][0]);
        if (explosionType == LOExplosion_Ground)
        {
            glScalef(1.5, 1.5, 1.0);
            //flash
            if (frameNum==0)
                glDrawArrays(GL_TRIANGLE_STRIP, 10*4, 4);
            else
            if (frameNum==1)
                glDrawArrays(GL_TRIANGLE_STRIP, 11*4, 4);
            else
            {
                float scale = 1.0 + (float)frameNumFloat/explosionFramesCount*0.5;
                glScalef(scale, scale, 1);
                float alpha = 1.0 - (float)frameNumFloat/explosionFramesCount;
                glColor4f(1, 1, 1, alpha);
                //1 layer
                glPushMatrix();
                glRotatef(cloudRotations.x, 0, 0, 1);
                glDrawArrays(GL_TRIANGLE_STRIP, 12*4, 4);
                glPopMatrix();
                
                //3 layer
                for (short i = 0; i<2; i++) 
                {
                    glPushMatrix();
                    glRotatef(beamPosition[i].z, 0, 0, 1);
                    glDrawArrays(GL_TRIANGLE_STRIP, (13+i)*4, 4);
                    glPopMatrix();
                }
                
                //4 layer
                for (short i = 0;i<3;i++)
                {
                    glPushMatrix();
                    glTranslatef(cloudPosition[i].x, cloudPosition[i].y, 0);
                    glRotatef(cloudRotations[i+1], 0, 0, 1);
                    glDrawArrays(GL_TRIANGLE_STRIP, 15*4, 4);
                    glPopMatrix();
                }
            }
        } else
        {
            //flash
            if (frameNum==0)
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            else
            if (frameNum==1)
                glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
            else
            {
                float scale = 1.0 + (float)frameNumFloat/explosionFramesCount*0.5;
                glScalef(scale, scale, 1);
                float alpha = 1.0 - (float)frameNumFloat/explosionFramesCount;
                glColor4f(1, 1, 1, alpha);
                //1 layer
                glPushMatrix();
                glRotatef(cloudRotations.x, 0, 0, 1);
                glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
                glPopMatrix();
                
                //2 layer
                for (short i = 0; i<3; i++) 
                {
                    float newAlpha = alpha * beamPosition[i].w;
                    beamPosition[i].x = -sinf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                    beamPosition[i].y =  cosf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                    glColor4f(1, 1, 1, newAlpha);
                    glPushMatrix();
                    glTranslatef(beamPosition[i].x, beamPosition[i].y, 0);
                    glRotatef(beamPosition[i].z, 0, 0, 1);
                    glDrawArrays(GL_TRIANGLE_STRIP, 3*4, 4);
                    glPopMatrix();
                }
                glColor4f(1, 1, 1, alpha);
                
                //3 layer
                for (short i = 3; i<5; i++) 
                {
                    beamPosition[i].x = -sinf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                    beamPosition[i].y =  cosf(beamPosition[i].z*M_PI/180)*(float)frameNumFloat/explosionFramesCount*0.5;
                    glPushMatrix();
                    glTranslatef(beamPosition[i].x, beamPosition[i].y, 0);
                    glRotatef(beamPosition[i].z, 0, 0, 1);
                    glDrawArrays(GL_TRIANGLE_STRIP, (i+1)*4, 4);
                    glPopMatrix();
                }
                
                //4 layer
                for (short i = 0;i<3;i++)
                {
                    glPushMatrix();
                    glTranslatef(cloudPosition[i].x, cloudPosition[i].y, 0);
                    glRotatef(cloudRotations[i+1], 0, 0, 1);
                    glDrawArrays(GL_TRIANGLE_STRIP, (6+i)*4, 4);
                    glPopMatrix();
                }
            }
            
            if (frameNum==0 || frameNum==1)
            {
                glPushMatrix();
                SunnyVector2D angle = activeMap->getRotationAngle();
                glRotatef(angle.x*30, 0, 0, 1);
                float scale = 1.0 - fabsf(angle.y)*0.3;
                glScalef(scale, scale, 1.0);
                glRotatef(-42, 0, 0, 1);
                glDrawArrays(GL_TRIANGLE_STRIP, 9*4, 4);
                glPopMatrix();
            }
        }
        glPopMatrix();
    }
}

void LOExplosion::prepareExplosions()
{
    if (explosionObj) return;
    explosionObj = new SunnyModelObj;
    explosionObj->loadFromSobjFile(getPath("Base/Objects/EXPLOSIONS.sh3do"));
    explosionObj->makeVBO(true,isVAOSupported,false);
    
    for (short i = 0;i<explosionObj->objectsCount;i++)
    {
        if (!strcmp(explosionObj->objects[i]->objectName, "Teleportation_01"))
            teleportationObject = i;
        else
        if (!strcmp(explosionObj->objects[i]->objectName, "Shield_Frame_01"))
            shieldObjects[0] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Shield_Frame_02"))
                shieldObjects[1] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Shield_Frame_03"))
                    shieldObjects[2] = i;
                else
                    if (!strcmp(explosionObj->objects[i]->objectName, "Shield_Frame_04"))
                        shieldObjects[3] = i;
                    else
                        if (!strcmp(explosionObj->objects[i]->objectName, "Shield_S_01"))
                            shieldObjects[4] = i;
                        else
        if (!strcmp(explosionObj->objects[i]->objectName, "Fire_Frame_01"))
            fireObjects[0] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Fire_Frame_02"))
                fireObjects[1] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Fire_SmokeRays_01"))
                    fireObjects[3] = i;
                else
                    if (!strcmp(explosionObj->objects[i]->objectName, "Fire_SmokeRays_02"))
                        fireObjects[4] = i;
                    else
                        if (!strcmp(explosionObj->objects[i]->objectName, "Fire_SmokeRay"))
                            fireObjects[5] = i;
                        else
                            if (!strcmp(explosionObj->objects[i]->objectName, "Fire_BigDownSmoke"))
                                fireObjects[2] = i;
                            else
                                if (!strcmp(explosionObj->objects[i]->objectName, "Fire_SmallSmoke_01"))
                                    fireObjects[6] = i;
                                else
                                    if (!strcmp(explosionObj->objects[i]->objectName, "Fire_SmallSmoke_02"))
                                        fireObjects[7] = i;
                                    else
                                        if (!strcmp(explosionObj->objects[i]->objectName, "Fire_SmallSmoke_03"))
                                            fireObjects[8] = i;
                                        else
                                            if (!strcmp(explosionObj->objects[i]->objectName, "Fire_Lignting_Frames"))
                                                fireObjects[9] = i;
                                            else
                                                if (!strcmp(explosionObj->objects[i]->objectName, "Fire_Frame_03"))
                                                    fireObjects[10] = i;
                                                else
                                                    if (!strcmp(explosionObj->objects[i]->objectName, "Fire_Frame_04"))
                                                        fireObjects[11] = i;
                                                    else
                                                        if (!strcmp(explosionObj->objects[i]->objectName, "Fire_Frame_05"))
                                                            fireObjects[12] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Earth_Frame_01"))
                groundObjects[0] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Earth_Frame_02"))
                    groundObjects[1] = i;
                else
                    if (!strcmp(explosionObj->objects[i]->objectName, "Earth_SmokeRay_01"))
                        groundObjects[3] = i;
                    else
                        if (!strcmp(explosionObj->objects[i]->objectName, "Earth_SmokeRay_02"))
                            groundObjects[4] = i;
                        else
                            if (!strcmp(explosionObj->objects[i]->objectName, "Earth_BigDownSmoke"))
                                groundObjects[2] = i;
                            else
                                if (!strcmp(explosionObj->objects[i]->objectName, "Earth_SmallUpSmoke"))
                                    groundObjects[5] = i;
                                else
        if (!strcmp(explosionObj->objects[i]->objectName, "Water_Splash_01"))
            waterObjects[0] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Water_Splash_02"))
                waterObjects[1] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Water_Splash_03"))
                    waterObjects[2] = i;
                else
                    if (!strcmp(explosionObj->objects[i]->objectName, "Water_Splash_04"))
                        waterObjects[3] = i;
                    else
        if (!strcmp(explosionObj->objects[i]->objectName, "Snow_01"))
            snowObjects[0] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Snow_02"))
                snowObjects[1] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Snow_03"))
                    snowObjects[2] = i;
                else
                    if (!strcmp(explosionObj->objects[i]->objectName, "SnowLight_01"))
                        snowObjects[3] = i;
                    else
                        if (!strcmp(explosionObj->objects[i]->objectName, "Snow_04"))
                            snowObjects[4] = i;
                        else
                            if (!strcmp(explosionObj->objects[i]->objectName, "Snow_05"))
                                snowObjects[5] = i;
                            else
                                if (!strcmp(explosionObj->objects[i]->objectName, "Snow_BIG_01"))
                                    snowObjects[6] = i;
                                else
        if (!strcmp(explosionObj->objects[i]->objectName, "Water_Steam_FirstFrames"))
            steamObjects[0] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Water_Steam_BigDown"))
                steamObjects[1] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Water_Steam_SmollUp"))
                    steamObjects[2] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Anomaly_DownSmoke"))
                anomalyObjects[0] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Anomaly_Ray_01"))
                    anomalyObjects[1] = i;
                else
                    if (!strcmp(explosionObj->objects[i]->objectName, "Anomaly_Ray_02"))
                        anomalyObjects[2] = i;
                    else
                        if (!strcmp(explosionObj->objects[i]->objectName, "Anomaly_SmallUpSmoke_02"))
                            anomalyObjects[3] = i;
                        else
                            if (!strcmp(explosionObj->objects[i]->objectName, "Anomaly_SmallUpSmoke_01"))
                                anomalyObjects[4] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Life_Frame_01"))
                lifeObjects[0] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Life_Frame_02"))
                    lifeObjects[1] = i;
                else
                    if (!strcmp(explosionObj->objects[i]->objectName, "Life_Frame_03"))
                        lifeObjects[2] = i;
                    else
                        if (!strcmp(explosionObj->objects[i]->objectName, "Life_Particle"))
                            lifeObjects[3] = i;
        else
            if (!strcmp(explosionObj->objects[i]->objectName, "Speed_Start_01"))
                speedObjects[0] = i;
            else
                if (!strcmp(explosionObj->objects[i]->objectName, "Speed_Start_02"))
                    speedObjects[1] = i;

    }
}

void LOExplosion::clearExplosions()
{
    if (explosionObj)
        delete explosionObj;
}