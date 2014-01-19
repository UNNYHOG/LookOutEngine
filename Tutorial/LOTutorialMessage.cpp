//
//  LOTutorialMessage.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 17.11.13.
//
//

#include "LOTutorialMessage.h"
#include "LOGlobalVar.h"
#include "LookOutEngine.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"

void emptyRenderFunction(SunnyButton * btn);
void tutorialButtonPressed(SunnyButton *btn);

void LOTutorialMessage::buttonPressed(SunnyButton *btn)
{
    if (btn == showMeButton)
    {
        LOMenu::getSharedMenu()->closeTutorial();
        
        LOMenu::getSharedMenu()->showLevelCompleteButtons(false);
        loLoadEmptyMap();
        gamePaused = true;
        isSurvival = true;
        LOTasksView::getSharedView()->setVisible(true);
        LOMenu::getSharedMenu()->showSurvivalButtons(true);
    } else
        if (btn == laterButton)
        {
            LOMenu::getSharedMenu()->closeTutorial();
        }
}

LOTutorialMessage::LOTutorialMessage(const char * header, const char * text, const char * underHeader)
{
    setName(header);
    description = getLocalizedLabel(text);
    showMeText = getLocalizedLabel("ShowMe");
    laterText = getLocalizedLabel("Later");
    
    if (underHeader)
        secondHeader = getLocalizedLabel(underHeader);
    else
        secondHeader = 0;
    
    setVisible(true);
    
    const float distBetweenButtons = 500;
    showMeButton = sunnyAddButton(SunnyVector2D(mapSize.x/2 - distBetweenButtons/2/1920*mapSize.x,windowCenter.y-frameSize.y/2 + backBorderSize/4), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,okButtonSize*pixelToScale);
    sunnyButtonSetRenderFunc(showMeButton, emptyRenderFunction);
    sunnyButtonSetCallbackFunc(showMeButton, tutorialButtonPressed);
    showMeButton->matrix.pos.z = -2;
    
    laterButton = sunnyAddButton(SunnyVector2D(mapSize.x/2 + distBetweenButtons/2/1920*mapSize.x,windowCenter.y-frameSize.y/2 + backBorderSize/4), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,okButtonSize*pixelToScale);
    sunnyButtonSetRenderFunc(laterButton, emptyRenderFunction);
    sunnyButtonSetCallbackFunc(laterButton, tutorialButtonPressed);
    laterButton->matrix.pos.z = -2;
}

LOTutorialMessage::~LOTutorialMessage()
{
    delete []description;
    delete [] showMeText;
    delete []laterText;
    if (secondHeader)
        delete []secondHeader;
    sunnyRemoveButton(showMeButton);
    sunnyRemoveButton(laterButton);
}

void LOTutorialMessage::render()
{
    LOTutorial::render();
    
    SunnyMatrix m;
    
    if (secondHeader)
    {
        m = getScaleMatrix(0.4) * getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y*0.61,-1));
        LOFont::writeText(&m, LOTA_Center, true, secondHeader, yellowInnerColor(appearAlpha),yellowOuterColor(appearAlpha));
    }
    
    m = getScaleMatrix(0.4) * getTranslateMatrix(SunnyVector3D(mapSize.x*0.5,mapSize.y*0.43,-1));
    LOFont::writeTextInRect(&m, SunnyVector2D(mapSize.x*0.6,0), LOTA_Center, true, description,whiteInnerColor(appearAlpha),whiteOuterColor(appearAlpha));
    
    
    //Showme
    {
        bindGameGlobal();
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SHTextureBase::bindTexture(menuTexture);
        glUniform1f(uniformTSA_A, appearAlpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        
        float scale = appearAlpha/1.5;
        if (showMeButton->active) scale*=0.85;
        m = getScaleMatrix(scale);
        m.pos = showMeButton->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOkButtonBack_VAO);
        
        m = getScaleMatrix(0.55) * m;
        LOFont::writeText(&m, LOTA_Center, true, showMeText, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    }
    //Later
    {
        bindGameGlobal();
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SHTextureBase::bindTexture(menuTexture);
        glUniform1f(uniformTSA_A, appearAlpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        
        float scale = appearAlpha/1.5;
        if (laterButton->active) scale*=0.85;
        m = getScaleMatrix(scale);
        m.pos = laterButton->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOkButtonBack_VAO);
        
        m = getScaleMatrix(0.55) * m;
        LOFont::writeText(&m, LOTA_Center, true, laterText, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    }
}

