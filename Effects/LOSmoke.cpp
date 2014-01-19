//
//  LOSmoke.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 02.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LOSmoke.h"
#include "SunnyVector4D.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "LookOutEngine.h"
#include "SunnyShaders.h"

LOSmoke* sharedSmoke_ = 0;
const float customSmokeLifeTime = 2;
const float customAnomalySmokeLifeTime = 0.3;

LOSmoke* LOSmoke::sharedSmoke()
{
    if (!sharedSmoke_) sharedSmoke_ = new LOSmoke;
    return sharedSmoke_;
}

void LOSmoke::init()
{

}

void LOSmoke::clear()
{
}

LOSmoke::LOSmoke()
{
    init();
}

LOSmoke::~LOSmoke()
{
    sharedSmoke_ = 0;
    clear();
}

void LOSmoke::addRandomSmoke(SunnyVector2D pos)
{
    if (smokesCount>=maxSmokesCount) return;
    
    smokes[smokesCount].position = pos;
    smokes[smokesCount].velocity = SunnyVector2D(0.5 - sRandomf(), 0.5 - sRandomf());
    smokes[smokesCount].lifeTime = customSmokeLifeTime;
//    if (anomaled && sRandomi()%3==0)
//        anomalySmokes[anomalySmokesCount].smokeNum = 14+sRandomi()%3;
//    else
        smokes[smokesCount].smokeNum = sRandomi()%6;
    smokesCount++;
}

void LOSmoke::addGroundSmoke(SunnyVector2D pos)
{
    if (groundSmokesCount>=maxGroundSmokesCount) return;
    
    groundSmokes[groundSmokesCount].position = pos;
    groundSmokes[groundSmokesCount].velocity = SunnyVector2D(0.5 - sRandomf(), 0.5 - sRandomf());
    groundSmokes[groundSmokesCount].lifeTime = customSmokeLifeTime;
    groundSmokes[groundSmokesCount].smokeNum = 7;
    groundSmokesCount++;
}

void LOSmoke::addAnomalySmoke(SunnyVector2D pos)
{
    if (anomalySmokesCount>=maxAnomalySmokesCount) return;
    
    SunnyVector2D p;
//    if (sRandomi()%2==0)
//    {
//        p.x = sRandomf()*M_PI*2;
//        p = SunnyVector2D(cosf(p.x),sinf(p.x))*cellSize/3;
//    } else
        p = SunnyVector2D(0.5-sRandomf(),0.5-sRandomf())*2*cellSize/4;
    anomalySmokes[anomalySmokesCount].position = pos + p;
    anomalySmokes[anomalySmokesCount].velocity = -p*(1+sRandomi()%3);//SunnyVector2D(0.5 - sRandomf(), 0.5 - sRandomf());
    anomalySmokes[anomalySmokesCount].lifeTime = customAnomalySmokeLifeTime;
    anomalySmokes[anomalySmokesCount].smokeNum = 14+sRandomi()%3;
    anomalySmokesCount++;
}

void LOSmoke::update()
{
    if (mapWind.x || mapWind.y)
    {
        for (short i = 0;i<groundSmokesCount;i++)
        {
            groundSmokes[i].lifeTime -= 3*deltaTime;
            if (groundSmokes[i].lifeTime<=0)
            {
                groundSmokes[i] = groundSmokes[groundSmokesCount-1];
                i--;
                groundSmokesCount--;
                continue;
            } else
                groundSmokes[i].position += groundSmokes[i].velocity * deltaTime;
            
            groundSmokes[i].position += SunnyVector2D(mapWind.x,mapWind.y)*0.04;
        }
        
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
            
            smokes[i].position += SunnyVector2D(mapWind.x,mapWind.y)*0.04;
        }
        
        for (short i = 0;i<anomalySmokesCount;i++)
        {
            anomalySmokes[i].lifeTime -= deltaTime;
            if (anomalySmokes[i].lifeTime<=0)
            {
                anomalySmokes[i] = anomalySmokes[anomalySmokesCount-1];
                i--;
                anomalySmokesCount--;
                continue;
            } else
                anomalySmokes[i].position += anomalySmokes[i].velocity * deltaTime;
            
            anomalySmokes[i].position += SunnyVector2D(mapWind.x,mapWind.y)*0.04;
        }
    } else
    {
        for (short i = 0;i<groundSmokesCount;i++)
        {
            groundSmokes[i].lifeTime -= 3*deltaTime;
            if (groundSmokes[i].lifeTime<=0)
            {
                groundSmokes[i] = groundSmokes[groundSmokesCount-1];
                i--;
                groundSmokesCount--;
                continue;
            } else
                groundSmokes[i].position += groundSmokes[i].velocity * deltaTime;
        }
        
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
        
        for (short i = 0;i<anomalySmokesCount;i++)
        {
            anomalySmokes[i].lifeTime -= deltaTime;
            if (anomalySmokes[i].lifeTime<=0)
            {
                anomalySmokes[i] = anomalySmokes[anomalySmokesCount-1];
                i--;
                anomalySmokesCount--;
                continue;
            } else
                anomalySmokes[i].position += anomalySmokes[i].velocity * deltaTime;
        }
    }
}

void LOSmoke::render()
{
//    glBindVertexArray(verticesVAO);
    float scale;
    for (short i = 0;i<groundSmokesCount;i++)
    {
        scale = 1.2 + (customSmokeLifeTime-groundSmokes[i].lifeTime)/customSmokeLifeTime*0.8;
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(groundSmokes[i].position.x, groundSmokes[i].position.y, scale, scale));
        //glUniform1f(uniformTSA_A, groundSmokes[i].lifeTime/customSmokeLifeTime);
        glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, groundSmokes[i].lifeTime/customSmokeLifeTime));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+groundSmokes[i].smokeNum)*4, 4);
    }
    
    for (short i = 0;i<smokesCount;i++)
    {
        scale = 0.6 + (customSmokeLifeTime-smokes[i].lifeTime)/customSmokeLifeTime*0.4;
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(smokes[i].position.x, smokes[i].position.y, scale, scale));
//        glUniform1f(uniformTSA_A, smokes[i].lifeTime/customSmokeLifeTime);
        glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, smokes[i].lifeTime/customSmokeLifeTime));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+smokes[i].smokeNum)*4, 4);
    }
    
    for (short i = 0;i<anomalySmokesCount;i++)
    {
        scale = anomalySmokes[i].lifeTime/customAnomalySmokeLifeTime;
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(anomalySmokes[i].position.x, anomalySmokes[i].position.y, 1, 1));
//        glUniform1f(uniformTSA_A, scale);
        glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, scale));
        glDrawArrays(GL_TRIANGLE_STRIP, (LOSmoke_VAO+anomalySmokes[i].smokeNum)*4, 4);
        {
            short k = sRandomi()%15;
            if (k<=2)
            {
                glDrawArrays(GL_TRIANGLE_STRIP, (LOObject_VAO+57+k)*4, 4);
            }
        }
    }
//    glUniform1f(uniformTSA_A, 1);
}

void LOSmoke::reset()
{
    smokesCount = 0;
    groundSmokesCount = 0;
    anomalySmokesCount = 0;
}
