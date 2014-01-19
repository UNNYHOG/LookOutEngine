//
//  LOMainWindow.cpp
//  LookOut
//
//  Created by Ignatov on 10.10.13.
//
//

#include "LOMainWindow.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "iPhone_Strings.h"
#include "LookOutEngine.h"

LOMainWindow * sharedMainWindow = NULL;

float mainMenuSnowballScale = 7;

const float mwSnowballAppearTime = 0.7;

const float someButtonsScale = 0.6;

SunnyVector2D snowballMenuCenter = SunnyVector2D(mapSize.x/2,(640-450)/640.*mapSize.y);
SunnyVector2D snowballMenuAppear = SunnyVector2D(-5,(640-450)/640.*mapSize.y);

void loMWButtonPressed(SunnyButton *btn)
{
    sharedMainWindow->buttonPressed(btn);
}

void loMWButtonRender(SunnyButton *btn)
{
    //sharedLevelComplete->buttonRenderCallback(btn);
}

void LOMainWindow::buttonPressed(SunnyButton *btn)
{
    if (btn == buttons[LOMWB_Play])
    {
//        LOMenu::getSharedMenu()->showSurvivalTutorial();
//        return;
        isSurvival = false;
        LOMenu::getSharedMenu()->showMainMenu(false);
        
        if (LOScore::getStageCoins(0)==0)
        {
            selectedLevelNum = selectedLevelNum = 0;
            loLoadMap(selectedStepNum*loLevelsPerStep + selectedLevelNum);
            LOMenu::getSharedMenu()->showGameSpellButtons(true);
        } else
            LOMenu::getSharedMenu()->showLevelsButtons(true);
    } else
        if (btn == buttons[LOMWB_PlaySurvival])
        {
            isSurvival = true;
            LOTasksView::getSharedView()->setVisible(true);
            LOMenu::getSharedMenu()->showMainMenu(false);
            LOMenu::getSharedMenu()->showSurvivalButtons(true);
        } else
            if (btn == buttons[LOMWB_Puzzle])
            {
                LOMenu::getSharedMenu()->showMainMenu(false);
                LOMenu::getSharedMenu()->showPuzzleMenu(true);
            } else
                if (btn==buttons[LOMWB_Store])
                {
                    LOMenu::getSharedMenu()->showMainMenu(false);
                    LOMenu::getSharedMenu()->showStoreMenu(true,StoreBack_MainMenu);
                } else
                    if (btn==buttons[LOMWB_Settings])
                    {
                        LOMenu::getSharedMenu()->showSettingsWindow();
                    }
}

LOMainWindow * LOMainWindow::getSharedWindow()
{
    return sharedMainWindow;
}

LOMainWindow::LOMainWindow()
{
    snowballMenuCenter.y -= verticalMapAdd;
    if (verticalMapAdd>0.1)
        mainMenuSnowballScale+=2;
    sharedMainWindow = this;
    float btnSize = 340/2;
    const float mapKoef = mapSize.x/1920;
    float smallButtonY = (1280.-1150)*mapKoef-verticalMapAdd;
    float distX = 230*mapKoef;
    float leftx = 150*mapKoef - horizontalMapAdd;
    buttons[LOMWB_Play] = sunnyAddButton(snowballMenuCenter, objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale*mainMenuSnowballScale/5);
    buttons[LOMWB_Settings] = sunnyAddButton(SunnyVector2D(leftx+1*distX,smallButtonY), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*someButtonsScale*pixelToScale);
    buttons[LOMWB_Store] = sunnyAddButton(SunnyVector2D(leftx+0*distX,smallButtonY), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale*someButtonsScale);
    buttons[LOMWB_PlaySurvival] = sunnyAddButton(SunnyVector2D(1718*mapKoef + horizontalMapAdd,(1280.-1076)*mapKoef-verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOMWB_Puzzle] = sunnyAddButton(SunnyVector2D(leftx+2*distX,smallButtonY), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale*someButtonsScale);
    for (short i = 0;i<LOMWB_Count;i++)
    {
        sunnyButtonSetCallbackFunc(buttons[i], loMWButtonPressed);
        sunnyButtonSetRenderFunc(buttons[i], loMWButtonRender);
        buttons[i]->matrix.pos.z = -2;
//        buttons[i]->hidden = true;
    }
    
    isVisible = false;
}

LOMainWindow::~LOMainWindow()
{
    for (short i = 0;i<LOMWB_Count;i++)
        sunnyRemoveButton(buttons[i]);
}

void LOMainWindow::setVisible(bool visible)
{
    if (isVisible == visible)
        return;
    
    isVisible = visible;
    
    for (short i = 0;i<LOMWB_Count;i++)
        buttons[i]->hidden = !visible;
    
    for (short i = 0;i<3;i++)
        overlayParameters[i] = SunnyVector2D((2+sRandomf())/2,M_PI*2*sRandomf());
    
    mainWindowTime = 0;
    playScale = 0;
    
    if (visible)
    {
        buttons[LOMWB_PlaySurvival]->hidden = !LOScore::isStageEnabled(0);
        buttons[LOMWB_Puzzle]->hidden = !LOScore::isAnyPuzzle();
    }
    
    update();
}

void LOMainWindow::update()
{
    if (!isVisible) return;
    mainWindowTime += deltaTime;
    
    float t = mainWindowTime/mwSnowballAppearTime;
    if (t>1)
    {
        t = 1;
        playScale = fminf((mainWindowTime-mwSnowballAppearTime)/0.3,1);
    }
    SunnyVector2D pos = snowballMenuAppear*(1-t) + t*snowballMenuCenter;
    snowballMatrix = getScaleMatrix(mainMenuSnowballScale) * getRotationZMatrix(M_PI*2*(1-t)) * getTranslateMatrix(SunnyVector3D(pos.x,pos.y,-10));
}

void LOMainWindow::renderOverlay()
{
    if (!isVisible) return;
    
    const float mapKoef = mapSize.x/960;
    SunnyMatrix rot = sunnyIdentityMatrix;
    rot.pos.z = -1;
    glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(rot.front.x));
    
    float scale;
    scale = 2.5 + 0.5*sinf(overlayParameters[0].x*mainWindowTime + overlayParameters[0].y);
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(66*mapKoef,(640-332)*mapKoef,scale,scale));
    SunnyDrawArrays(LOOverlayFireball_VAO);
