//
//  LORollingBlades.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 08.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "LORollingBlades.h"
#include "SunnyOpengl.h"

#include "SunnyOpenGL.h"
#include "LOGlobalVar.h"
#include "LookOutEngine.h"
#include "iPhone_Strings.h"

const float halfBladeWidth = cellSize/5;
const float bladeLength = cellSize*3;
const float halfElectricWidth = cellSize/8;
const float electricLength = cellSize*2;
const float timeToAngle = M_PI/3;
const float timeToDistance = 4;

float verts[] = {0,-electricLength,0,electricLength};

void LORollingBlades::renderUnderPlayer()
{
#ifndef SKIP_PHYSICS
    //ложбинка
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D((bounds.x + bounds.y)/2, position.y, (bounds.y-bounds.x)/(16./640*mapSize.y), 1));
    glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+6)*4, 4);

    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(bounds.x, position.y,1, 1));
    glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+7)*4, 4);

    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(bounds.y, position.y,1, 1));
    glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+8)*4, 4);
#endif
}

void LORollingBlades::renderElectricFieldShadow(SunnyMatrix *MV)
{
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x - lightPosition.x*(0.15/lightPosition.z), position.y - lightPosition.y*(0.15/lightPosition.z),0));
    m = m * *MV;
    if (type==4)// glRotatef(90, 0, 0, 1);
        m = getRotationZMatrix(M_PI_2) * m;
    
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, 1));
    
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, -2*cellSize, 1, 1));
    SunnyDrawArrays(LOShadows_VAO+7);
    
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, 2*cellSize, 1, -1));
    SunnyDrawArrays(LOShadows_VAO+7);
}

void LORollingBlades::renderOverlay(SunnyMatrix *MV)
{
    if (!charged) return;
    
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) * *MV;
    if (type==4) m = getRotationZMatrix(M_PI_2) * m;
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,-2*cellSize,1,1));
    SunnyDrawArrays(LOOverlayElectricity_VAO);
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,2*cellSize,1,1));
    SunnyDrawArrays(LOOverlayElectricity_VAO);
}

void LORollingBlades::render(SunnyMatrix *MV)
{
#ifndef SKIP_PHYSICS
    if (type==3 || type==4)
    {
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) * *MV;
        if (type==4) m = getRotationZMatrix(M_PI_2) * m;//glRotatef(90, 0, 0, 1);
        glUniformMatrix4fv(globalModelview[LOS_Textured3DA], 1, GL_FALSE, &(m.front.x));
        if (bounds.y<0)
        {
            electricField->renderObject(0);
            if (charged)
            {
                attachPhysics();
                electricField->renderObject(1+sRandomi()%5);
                if (attaking)
                    electricField->renderObject(6+sRandomi()%3);
            } else
                detachPhysics();
            return;
        }
        short count = (realPlayingTime+startAngle)/bounds.y;
        float time = realPlayingTime+startAngle - count*bounds.y;
        
        electricField->renderObject(0);
        if (time<bounds.x)//on
        {
            attachPhysics();
            electricField->renderObject(1+sRandomi()%5);
            if (attaking)
                electricField->renderObject(6+sRandomi()%3);
            charged = 1;
        } else//off
        {
            detachPhysics();
            charged = 0;
        }
    } else
    if (type==2)//rolling
    {
        if (!physicsPaused)
        {
            position.x += velocityScale*deltaTime*timeToDistance;
            startAngle+=(float)deltaTime;
        }
        if (velocityScale>0)
        {
            if (position.x>=bounds.y)
            {
                velocityScale = -velocityScale;
                position.x = bounds.y - (position.x - bounds.y);
                startAngle = 0;
            }
        } else
        {
            if (position.x<=bounds.x)
            {
                velocityScale = -velocityScale;
                position.x = bounds.x + (bounds.x - position.x);
                startAngle = 0;
            }
        }
        
        body->position.x = position.x;
        
        SunnyMatrix m1 = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) **MV;
        SunnyVector2D shadowTranslate(-lightPosition.x*(0.3/lightPosition.z),-lightPosition.y*(0.3/lightPosition.z));
        
        //6        
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(shadowTranslate.x, shadowTranslate.y, 0)) * m1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+5)*4, 4);
        m = getRotationZMatrix(M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+5)*4, 4);
        //5
        m = getTranslateMatrix(SunnyVector3D(shadowTranslate.x, shadowTranslate.y, 0)) * m1;
        m = getRotationZMatrix(-M_PI_4);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+4)*4, 4);
        //4
        m = m1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+3)*4, 4);
        m = getRotationZMatrix(M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+3)*4, 4);
        //3
        m = getTranslateMatrix(SunnyVector3D(shadowTranslate.x/2, shadowTranslate.y/2, 0)) * m1;
        m = getRotationZMatrix(M_PI_4);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+2)*4, 4);
        
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m1.front.x));
        //2
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+1)*4, 4);
        //1
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO)*4, 4);
    } else
    {
        float angle = (realPlayingTime*velocityScale*timeToAngle + startAngle);
        blade->rotationAngle = angle;
        SunnyVector2D shadowTranslate(-lightPosition.x*(0.3/lightPosition.z),-lightPosition.y*(0.3/lightPosition.z));
        SunnyMatrix m1 = getTranslateMatrix(SunnyVector3D(position.x, position.y, 0)) **MV;
        
        //6
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(shadowTranslate.x, shadowTranslate.y, 0)) * m1;
        m = getRotationZMatrix(angle)*m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+5)*4, 4);
        m = getRotationZMatrix(M_PI*2/3)*m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+5)*4, 4);
        m = getRotationZMatrix(M_PI*2/3)*m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+5)*4, 4);
        //5
        m = getTranslateMatrix(SunnyVector3D(shadowTranslate.x, shadowTranslate.y, 0)) * m1;
        m = getRotationZMatrix(-M_PI_4);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+4)*4, 4);
        //4
        m = getRotationZMatrix(angle)*m1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+3)*4, 4);
        m = getRotationZMatrix(M_PI*2/3)*m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+3)*4, 4);
        m = getRotationZMatrix(M_PI*2/3)*m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+3)*4, 4);
        //3
        m = getTranslateMatrix(SunnyVector3D(shadowTranslate.x/2, shadowTranslate.y/2, 0)) * m1;
        m = getRotationZMatrix(M_PI_4);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+2)*4, 4);
        //2
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m1.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO+1)*4, 4);
        //1
        m = getRotationZMatrix(angle)*m1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
        glDrawArrays(GL_TRIANGLE_STRIP, (LORollingBlades_VAO)*4, 4);
    }
