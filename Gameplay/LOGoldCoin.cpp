//
//  LOGoldCoin.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 02.08.13.
//
//

#include "LOGoldCoin.h"
#include "iPhone_Strings.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"
#include "LOEvents.h"

unsigned short goldObjects[2];

LOGoldCoin::LOGoldCoin()
{
    physicsCreated = false;
    physics = scCreateCircle(cellSize/2*(2./4.));
    physics->layers = LAYER_SNOWFLAKE;
    physics->collisionType = COLLISION_GOLD_COIN;
    physics->data = this;
}

LOGoldCoin::~LOGoldCoin()
{
    scDeactivateCircle(physics);
    delete physics;
}

void LOGoldCoin::collectCoin(bool grab)
{
    physicsCreated = false;
    scDeactivateCircle(physics);
    
    if (grab)
        LOEvents::OnGoldCoinCollected();
}

void LOGoldCoin::initWithPosition(SunnyVector2D pos)
{
    const float minDist = 8;
    physicsCreated = false;
    float angle;
    if (pos.x < minDist)
    {
        if (pos.y < minDist)
            angle = -sRandomf()*M_PI_2;
        else
            if (pos.y > mapSize.y-minDist)
                angle = -(sRandomf()+1)*M_PI_2;
        else
            angle = -sRandomf()*M_PI;
    } else
        if (pos.x > mapSize.x-minDist)
        {
            if (pos.y < minDist)
                angle = sRandomf()*M_PI_2;
            else
                if (pos.y > mapSize.y-minDist)
                    angle = (sRandomf()+1)*M_PI_2;
                else
                    angle = sRandomf()*M_PI;
        } else
            if (pos.y < minDist)
                angle = (sRandomf()-0.5)*M_PI;
            else
                if (pos.y > mapSize.y-minDist)
                    angle = (sRandomf()+0.5)*M_PI;
                else
                    angle = sRandomf()*M_PI*2;
    position = getRotationZMatrix(angle) * getTranslateMatrix(SunnyVector3D(pos.x,pos.y,minCoinHeight*1.01));
    verticalVelocity = coinsVerticalVelocity;
    rotAngle = M_PI_2;//sRandomf()*M_PI;
    velocity = SunnyVector2D(position.up.x,position.up.y)*3;
}

void LOGoldCoin::prepareModel()
{
    goldCoinObj = new SunnyModelObj;
    goldCoinObj->loadFromSobjFile(getPath("Base/Objects/Gold","sh3do"));
	goldCoinObj->makeVBO(true,isVAOSupported,false);

    goldObjects[0] = goldCoinObj->getObjectNum(goldCoinObj->getObjectByName("Gold"));
    goldObjects[1] = goldCoinObj->getObjectNum(goldCoinObj->getObjectByName("LensFlare"));
}

void LOGoldCoin::prepareForRender()
{
    goldCoinObj->prepareForRender(isVAOSupported);
}

void LOGoldCoin::renderForMenu(SunnyVector2D pos)
{
    glEnable(GL_DEPTH_TEST);
    goldCoinObj->prepareForRender(isVAOSupported);
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_Textured3DNoLight]);
    glUniform4fv(uniform3DNL_C, 1, SunnyVector4D(1,1,1,1));
    
    SunnyMatrix m =getIdentityMatrix();
    m.pos = SunnyVector4D(pos.x,pos.y,-10,1);
    m = getRotationYMatrix(M_PI/6) * getRotationXMatrix(-M_PI/6) *  m;
    glUniformMatrix4fv(globalModelview[LOS_Textured3DNoLight], 1, GL_FALSE, &(m.front.x));
    goldCoinObj->renderObject(goldObjects[0]);
    glDisable(GL_DEPTH_TEST);
}

void LOGoldCoin::renderShadow(SunnyMatrix *MV)
{
    SunnyMatrix m = position*getTranslateMatrix(SunnyVector3D(- lightPosition.x*(position.pos.z-minCoinHeight+0.1)*2,
                                                              - lightPosition.y*(position.pos.z-minCoinHeight+0.1)*2,0))**MV;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformTSC_C,1, SunnyVector4D(globalColor.x,globalColor.y,globalColor.z, (1.0 - 0.5*(position.pos.z - minCoinHeight)/(coinsHeight-minCoinHeight))));
    float scale = 0.6;
    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0,0,scale, scale*(0.5+0.5*fabsf(cosf(rotAngle)))));
    SunnyDrawArrays(LOShadows_VAO);
}

float LOGoldCoin::getMove()
{
    float move = 300.*(rotAngle+M_PI_2)/M_PI;
    move -= rotationAngle.y*20;
    while (move>390) move-=390;
    return move;
}

void LOGoldCoin::renderModelForMenu(SunnyMatrix* mat,float alpha, float angle)
{
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_Textured3DNoLight]);
    glUniform4fv(uniform3DNL_C, 1, SunnyVector4D(1,1,1,alpha));
    
    glUniformMatrix4fv(globalModelview[LOS_Textured3DNoLight], 1, GL_FALSE, &(mat->front.x));
    goldCoinObj->renderObject(goldObjects[0]);
    
    float move = 300.*(angle+M_PI_2)/M_PI;
    move -= rotationAngle.y*20;
    while (move>390) move-=390;
