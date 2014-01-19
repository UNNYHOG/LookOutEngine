//
//  LOSettingsWindow.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 18.11.13.
//
//

#include "LOSettingsWindow.h"
#include "LOSettings.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"

void emptyRenderFunction(SunnyButton * btn);
void tutorialButtonPressed(SunnyButton *btn);

void LOSettingsWindow::buttonPressed(SunnyButton *btn)
{
    if (btn == buttons[SB_Sounds])
        LOSettings::switchSoundsVolume();
    else
        if (btn == buttons[SB_Music])
            LOSettings::switchMusicVolume();
        else
            if (btn == buttons[SB_LeftHand])
                LOSettings::changeLeftHanded();
            else
                if (btn == buttons[SB_Control])
                    LOSettings::changeControlType();
                else
                    if (btn == buttons[SB_Vibration])
                        LOSettings::changeVibrationSettings();
                    else
                        if (btn == buttons[SB_GameCenter])
                            showGameCenter();
#ifdef __ANDROID__
                        else
                            if (btn == buttons[SB_Leaderboard])
                                showLeaderboard();
#endif
    
    else
        if (btn == okButton)
        {
            LOSettings::saveSettings();
            LOMenu::getSharedMenu()->closeTutorial();
        }
}

LOSettingsWindow::LOSettingsWindow()
{
    setName("SETTINGS",0.65);
    
    setVisible(true);

    SunnyVector2D btnSize(190/2,170/2);
    buttons[SB_Sounds] = sunnyAddButton(PixelToWorld(522,512), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(190,170)/1.5*pixelToScale);
    buttons[SB_LeftHand] = sunnyAddButton(PixelToWorld(1272,510), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(450,130)/1.5*pixelToScale);
    buttons[SB_Vibration] = sunnyAddButton(PixelToWorld(526,748), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(280,250)/1.5*pixelToScale);
    buttons[SB_Control] = sunnyAddButton(PixelToWorld(1252,766), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(520,200)/1.5*pixelToScale);
    buttons[SB_GameCenter] = sunnyAddButton(PixelToWorld(828,754), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(190,170)/1.5*pixelToScale);
    buttons[SB_Music] = sunnyAddButton(PixelToWorld(826,516), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(190,170)/2*pixelToScale);
#ifndef __ANDROID__
    buttons[SB_Leaderboard] = sunnyAddButton(PixelToWorld(828,754), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(190,170)/1.5*pixelToScale);
#endif
    
    for (short i = 0;i<SB_Count;i++)
    {
        sunnyButtonSetCallbackFunc(buttons[i], tutorialButtonPressed);
        sunnyButtonSetRenderFunc(buttons[i], emptyRenderFunction);
        buttons[i]->matrix.pos.z = -2;
    }
    
    windowCenter = PixelToWorld(960,600);
    namePosition.y += 40./1920*mapSize.x;
    
    frameSize = SunnyVector2D(1452./1920*mapSize.x,912./1280*mapSize.y);
    addStandartOkButtonToClose();
}

LOSettingsWindow::~LOSettingsWindow()
{
    for (short i = 0;i<SB_Count;i++)
        sunnyRemoveButton(buttons[i]);
    
    LOSettings::saveSettings();
}

void LOSettingsWindow::render()
{
    LOTutorial::render();
    
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, appearAlpha);
    float btnScale = 1;
    SunnyMatrix m = sunnyIdentityMatrix;
    
    const float smallValue = 0.01;
    const float mapKoef = mapSize.x/1920;
    //Sounds
    m.pos = buttons[SB_Sounds]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    buttons[SB_Sounds]->active?btnScale=defaultButtonScale:btnScale=1;
    if (LOSettings::getSoundsVolume()>smallValue)
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOSounds_VAO);
    } else
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, 0, btnScale, btnScale));
        SunnyDrawArrays(LOSounds_VAO+1);
    }
    
    //LeftHand
    m.pos = buttons[SB_LeftHand]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    buttons[SB_LeftHand]->active?btnScale=defaultButtonScale:btnScale=1;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
    if (LOSettings::isLeftHanded())
        SunnyDrawArrays(LOLeftRightHand_VAO);
    else
        SunnyDrawArrays(LOLeftRightHand_VAO+1);
    
    //Vibration
    m.pos = buttons[SB_Vibration]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    buttons[SB_Vibration]->active?btnScale=defaultButtonScale:btnScale=1;
    if (LOSettings::canVibrate())
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOVibration_VAO);
    } else
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, -10*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOVibration_VAO+1);
    }
    
    //Control
    m.pos = buttons[SB_Control]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    buttons[SB_Control]->active?btnScale=defaultButtonScale:btnScale=1;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
    if (controlType == LOC_Accelerometer)
        SunnyDrawArrays(LOJoysticAcc_VAO);
    else
        SunnyDrawArrays(LOJoysticAcc_VAO+1);
    
    //GameCenter
    m.pos = buttons[SB_GameCenter]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    buttons[SB_GameCenter]->active?btnScale=defaultButtonScale:btnScale=1;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
    SunnyDrawArrays(LOGameCenter_VAO);
    
#ifndef __ANDROID
    //Leader
    m.pos = buttons[SB_Leaderboard]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    buttons[SB_Leaderboard]->active?btnScale=defaultButtonScale:btnScale=1;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
    SunnyDrawArrays(LOGameCenter_VAO+1);
#endif
    
    //Music
    m.pos = buttons[SB_Music]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    buttons[SB_Music]->active?btnScale=defaultButtonScale:btnScale=1;
    if (LOSettings::getMusicVolume()>smallValue)
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOMusicButtonOn_VAO);
    } else
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(2*mapKoef,0, btnScale, btnScale));
        SunnyDrawArrays(LOMusicButtonOn_VAO+1);
    }
}