#endif
}

void LORollingBlades::renderPhysics()
{
    glPushMatrix();
    glTranslatef(position.x, position.y, 0);
    if (type==4) glRotatef(90, 0, 0, 1);
//    glDisable(GL_TEXTURE_2D);
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glColor4f(1, 0, 1, 1);
    glDrawArrays(GL_LINES, 0, 2);
//    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}

LORollingBlades::~LORollingBlades()
{
    if (body)
    {
        scDeactivateStaticBox(body);
        delete body;
    }
    if (blade)
    {
        scDeactivateRollingBlades(blade);
        delete blade;
    }
}

LORollingBlades::LORollingBlades()
{
    type = 0;
    isPhysicsAttached = true;
    body = 0;
    blade = 0;
}

void LORollingBlades::reset()
{
    if (type==1)
    {
        position.x = resetParams.x;
        position.y = resetParams.y;
        velocityScale = resetParams.z;
        startAngle = resetParams.w;
    } else
    if (type==2)
    {
        position = SunnyVector2D(resetParams.x,resetParams.y);
        velocityScale = resetParams.z;
    } else
    {
        
    }
}

void LORollingBlades::initWithParam(SunnyVector4D params)
{
    type = 1;
    position.x = params.x;
    position.y = params.y;
    velocityScale = params.z;
    startAngle = params.w*M_PI/180;
    
    resetParams = params;
    resetParams.w = startAngle;
    
    blade = scCreateRollingBlade(bladeLength, 3, halfBladeWidth, SunnyVector2D(position.x,position.y),true);
    blade->rotationAngle = startAngle;
    blade->layers = LAYER_BLADES;
    blade->collisionType = COLLISION_ROLLING_BLADES;
}

void LORollingBlades::initWithParam(SunnyVector2D pos, SunnyVector3D params)
{
    type = 2;
    position = pos;
    velocityScale = params.z;
    bounds.x = params.x;
    bounds.y = params.y;
    
    resetParams = SunnyVector4D(pos.x,pos.y,velocityScale,0);
    
    body = scCreateStaticBox(SunnyVector2D(2*halfBladeWidth,2*bladeLength), position,true, true);
    body->layers = LAYER_BLADES;
    body->collisionType = COLLISION_ROLLING_BLADES;
}

void LORollingBlades::initElectricity(LOBufElectricField params)
{
    charged = 0;
    attaking = 0;
    type = 3+params.type;
    position = params.position;
    bounds.x = params.params.x;
    bounds.y = params.params.y;
    startAngle = params.params.z;
    
    if (type==3)
        body = scCreateStaticBox(SunnyVector2D(2*halfElectricWidth,2*electricLength), position, true);
    else
        body = scCreateStaticBox(SunnyVector2D(2*electricLength,2*halfElectricWidth), position, true);
    body->layers = LAYER_ELECTRIC;
    body->collisionType = COLLISION_ELECTRICFIELD;
    body->data = this;
    isPhysicsAttached = false;
}

void LORollingBlades::attachPhysics()
{
    if (isPhysicsAttached) return;
    isPhysicsAttached = true;

    scActivateStaticBox(body);
}

void LORollingBlades::detachPhysics()
{
    if (!isPhysicsAttached) return;
    isPhysicsAttached = false;

    scDeactivateStaticBox(body);
}

void LORollingBlades::prepareElectricFieldForRender()
{
    electricField->prepareForRender(isVAOSupported);
    glDisableVertexAttribArray(2);
}