//    float move = 120-rotationAngle.y*40;
    glUniform2fv(uniform3DNL_Tex, 1, SunnyVector2D(0, move/2048.));
    glUniform4fv(uniform3DNL_C, 1, SunnyVector4D(1,1,1,0.5*alpha));
    goldCoinObj->renderObject(goldObjects[1]);
    glUniform2fv(uniform3DNL_Tex, 1, SunnyVector2D(0, 0));
    
    
    if (move>100 && move<200)
    {
        glDisable(GL_DEPTH_TEST);
        SunnyMatrix *m1 = mat;
        bindGameGlobal();
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SHTextureBase::bindTexture(objectsTexture);
        glUniform1f(uniformTSA_A, 1.0 - fabsf(150-move)/50);
        
        float scale = 1;
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
        SunnyMatrix m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) ** m1;
        SunnyVector2D angle = activeMap->getRotationAngle();
        m = getRotationZMatrix(angle.x*30./180*M_PI) * m;
        scale = 1.0 - fabsf(angle.y)*0.3;
        m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m;
        m = getRotationZMatrix(-42./180*M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSnowflake_VAO+2);
        
        LOGoldCoin::prepareForRender();
        SHTextureBase::bindTexture(menuTexture);
        glEnable(GL_DEPTH_TEST);
    }
}

void LOGoldCoin::render(SunnyMatrix *MV)
{
    glEnable(GL_DEPTH_TEST);
    LOGoldCoin::prepareForRender();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_Textured3DNoLight]);
    glUniform4fv(uniform3DNL_C, 1, SunnyVector4D(1,1,1,1));
    
    SunnyMatrix m1 = position**MV;
    float scale = (1.0 + 0.6*(position.pos.z - minCoinHeight)/(coinsHeight-minCoinHeight))/2;
    SunnyMatrix m = getRotationXMatrix(rotAngle) * getScaleMatrix(SunnyVector3D(scale,scale,scale)) * m1;
    glUniformMatrix4fv(globalModelview[LOS_Textured3DNoLight], 1, GL_FALSE, &(m.front.x));
    goldCoinObj->renderObject(goldObjects[0]);
    
    float move = getMove();
    glUniform2fv(uniform3DNL_Tex, 1, SunnyVector2D(0, move/2048.));
    goldCoinObj->renderObject(goldObjects[1]);
    glUniform2fv(uniform3DNL_Tex, 1, SunnyVector2D(0, 0));
    
    glDisable(GL_DEPTH_TEST);
    
    if (move>100 && move<200)
    {
        m1 = getTranslateMatrix(SunnyVector3D(position.pos.x,position.pos.y,position.pos.z))**MV;
        bindGameGlobal();
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SHTextureBase::bindTexture(objectsTexture);
        glUniform1f(uniformTSA_A, 1.0 - fabsf(150-move)/50);
        
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
        m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m1;
        SunnyVector2D angle = activeMap->getRotationAngle();
        m = getRotationZMatrix(angle.x*30./180*M_PI) * m;
        scale = 1.0 - fabsf(angle.y)*0.3;
        m = getScaleMatrix(SunnyVector3D(scale, scale, 1)) * m;
        m = getRotationZMatrix(-42./180*M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOSnowflake_VAO+2);
    }
}

void LOGoldCoin::renderOverlay(SunnyMatrix *MV)
{
    if (physicsCreated)
    {
        SunnyMatrix m = position**MV;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,fmaxf(0, sinf(physicsCreationTime-realPlayingTime))));
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,4,4));
        SunnyDrawArrays(LOOverlayLifeSpell_VAO);
        return;
    }
    
    float move = getMove();
    if (move>100 && move<200)
    {
        SunnyMatrix m = position**MV;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1.0 - fabsf(150-move)/50));
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,4,4));
        SunnyDrawArrays(LOOverlayLifeSpell_VAO);
    }
}

void LOGoldCoin::update()
{
    if (position.pos.z>minCoinHeight)
    {
        double time = deltaTime*2;
        rotAngle -= time*8;
        if (rotAngle<0) rotAngle += 2*M_PI;
        position.pos.x += velocity.x*time;
        position.pos.y += velocity.y*time;
        position.pos.z += (verticalVelocity - 3*time)*time;
        verticalVelocity -= 6*time;
        if (position.pos.z < minCoinHeight)
            position.pos.z = minCoinHeight;
    } else
    {
        if (!physicsCreated)
        {
            physicsCreated = true;
            scActivateCircle(physics);
            physicsCreationTime = realPlayingTime;
        }
        velocity *= 0.9;
        position.pos.x += velocity.x*deltaTime;
        position.pos.y += velocity.y*deltaTime;
        position.pos.z = minCoinHeight;
        
        physics->position.x = position.pos.x;
        physics->position.y = position.pos.y;
    }
}
