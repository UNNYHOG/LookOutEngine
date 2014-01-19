//
//  LOBonfire.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOBonfire.h"
#include "SunnyVector4D.h"
#include "LookOutEngine.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"

const float customBonfireSmokeLifeTime = 2;
const short customBonfireSmokeDelay = 5;
const float maxFireTranslateDist = 0.1;

LOBonfire::LOBonfire()
{
    
}

void LOBonfire::initWithPosition(SunnyVector2D pos)
{
    position = pos;
#ifndef SKIP_PHYSICS    
    randomAngle = sRandomf()*360.;
    lightRotate = sRandomi()%2;

    for (short i = 0;i<3;i++)
        fireRotations[i] = sRandomf()*360;
    animationFrame = sRandomi()%12;
#endif    
    smokesCount = 0;
    smokeDelay = 0;
    
    fireVelocity[0] = fireVelocity[1] = SunnyVector2D(0,0);
    fireTranslate[0] = fireTranslate[1] = SunnyVector2D(maxFireTranslateDist,0);    
}

void LOBonfire::renderOverlay(SunnyMatrix *MV)
{
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,0.65 + sRandomf()*0.2));
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) * *MV;
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOOverlayFireball_VAO);
}

void LOBonfire::render(SunnyMatrix *MV)
{
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));

    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) **MV;
    m = getRotationZMatrix(randomAngle*M_PI/180) * m;
    //3 light
    if (noFire)
        glUniform1f(uniformTSA_A, 1.0 - noFireAlpha);
    else
        glUniform1f(uniformTSA_A, 1);
    
    if (lightRotate<0)
    {
        SunnyMatrix m1 = getRotationZMatrix(3./180*M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m1.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (4+LOBonfire_VAO)*4, 4);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    } else
    {
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (4+LOBonfire_VAO)*4, 4);
    }
    
    //1 wood
    glDrawArrays(GL_TRIANGLE_STRIP, LOBonfire_VAO*4, 4);
    if (noFire)
    {
        glUniform1f(uniformTSA_A, noFireAlpha);
        glDrawArrays(GL_TRIANGLE_STRIP, (17+LOBonfire_VAO)*4, 4);
        glUniform1f(uniformTSA_A, 1.0 - noFireAlpha);
    }
    
    m = getRotationZMatrix(randomAngle*M_PI/180) * m;

    float scale;
    SunnyMatrix m1 = getRotationZMatrix(fireRotations[0]/180*M_PI) * m;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m1.front.x));
    glDrawArrays(GL_TRIANGLE_STRIP, (5+animationFrame+LOBonfire_VAO)*4, 4);
    
    for (short i = 1;i<3;i++)
    {
        SunnyMatrix m1 = getRotationZMatrix(fireRotations[i]/180*M_PI) *getTranslateMatrix(SunnyVector3D(fireTranslate[i-1].x, fireTranslate[i-1].y,0)) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m1.front.x));
        scale = 1.0 - i*0.1;
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
        glDrawArrays(GL_TRIANGLE_STRIP, (5+animationFrame+LOBonfire_VAO)*4, 4);
    }
}

void LOBonfire::renderAfterPlayer(SunnyMatrix *MV)
{
    SunnyMatrix m1 = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) **MV;
    float scale;    
    SunnyMatrix m;
    for (short i = 0;i<smokesCount;i++)
    {
        scale = 3.0*(customBonfireSmokeLifeTime-smokes[i].lifeTime)/customBonfireSmokeLifeTime;
        if (scale>1.0) glUniform1f(uniformTSA_A, (3.0 - scale)/2.0);//glColor4f(1, 1, 1, (3.0 - scale)/2.0);
        else
            glUniform1f(uniformTSA_A, 1);//glColor4f(1, 1, 1, 1.0);
        
        m = getTranslateMatrix(SunnyVector3D(smokes[i].position.x, smokes[i].position.y,0)) *m1;
        m = getScaleMatrix(SunnyVector3D(scale, scale, 1))*m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (smokes[i].smokeNum+LOBonfire_VAO)*4, 4);
    }
}

void LOBonfire::update()
{
    lightRotate++;
    if (lightRotate>=2) lightRotate = -2;
    animationFrame = (animationFrame+1)%12;
    
    fireRotations.x += 2;
    fireRotations.y -= 2.5;
    fireRotations.z += 3;
    
    if (noFire && (noFireAlpha<1))
        noFireAlpha += deltaTime;
    
    if (mapWind.x || mapWind.y)
    {
        for (short i = 0;i<smokesCount;i++)
        {
            smokes[i].lifeTime -= deltaTime;
            if (smokes[i].lifeTime<=0)
            {
                smokes[i] = smokes[smokesCount-1];
                i--;
                smokesCount--;
                continue;
            } else
                smokes[i].position += smokes[i].velocity * deltaTime;
            
            smokes[i].position += SunnyVector2D(mapWind.x,mapWind.y)*0.05;
        }
    } else
    for (short i = 0;i<smokesCount;i++)
    {
        smokes[i].lifeTime -= deltaTime;
        if (smokes[i].lifeTime<=0)
        {
            smokes[i] = smokes[smokesCount-1];
            i--;
            smokesCount--;
            continue;
        } else
            smokes[i].position += smokes[i].velocity * deltaTime;
    }
    if (smokeDelay>0)
        smokeDelay--;
    if (smokeDelay<=0 && smokesCount<maxBondireSmokesCount && !noFire)
    {
        smokeDelay = customBonfireSmokeDelay;
        SunnyVector2D pos = SunnyVector2D(0.5 - sRandomf(), 0.5 - sRandomf());
        smokes[smokesCount].position = pos;
        smokes[smokesCount].velocity = SunnyVector2D(0.5 - sRandomf(), 0.5 - sRandomf());
        smokes[smokesCount].lifeTime = customBonfireSmokeLifeTime;
        smokes[smokesCount].smokeNum = sRandomi()%3+1;
        smokesCount++;
    }
    
    for (short i = 0;i<2;i++)
    {
        fireTranslate[i] += fireVelocity[i]*deltaTime;
        if (fabsf(fireTranslate[i].x) >= maxFireTranslateDist || fabsf(fireTranslate[i].y) >= maxFireTranslateDist)
        {
            SunnyVector2D pos = SunnyVector2D(0.5-sRandomf(),0.5-sRandomf())*maxFireTranslateDist;
            fireVelocity[i] = (pos - fireTranslate[i]);
        }
    }
}

bool LOBonfire::isActive()
{
    return !noFire;
}

void LOBonfire::extinguishFire()
{
    noFire = true;
    noFireAlpha = 0;
}

void LOBonfire::reset()
{
    noFire = false;
}
