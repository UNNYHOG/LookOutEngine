//
//  LOTutorial.cpp
//  LookOut
//
//  Created by Ignatov on 01.11.13.
//
//

#include "LOTutorial.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"

static LOTutorial *sharedTutorial = 0;

void emptyRenderFunction(SunnyButton * btn)
{
    
}

void tutorialButtonPressed(SunnyButton *btn)
{
    if (sharedTutorial)
        sharedTutorial->buttonPressed(btn);
}

void LOTutorial::setName(const char * name, float scale)
{
    if (nameString) delete [] nameString;
    nameString = getLocalizedLabel(name);
    nameScale = scale;
}

bool LOTutorial::blockRenderOverlays()
{
    return false;
}

LOTutorial::LOTutorial()
{
    sharedTutorial = this;
    isVisible = false;
    lockButton = 0;
    okButton = 0;
    nameString = 0;
    
    frameSize = SunnyVector2D(mapSize.x*0.7,mapSize.y*0.7);
    namePosition = SunnyVector2D(mapSize.x/2,mapSize.y*0.70);
    windowCenter = mapSize/2;
}

LOTutorial::~LOTutorial()
{
    gamePaused = lastGameState;
    if (lockButton)
    {
        sunnyRemoveButton(lockButton);
        lockButton = 0;
    }
    if (okButton)
    {
        sunnyRemoveButton(okButton);
        okButton = 0;
    }
    
    if (nameString)
        delete []nameString;
}

void LOTutorial::addStandartOkButtonToClose(void (*func)(SunnyButton*))
{
    if (okButton) return;
    
    okButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,windowCenter.y-frameSize.y/2 + backBorderSize/4), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,okButtonSize*pixelToScale);
    if (func!=0)
        sunnyButtonSetCallbackFunc(okButton, func);
    else
        sunnyButtonSetCallbackFunc(okButton, tutorialButtonPressed);
    sunnyButtonSetRenderFunc(okButton, emptyRenderFunction);
    okButton->matrix.pos.z = -2;
    
    okButtonAlpha = 0;
}

void LOTutorial::setVisible(bool visible)
{
    if (isVisible == visible) return;
    
    isVisible = visible;
    
    if (isVisible)
    {
        tutorialTime = 0;
        lastGameState = gamePaused;
        gamePaused = true;
        appearAlpha = 0;
        if (lockButton==0)
        {
            lockButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapSize.y/2), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(100, 100));
            sunnyButtonSetRenderFunc(lockButton, emptyRenderFunction);
            lockButton->soundTag = 0;
        }
    } else
    {
        gamePaused = lastGameState;
        if (lockButton)
        {
            sunnyRemoveButton(lockButton);
            lockButton = 0;
        }
        if (okButton)
        {
            sunnyRemoveButton(okButton);
            okButton = 0;
        }
    }
}

void LOTutorial::update(float deltaTime)
{
    appearAlpha+=deltaTime*5;
    if (appearAlpha>1) appearAlpha = 1;
    
    tutorialTime+=deltaTime;
    
    okButtonAlpha += deltaTime*3;
    if (okButtonAlpha>1)
        okButtonAlpha = 1;
}

bool LOTutorial::renderOverlay()
{
    return true;
}

void LOTutorial::reset()
{
    tutorialTime = 0;
}

void LOTutorial::buttonPressed(SunnyButton *btn)
{
    if (btn==okButton)
    {
        LOMenu::getSharedMenu()->closeTutorial();
    }
}

void LOTutorial::renderBackForSize(SunnyVector2D frameSize, SunnyVector2D center, float alpha, SunnyVector3D color)
{
    const float farDist = -20;
    
    if (isVAOSupported)
        glBindVertexArray(solidVAO);
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, solidVBO);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(-0.5,0,0))* getScaleMatrix(SunnyVector3D(frameSize.x-backBorderSize*2/3,frameSize.y-backBorderSize*2/3,1)) * getTranslateMatrix(SunnyVector3D(center.x,center.y,farDist));
    sunnyUseShader(globalShaders[LOS_SolidObject]);
    glUniformMatrix4fv(globalModelview[LOS_SolidObject], 1, GL_FALSE, &(m.front.x));
    SunnyVector4D c = SunnyVector4D(color.x,color.y,color.z,alpha);
    glUniform4fv(uniformSO_Color,1, c);
    SunnyDrawArrays(0);
    
    m = getTranslateMatrix(SunnyVector3D(center.x,center.y,farDist));
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, alpha);
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    //left
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-frameSize.x/2, 0, 1,(frameSize.y-backCornerSize*2)/backSideSize));
    SunnyDrawArrays(LOMessageBorders_VAO+1);
    
    //right
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(frameSize.x/2,0, -1, (frameSize.y-backCornerSize*2)/backSideSize));
    SunnyDrawArrays(LOMessageBorders_VAO+1);
    
    //glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, (frameSize.x-backCornerSize*2)/backSideSize));
    //top
    //m = getRotationZMatrix(-M_PI_2) * getTranslateMatrix(SunnyVector3D(center.x,center.y+frameSize.y/2,farDist));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,frameSize.y/2, (frameSize.x-backCornerSize*2)/backSideSize,1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOMessageBorders_VAO+2);
    
    //bottom
//    m = getRotationZMatrix(M_PI_2) * getTranslateMatrix(SunnyVector3D(center.x,center.y-frameSize.y/2,farDist));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,-frameSize.y/2, (frameSize.x-backCornerSize*2)/backSideSize,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOMessageBorders_VAO+2);
    
    //Corners
    m = getTranslateMatrix(SunnyVector3D(center.x,center.y,farDist));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-frameSize.x/2, frameSize.y/2, 1,1));
    SunnyDrawArrays(LOMessageBorders_VAO);
    
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(frameSize.x/2, frameSize.y/2, -1,1));
    SunnyDrawArrays(LOMessageBorders_VAO);
    
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(frameSize.x/2, -frameSize.y/2, -1,-1));
    SunnyDrawArrays(LOMessageBorders_VAO);
    
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-frameSize.x/2, -frameSize.y/2, 1,-1));
    SunnyDrawArrays(LOMessageBorders_VAO);
}

void LOTutorial::render()
{
    //render back
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    
    glDisable(GL_DEPTH_TEST);
    const float farDist = -20;
    //fade
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y/2,farDist));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform1f(uniformTSA_A, 0.5*appearAlpha);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    SunnyDrawArrays(LODeath_VAO+1);
    
    //Back
    renderBackForSize(frameSize,windowCenter,appearAlpha);
    
    float scale;
    if (okButton)
    {
        scale = okButtonAlpha/1.5;
        if (okButton->active) scale*=0.85;
        m = getScaleMatrix(scale);
        m.pos = okButton->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        
        glUniform1f(uniformTSA_A, appearAlpha*okButtonAlpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        SunnyDrawArrays(LOOkButtonBack_VAO);
        LOFont::writeText(&m, LOTA_Center, true, loLabelOk, SunnyVector4D(250,251,253,255*appearAlpha*okButtonAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha*okButtonAlpha)/255);
    }
    if (nameString)
    {
        m = getScaleMatrix(nameScale) * getTranslateMatrix(SunnyVector3D(namePosition.x,namePosition.y,-1));
        LOFont::writeText(&m, LOTA_Center, true, nameString, yellowInnerColor(appearAlpha),yellowOuterColor(appearAlpha));
    }
}