//    scale = 2 + 0.5*sinf(overlayParameters[1].x*mainWindowTime + overlayParameters[1].y);
//    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(670*mapKoef,(640-446)*mapKoef,scale,scale));
//    SunnyDrawArrays(LOOverlayFireball_VAO);
    scale = 2.5 + 0.5*sinf(overlayParameters[2].x*mainWindowTime + overlayParameters[2].y);
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(870*mapKoef,(640-168)*mapKoef,scale,scale));
    SunnyDrawArrays(LOOverlayFireball_VAO);
    
    scale = mainMenuSnowballScale*0.8;
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(snowballMatrix.pos.x,snowballMatrix.pos.y,scale,scale));
    SunnyDrawArrays(LOOverlaySnowflake_VAO);
}

void LOMainWindow::renderPlayButton()
{
    if (!isVisible || LOMenu::getSharedMenu()->isTutorial())
        return;
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    float btnScale;
    SunnyMatrix rot = sunnyIdentityMatrix;
    float t = 0.2*sinf(mainWindowTime*2);
    glUniform1f(uniformTSA_A, (1 + t)*playScale);
    t = 0.8*(1-t/2);
    buttons[LOMWB_Play]->active?btnScale = defaultButtonScale:btnScale = 1;
    rot.pos = buttons[LOMWB_Play]->matrix.pos;
    btnScale*=t*mainMenuSnowballScale/7*playScale;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0.2, 0, btnScale, btnScale));
    SunnyDrawArrays(LOLSPlayButtonVAO);
}

void LOMainWindow::render()
{
    if (!isVisible) return;
    
    const float mapKoef = mapSize.x/1920;
    const float farDist = -20;
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    SunnyMatrix m;
    glUniform1f(uniformTSA_A, 1);
    m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,(1280-870-422/2)*mapKoef,farDist));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,-verticalMapAdd,(mapZoomBounds.y-mapZoomBounds.x)/mapSize.x,1));
    SunnyDrawArrays(LOLCForest_VAO);
    
    SHTextureBase::bindTexture(puzzleTextureID);
    m = getRotationZMatrix(-M_PI_2) * getTranslateMatrix(SunnyVector3D(mapSize.x/2,(1280-600+40)*mapKoef,farDist));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    SunnyDrawArrays(LOMainMenu_VAO);
    
    m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,(1280-1016)*mapKoef,farDist));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,-verticalMapAdd,(mapZoomBounds.y-mapZoomBounds.x)/mapSize.x,1));
    SunnyDrawArrays(LOMainMenu_VAO+1);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
//    rot = getScaleMatrix(4) * getRotationZMatrix(-(mapZoomBounds.y-mapZoomBounds.x)*d/4) * getTranslateMatrix(SunnyVector3D(mapZoomBounds.x + (mapZoomBounds.y-mapZoomBounds.x)*d,mapZoomBounds.z+2 + sinf(d*M_PI),-5));
    //losSetRollingSnowballSpeed(sinf(d*M_PI) + 0.5);
    LOPlayer::renderForMenu(&snowballMatrix,1);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    float btnScale;
    SunnyMatrix rot = sunnyIdentityMatrix;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    //Buttons
    {
        //Play
        if (!isFastDevice)
        {
            renderPlayButton();
            glUniform1f(uniformTSA_A, 1);
        }
        
        //Store
        buttons[LOMWB_Store]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = buttons[LOMWB_Store]->matrix.pos;
        btnScale *= someButtonsScale;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, -4*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+2);
        
        //Puzzle
        if (!buttons[LOMWB_Puzzle]->hidden)
        {
            buttons[LOMWB_Puzzle]->active?btnScale = defaultButtonScale:btnScale = 1;
            rot.pos = buttons[LOMWB_Puzzle]->matrix.pos;
            btnScale *= someButtonsScale;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
            SunnyDrawArrays(LOLCButtonsVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0*mapKoef, 6*mapKoef, btnScale/someButtonsScale, btnScale/someButtonsScale));
            SunnyDrawArrays(LOPuzzleButton_VAO);
        }
        
        //Survival
        if (!buttons[LOMWB_PlaySurvival]->hidden)
        {
            buttons[LOMWB_PlaySurvival]->active?btnScale = defaultButtonScale:btnScale = 1;
            rot.pos = buttons[LOMWB_PlaySurvival]->matrix.pos;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
            SunnyDrawArrays(LOLCButtonsVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0*mapKoef, 0*mapKoef, btnScale, btnScale));
            SunnyDrawArrays(LOSurvival_VAO);
        }
        
        //Settings
        buttons[LOMWB_Settings]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = buttons[LOMWB_Settings]->matrix.pos;
        btnScale *= someButtonsScale;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-8*mapKoef, 4*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOSettingsButton_VAO);
    }
}