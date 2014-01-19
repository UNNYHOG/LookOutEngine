//
//  LOMessageWindow.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 13.10.13.
//
//

#include "LOMessageWindow.h"
#include "iPhone_Strings.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "LOFont.h"
#include "LOTutorial.h"

LOMessageWindow * sharedMessage = 0;

void loMessageButtonPressed(SunnyButton *btn)
{
    sharedMessage->buttonPressed(btn);
}

void loMessageButtonRender(SunnyButton *btn)
{
    //sharedLevelComplete->buttonRenderCallback(btn);
}

void LOMessageWindow::buttonPressed(SunnyButton *btn)
{
    if (btn == buttons[0])
    {
        setVisible(false);
        if (callbacks[0].callbackFunction)
            callbacks[0].callbackFunction(callbacks[0].callbackParameter);
    } else
    {
        for (short i = 1;i<buttonsCount;i++)
        if (btn == buttons[i])
        {
            if (callbacks[i].callbackFunction)
                callbacks[i].callbackFunction(callbacks[i].callbackParameter);
            break;
        }
        setVisible(false);
        return;
    }
    if (messageType == LOMessage_NoGold)
    {
        if (!LOStore::getSharedStore())
        {
            LOMenu::getSharedMenu()->showSurvivalButtons(false);
            LOMenu::getSharedMenu()->showStoreMenu(true,StoreBack_Survival);
        }
        
        LOStore::getSharedStore()->scroll->setCurrentPage(LOStore_GoldCoins,true);
    }
}

void LOMessageWindow::addButton(const char* text, void (*func)(SunnyButton*),SunnyButton * btn)
{
    buttons = (SunnyButton**)realloc(buttons,sizeof(SunnyButton*)*(buttonsCount+1));
    buttonsText = (char**)realloc(buttonsText,sizeof(char*)*(buttonsCount+1));
    callbacks = (ButtonCallBack*)realloc(callbacks,sizeof(ButtonCallBack)*(buttonsCount+1));
    
    callbacks[buttonsCount].callbackFunction = func;
    callbacks[buttonsCount].callbackParameter = btn;
    buttonsText[buttonsCount] = getLocalizedLabel(text);
    
    float btnSize = 340/2;
    const float distBetweenButtons = 500;
    buttons[buttonsCount] = sunnyAddButton(SunnyVector2D(mapSize.x/2 + distBetweenButtons/2/1920*mapSize.x,buttons[0]->matrix.pos.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize/2)*pixelToScale);
    buttons[0]->matrix.pos.x = mapSize.x/2 - distBetweenButtons/2/1920.*mapSize.x;
    
    sunnyButtonSetCallbackFunc(buttons[buttonsCount], loMessageButtonPressed);
    sunnyButtonSetRenderFunc(buttons[buttonsCount], loMessageButtonRender);
    
    buttonsCount++;
}

void LOMessageWindow::setCallback(void (*func)(SunnyButton*),SunnyButton * btn, short buttonNum)
{
    callbacks[buttonNum].callbackFunction = func;
    callbacks[buttonNum].callbackParameter = btn;
}

LOMessageWindow::LOMessageWindow()
{
    sharedMessage = this;
    isVisible = false;
    text = 0;
    appearAlpha = 0;
    frameSize = SunnyVector2D(mapSize.x*0.7,mapSize.y*0.5);
    
    callbacks = (ButtonCallBack*)malloc(sizeof(ButtonCallBack));
    callbacks[0].callbackFunction = 0;
    callbacks[0].callbackParameter = 0;
    
    buttons = (SunnyButton**)malloc(sizeof(SunnyButton*));
    buttonsText = (char**)malloc(sizeof(char*));
    float btnSize = 340/2;
    buttons[0] = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapSize.y/2-frameSize.y/2 + backBorderSize/4), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize/2)*pixelToScale);
    
    sunnyButtonSetCallbackFunc(buttons[0], loMessageButtonPressed);
    sunnyButtonSetRenderFunc(buttons[0], loMessageButtonRender);
    buttons[0]->matrix.pos.z = -2;
    
    buttonsCount = 1;
    
    buttons[0]->enable = false;
    
    buttonsText[0] = getLocalizedLabel("Ok");
}

void LOMessageWindow::changeButtonText(const char* newText)
{
    delete []buttonsText[0];
    buttonsText[0] = getLocalizedLabel(newText);
}

LOMessageWindow::~LOMessageWindow()
{
    if (text)
        delete []text;
    
    for (short i = 0;i<buttonsCount;i++)
        sunnyRemoveButton(buttons[i]);
    free(buttons);
}

void LOMessageWindow::setText(const char * string)
{
    if (text)
        delete []text;
    
    text = getLocalizedLabel(string);
}

void LOMessageWindow::setVisible(bool visible,LOMessageType type)
{
    if (isVisible == visible) return;
    
    isVisible = visible;
    appearAlpha = 0;
    
    if (isVisible)
    {
        messageType = type;
        gameWasPaused = gamePaused;
        gamePaused = true;
        sunnyTemproraryDisableAllButtons();
        callbacks[0].callbackFunction = 0;
        callbacks[0].callbackParameter = 0;
    }
    else
    {
        for (short i = 1;i<buttonsCount;i++)
        {
            sunnyRemoveButton(buttons[i]);
            delete []buttonsText[i];
        }
        buttons = (SunnyButton**)realloc(buttons,sizeof(SunnyButton*));
        buttonsText = (char**)realloc(buttonsText,sizeof(char*));
        callbacks = (ButtonCallBack*)realloc(callbacks,sizeof(ButtonCallBack));
        buttonsCount = 1;
        changeButtonText("Ok");
        buttons[0]->matrix.pos.x = mapSize.x/2;
        
        gamePaused = gameWasPaused;
        sunnyReenableAllButtons();
    }
    
    for (short i = 0;i<buttonsCount;i++)
        buttons[i]->enable = isVisible;
}

void LOMessageWindow::render()
{
    if (!isVisible) return;
    
    appearAlpha+=deltaTime*5;
    if (appearAlpha>1) appearAlpha = 1;
    
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
    glUniform1f(uniformTSA_A, appearAlpha);
    float scale = 2;
    LOTutorial::renderBackForSize(frameSize,mapSize/2,appearAlpha);
    
    //Buttons
    m = getTranslateMatrix(SunnyVector3D(0,0,farDist));
    glUniform1f(uniformTSA_A, appearAlpha);
    float btnScale;
    SunnyMatrix rot = m;
    for (short i = 0;i<buttonsCount;i++)
    {
        buttons[i]->active?btnScale = defaultButtonScale:btnScale = 1;
        btnScale/=1.5;
        rot.pos = buttons[i]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOOkButtonBack_VAO);
    }
    
    m = getScaleMatrix(0.7)*m;
    for (short i = 0;i<buttonsCount;i++)
    {
        rot = m;
        rot.pos = buttons[i]->matrix.pos;
        LOFont::writeText(&rot, LOTA_Center, true, buttonsText[i], SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    }
    scale = 0.5;
    m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(mapSize.x/2, mapSize.y/2,farDist));
    LOFont::writeTextInRect(&m, SunnyVector2D(mapSize.x*0.6,mapSize.y/3), LOTA_Center, true, text, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
}


