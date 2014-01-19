//
//  LOPause.cpp
//  LookOut
//
//  Created by Ignatov on 05.10.13.
//
//

#include "LOPause.h"
#include "SHTextureBase.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "iPhone_Strings.h"
#include "LOFont.h"
#include "LOScore.h"
#include "LookOutEngine.h"
#include "LOTasksView.h"

LOPause * sharedPauseWindow = 0;

void loPButtonPressed(SunnyButton *btn)
{
    sharedPauseWindow->buttonPressed(btn);
}

void loPButtonRender(SunnyButton *btn)
{
    //sharedLevelComplete->buttonRenderCallback(btn);
}

void LOPause::hideWindow()
{
    buttonPressed(buttons[LOPB_Next]);
}

void LOPause::buttonPressed(SunnyButton *btn)
{
    if (buttons[LOPB_MainMenu] == btn)
    {
        loLoadEmptyMap();
        gamePaused = true;
        
        LOMenu::getSharedMenu()->showGamePlayButtons(false);
        if (isSurvival)
            LOMenu::getSharedMenu()->showSurvivalButtons(true);
        else
            LOMenu::getSharedMenu()->showLevelsButtons(true);
    } else
        if (buttons[LOPB_Replay] == btn)
        {
            resetType = RT_Pause;
        } else
            if (buttons[LOPB_Next] == btn)
            {
                LOMenu::getSharedMenu()->showGamePlayButtons(false);
                gamePaused = false;
                LOMenu::getSharedMenu()->showGameSpellButtons(true);
            } else
                if (buttons[LOPB_Settings] == btn)
                {
                    LOMenu::getSharedMenu()->showSettingsWindow();
                } else
                    if (buttons[LOPB_Store])
                    {
                        LOMenu::getSharedMenu()->showGamePlayButtons(false);
                        LOMenu::getSharedMenu()->showStoreMenu(true, StoreBackPause);
                    }
}

LOPause::LOPause()
{
    isVisible = false;
    sharedPauseWindow = this;
    
    float btnSize = 340/2;
    buttons[LOPB_MainMenu] = sunnyAddButton(SunnyVector2D(202/1920.*mapSize.x-horizontalMapAdd,(1280-588)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOPB_Store] = sunnyAddButton(SunnyVector2D(202/1920.*mapSize.x-horizontalMapAdd,(1280-966)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOPB_Replay] = sunnyAddButton(SunnyVector2D(1716/1920.*mapSize.x+horizontalMapAdd,(1280-588)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOPB_Next] = sunnyAddButton(SunnyVector2D(1718/1920.*mapSize.x+horizontalMapAdd,(1280-966)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    //buttons[LOPB_Control] = sunnyAddButton(SunnyVector2D(792/1920.*mapSize.x,(1280-966)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOPB_Settings] = sunnyAddButton(PixelToWorld(960, 966), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale*smallButtonsScale);
    //buttons[LOPB_Music] = sunnyAddButton(SunnyVector2D(1136/1920.*mapSize.x,(1280-966)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    for (short i = 0;i<LOPB_Count;i++)
    {
        sunnyButtonSetCallbackFunc(buttons[i], loPButtonPressed);
        sunnyButtonSetRenderFunc(buttons[i], loPButtonRender);
        buttons[i]->tag = i+1;
        buttons[i]->matrix.pos.z = -2;
        buttons[i]->hidden = true;
    }
    
    labelPause = getLocalizedLabel("Pause");
    labelLevel = getLocalizedLabel("Level");
    
    levelLength = LOFont::getTextSize(labelLevel);
    
    labelEpisode = NULL;
}

LOPause::~LOPause()
{
    delete [] labelPause;
    delete [] labelLevel;
    if (labelEpisode)
        delete [] labelEpisode;
    
    for (short i = 0;i<LOPB_Count;i++)
        sunnyRemoveButton(buttons[i]);
}

SunnyButton * LOPause::getStoreButton()
{
    return buttons[LOPB_Store];
}

void LOPause::update()
{
    if (!isVisible) return;
    
    appearAlpha+=deltaTime*5;
    if (appearAlpha>1)
        appearAlpha = 1;
}

void LOPause::render()
{
    if (!isVisible) return;
    
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);

    const float mapKoef = mapSize.x/1920;
    const float farDist = -20;
    //fade
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    SunnyMatrix m;
    m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y/2,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform1f(uniformTSA_A, 0.5*appearAlpha);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    SunnyDrawArrays(LODeath_VAO+1);
    
    //Buttons
    {
        //Main
        m = getTranslateMatrix(SunnyVector3D(0,0,farDist));
        glUniform1f(uniformTSA_A, appearAlpha);
        float btnScale;
        SunnyMatrix rot;
        rot = m;
        rot.pos = buttons[LOPB_MainMenu]->matrix.pos;
        buttons[LOPB_MainMenu]->active?btnScale = defaultButtonScale:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, 6*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+1);
        
        //Store
        buttons[LOPB_Store]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = buttons[LOPB_Store]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-5*mapKoef, -8*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+2);
        
        //Replay
        if (!buttons[LOPB_Replay]->hidden)
        {
            buttons[LOPB_Replay]->active?btnScale = defaultButtonScale:btnScale = 1;
            rot.pos = buttons[LOPB_Replay]->matrix.pos;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
            SunnyDrawArrays(LOLCButtonsVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, 4*mapKoef, btnScale, btnScale));
            SunnyDrawArrays(LOLCButtonsVAO+3);
        }
        
        //Settings
        buttons[LOPB_Settings]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = buttons[LOPB_Settings]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale*smallButtonsScale, btnScale*smallButtonsScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-6*mapKoef, 6*mapKoef, btnScale*smallButtonsScale, btnScale*smallButtonsScale));
        SunnyDrawArrays(LOSettingsButton_VAO);
        
        //Next
        buttons[LOPB_Next]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = buttons[LOPB_Next]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        btnScale *= (1.05 - 0.05*cos(2*getCurrentTime()));
        glUniform1f(uniformTSA_A, 1-(btnScale-1)*2);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(8*mapKoef, 4*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+4);
    }
    
    float scale = 0;
    const float middleDist = levelLength/2;
    const float afterDist = middleDist/2;
    float len = 0;
    //Pause Label
    if (!isSurvival)
        m = getScaleMatrix(SunnyVector3D(0.93)) * getTranslateMatrix(SunnyVector3D(960*mapKoef,(1280-166)*mapKoef,farDist));
    else
        m = getScaleMatrix(SunnyVector3D(0.93)) * getTranslateMatrix(SunnyVector3D(960*mapKoef,(1280-118)*mapKoef,farDist));
    LOFont::writeText(&m, LOTA_Center, true, labelPause, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    if (!isSurvival)
    {
        len = levelLength + episodeLength + middleDist + afterDist + numberLength;
        scale = 0.5;
        m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef - len/2*scale,(1280-328)*mapKoef,farDist));
        LOFont::writeText(&m, LOTA_LeftCenter, true, labelEpisode, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
        
        m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef + (-len/2 + episodeLength + middleDist)*scale,(1280-328)*mapKoef,farDist));
        LOFont::writeText(&m, LOTA_LeftCenter, true, labelLevel, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    }
    
    bindGameGlobal();
    if (!isSurvival)
    {
        m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef + (-len/2 + episodeLength + middleDist/3)*scale,(1280-328)*mapKoef,farDist));
        LOFont::writeTime(&m, true, ":",SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
        
        m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef + (-len/2 + episodeLength + middleDist + levelLength + afterDist)*scale,(1280-328)*mapKoef,farDist));
        LOFont::writeNumber(&m, true, selectedLevelNum+1,SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    }
    
    scale = 0.79;
    SunnyVector2D coinsPos;//x - xpos, y - deltaY
    if (!isSurvival)
        coinsPos = SunnyVector2D(960,0)*mapKoef;
    else
    {
        coinsPos = SunnyVector2D(1398,196)*mapKoef;
    }
    
    if (LOScore::getTotalCoins())
    {
        m = getScaleMatrix(SunnyVector3D(scale))*getTranslateMatrix(SunnyVector3D(coinsPos.x,coinsPos.y + (1280-678)*mapKoef,-1));
        loRenderPriceLabel(&m, LOPT_GoldCoins, (int)LOScore::getTotalCoins(),appearAlpha);
    }
    
    if (LOScore::getTotalSnowflakes())
    {
        m = getScaleMatrix(SunnyVector3D(scale))*getTranslateMatrix(SunnyVector3D(coinsPos.x,coinsPos.y + (1280-512)*mapKoef,-1));
        loRenderPriceLabel(&m, LOPT_VioletSnowflakes, (int)LOScore::getTotalSnowflakes(),appearAlpha);
    }
}

void LOPause::setVisible(bool visible)
{
    if (isVisible == visible)
        return;
    
    isVisible = visible;
    
    for (short i = 0;i<LOPB_Count;i++)
        buttons[i]->hidden = !visible;
    
    if (isVisible)
    {
        if (labelEpisode)
            delete [] labelEpisode;
        
        char ep[32];
        strcpy(ep, "Episode.");
        int len = (int)strlen(ep);
        ep[len] = '1' + selectedStepNum;
        ep[len+1] = '\0';
        
        labelEpisode = getLocalizedLabel(ep);
        
        episodeLength = LOFont::getTextSize(labelEpisode);
        
        numberLength = LOFont::getNumberSize(selectedLevelNum+1);
        
        appearAlpha = 0;
    }
    
    const float mapKoef = mapSize.x/1920;
    if (isSurvival)
    {
        LOTasksView::getSharedView()->setVisible(visible);
        
        if (isVisible)
        {
            buttons[LOPB_Replay]->hidden = true;
            
            sunnyButtonSetPosition(buttons[LOPB_MainMenu], SunnyVector2D(960*mapKoef,(1280-1076)*mapKoef));
            sunnyButtonSetPosition(buttons[LOPB_Store], SunnyVector2D(206*mapKoef-horizontalMapAdd,(1280-1076)*mapKoef));
            sunnyButtonSetPosition(buttons[LOPB_Next], SunnyVector2D(1716*mapKoef+horizontalMapAdd,(1280-1076)*mapKoef));
            sunnyButtonSetPosition(buttons[LOPB_Settings], PixelToWorld(1400, 710));
        }
    } else
    {
        if (isVisible)
        {
            buttons[LOPB_Replay]->hidden = false;
            
            sunnyButtonSetPosition(buttons[LOPB_MainMenu], SunnyVector2D(202*mapKoef-horizontalMapAdd,(1280-588)*mapKoef));
            sunnyButtonSetPosition(buttons[LOPB_Store], SunnyVector2D(202*mapKoef-horizontalMapAdd,(1280-966)*mapKoef));
            sunnyButtonSetPosition(buttons[LOPB_Next], SunnyVector2D(1718*mapKoef+horizontalMapAdd,(1280-966)*mapKoef));
            sunnyButtonSetPosition(buttons[LOPB_Settings], PixelToWorld(960, 966));
        }
    }
}



