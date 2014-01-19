//
//  LOLevelComplete.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 01.10.13.
//
//

#include "LOLevelComplete.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"
#include "LOFont.h"
#include "iPhone_Strings.h"
#include "LookOutEngine.h"
#include "LOSounds.h"

const SunnyVector2D loLCBackgroundAnimation(0.3,0.5);
const SunnyVector2D loLCTreesAnimation(loLCBackgroundAnimation.y,loLCBackgroundAnimation.y+0.2);
const SunnyVector2D loLCLevelCompleteAnimation(loLCTreesAnimation.y,loLCTreesAnimation.y+0.2);
const SunnyVector2D loLCTopTableAnimation(loLCLevelCompleteAnimation.y+0.1,loLCLevelCompleteAnimation.y+.25);
const SunnyVector2D loLCTimerAnimation(loLCTopTableAnimation.y+0.1,loLCTopTableAnimation.y+0.2);
const SunnyVector2D loLCTimeValueAnimation(loLCTimerAnimation.y+0.1,loLCTimerAnimation.y+.4);
const SunnyVector2D loLCCoinAnimation(loLCTimeValueAnimation.y+0.1,loLCTimeValueAnimation.y+.5);
float loLCCoinsAnimationFinished;
const SunnyVector2D loLCBottomTableAnimation(0.1,0.1+loLCTopTableAnimation.y-loLCTopTableAnimation.x);
const SunnyVector2D loLCPerfectAnimation(loLCBottomTableAnimation.y+0.1,loLCBottomTableAnimation.y+.1);
float loLCNextCoinAnimationFinished;
const SunnyVector2D loLCButtonsAnimation(0.1,0.3);
const SunnyVector2D loLCSnowballAnimation(loLCButtonsAnimation.y+0.1,loLCButtonsAnimation.y+1.6);

LOLevelComplete * sharedLevelComplete = NULL;

float loLCTableWidth = 0;

void loLCButtonPressed(SunnyButton *btn)
{
    sharedLevelComplete->buttonPressed(btn);
}

void loLCButtonRender(SunnyButton *btn)
{
    //sharedLevelComplete->buttonRenderCallback(btn);
}

void LOLevelComplete::setTableWidth(float value)
{
    loLCTableWidth = value;
}

void LOLevelComplete::buttonPressed(SunnyButton *btn)
{
    //prepareWindow(26,fminf(btn->tag, 3),25,20);
    if (btn == buttons[LOLCB_Share])
    {
        char ep[32];
        strcpy(ep, "Episode.");
        int len = (int)strlen(ep);
        ep[len] = '1' + selectedStepNum;
        ep[len+1] = '\0';
        char * episodeName = getLocalizedLabel((const char*)ep);
        char * messageName = getLocalizedLabel("FACEBOOK_LC_NAME");
        char * messageCaption = getLocalizedLabel("FACEBOOK_LC_CAPTION");
        char * messageDesc = getLocalizedLabel("FACEBOOK_LC_DESC");
        
        loShareFacebookLC(selectedLevelNum+1, episodeName, levelTime, messageName, messageCaption, messageDesc);
        delete []episodeName;
        delete []messageName;
        delete []messageCaption;
        delete []messageDesc;
    } else
    if (buttons[LOLCB_Skip] == btn)
    {
        animationTime = loLCNextCoinAnimationFinished + loLCSnowballAnimation.x;
    } else
    if (buttons[LOLCB_MainMenu] == btn)
    {
        LOMenu::getSharedMenu()->showLevelCompleteButtons(false);
        
        loLoadEmptyMap();
        gamePaused = true;
        LOMenu::getSharedMenu()->showLevelsButtons(true);
    } else
        if (buttons[LOLCB_Replay] == btn)
        {
            resetType = RT_LevelComplete;
        } else
            if (buttons[LOLCB_Store] == btn)
            {
                LOMenu::getSharedMenu()->showStoreMenu(true, StoreBackLevelComplete);
                //LOMenu::getSharedMenu()->showLevelCompleteButtons(false);
                isVisible = false;
            } else
            if (buttons[LOLCB_Next] == btn)
            {
                selectedLevelNum++;
                if (selectedLevelNum>=loLevelsPerStep)
                {
                    if (selectedStepNum>=loStepsCount-1)
                    {
                        buttonPressed(buttons[LOLCB_MainMenu]);
                        LOMenu::setCurrentPage(selectedStepNum+1);
                        selectedLevelNum--;
                        return;
                    } else
                    {
                        selectedLevelNum = 0;
                        selectedStepNum++;
                        LOMenu::setCurrentPage(selectedStepNum);
                    }
                }
                loLoadMap(selectedStepNum*loLevelsPerStep + selectedLevelNum);
                
                LOMenu::getSharedMenu()->showLevelCompleteButtons(false);
                LOMenu::getSharedMenu()->showGameSpellButtons(true);
            }
}

LOLevelComplete::LOLevelComplete()
{
    isVisible = false;
    sharedLevelComplete = this;
    
    float btnSize = 340/2;
    buttons[LOLCB_MainMenu] = sunnyAddButton(SunnyVector2D(220/1920.*mapSize.x-horizontalMapAdd,(1280-492)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOLCB_Store] = sunnyAddButton(SunnyVector2D(225/1920.*mapSize.x-horizontalMapAdd,(1280-858)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOLCB_Replay] = sunnyAddButton(SunnyVector2D(1714/1920.*mapSize.x+horizontalMapAdd,(1280-490)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOLCB_Next] = sunnyAddButton(SunnyVector2D(1718/1920.*mapSize.x+horizontalMapAdd,(1280-856)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LOLCB_Share] = sunnyAddButton(SunnyVector2D(mapSize.x/2,(1280-1084)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize/2)*pixelToScale);
    buttons[LOLCB_Skip] = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapSize.y/2), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(100, 100));
    buttons[LOLCB_Skip]->soundTag = 0;
    for (short i = 0;i<LOLCB_Count;i++)
    {
        sunnyButtonSetCallbackFunc(buttons[i], loLCButtonPressed);
        sunnyButtonSetRenderFunc(buttons[i], loLCButtonRender);
        buttons[i]->tag = i+1;
        buttons[i]->matrix.pos.z = -2;
        buttons[i]->hidden = true;
    }
    
    labelLevelComplete = getLocalizedLabel("Level Completed");
    labelResult[0] = getLocalizedLabel("Good");
    labelResult[1] = getLocalizedLabel("Great");
    labelResult[2] = getLocalizedLabel("Perfect");
    labelNextCoin = getLocalizedLabel("Next Coin");
    
    prepareWindow(26,1,25,20);
}

void LOLevelComplete::prepareWindow(float time, short coins,short time2, short time3)
{
    animationTime = 0;
    levelTime = time;
    levelTime2 = time2;
    levelTime3 = time3;
    levelCoins = coins;
    
    if (coins!=3)
    {
        short time;
        if (coins==1)
            time = time2;
        else
            time = time3;
        time--;
        
        strcpy(nextCoinTime, ": 0:");
        int len = (int)strlen(nextCoinTime);
        nextCoinTime[len] = '0' + (time/10);
        nextCoinTime[len+1] = '0' + (time%10);
        nextCoinTime[len+2] = '\0';
        
        nextCoinLength[0] = LOFont::getTextSize(labelNextCoin);
        nextCoinLength[1] = LOFont::getTimeSize(nextCoinTime);
    }
    
    timeLength = LOFont::getTimeSize(levelTime);
    
    loLCCoinsAnimationFinished = loLCCoinAnimation.x + (loLCCoinAnimation.y-loLCCoinAnimation.x)*levelCoins;
    
    loLCNextCoinAnimationFinished = loLCPerfectAnimation.y + loLCCoinsAnimationFinished;
    if (coins!=3) loLCNextCoinAnimationFinished += 0.2;
    
    for (short i = 0;i<levelCoins;i++)
    {
        coinRotationAngle[i] = sRandomf()*2*M_PI;
        coinRotationSpeed[i] = (0.8+1.2*sRandomf());
    }
    
    nextCoinAnimationTime = loLCCoinsAnimationFinished + sRandomf()*3;
    nextAnimatedCoin = sRandomi()%levelCoins;
}

LOLevelComplete::~LOLevelComplete()
{
    for (short i = 0;i<LOLCB_Count;i++)
        sunnyRemoveButton(buttons[i]);
    
    delete [] labelLevelComplete;
    for (short i = 0;i<3;i++)
        delete[] labelResult[i];
    
    delete []labelNextCoin;
}

void LOLevelComplete::update()
{
    if (!isVisible) return;
    
    animationTime += deltaTime;
}

void LOLevelComplete::activateButtons()
{
    buttons[LOLCB_Skip]->hidden = true;
    
    if (selectedLevelNum == loLevelsPerStep-1)
    {
        if (selectedStepNum==0)
        {
            if (!LOScore::checkFlag(SURVIVAL_1))
            {
                LOScore::addFlag(SURVIVAL_1,false);
                LOMenu::getSharedMenu()->showSurvivalTutorial();
            }
        } else
            if (selectedStepNum==1)
            {
                if (!LOScore::checkFlag(SURVIVAL_2))
                {
                  LOScore::addFlag(SURVIVAL_2,false);
                    LOMenu::getSharedMenu()->showSurvivalTutorial();
                }
            } else
                if (selectedStepNum==2)
                {
                    if (!LOScore::checkFlag(SURVIVAL_3))
                    {
                      LOScore::addFlag(SURVIVAL_3,false);
                        LOMenu::getSharedMenu()->showSurvivalTutorial();
                    }
                }
    }
}

bool LOLevelComplete::didHideEverything()
{
    return (isVisible && animationTime>loLCBackgroundAnimation.y);
}

void LOLevelComplete::render()
{
    if (!isVisible) return;
    
    glDisable(GL_DEPTH_TEST);
    float farDist = -30;
    
    bindGameGlobal();
    SunnyMatrix m = sunnyIdentityMatrix;
    SunnyMatrix rot = sunnyIdentityMatrix;
    SunnyVector2D scaleA = SunnyVector2D((mapZoomBounds.y-mapZoomBounds.x)/mapSize.x,(mapZoomBounds.w-mapZoomBounds.z)/mapSize.y);
    if (animationTime>=loLCBackgroundAnimation.x)
    {
        GLuint globalView;
        GLuint tsa;
        if (animationTime>=loLCBackgroundAnimation.y)
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniform1f(uniformTSA_A, 1);
            globalView = globalModelview[LOS_TexturedTSA];
            tsa = uniformTSA_TR;
        } else
        {
            sunnyUseShader(globalShaders[LOS_TexturedTS_Animation]);
            globalView = globalModelview[LOS_TexturedTS_Animation];
            tsa = uniformTS_TR;
            
            float dist = 1;
            float position = mapZoomBounds.y - (mapZoomBounds.y-mapZoomBounds.x)*(animationTime - loLCBackgroundAnimation.x)/(loLCBackgroundAnimation.y-loLCBackgroundAnimation.x);
            glUniform4fv(uniformTS_Params, 1, SunnyVector4D(position-dist, position, 0, 1));
        }
        
        m.pos.z = farDist;
        glUniformMatrix4fv(globalView, 1, GL_FALSE, &(m.front.x));
        
        //Snow
        glDisable(GL_BLEND);
        SHTextureBase::bindTexture(objectsTexture);
        glUniform4fv(tsa, 1, SunnyVector4D( mapZoomBounds.x-(mapZoomBounds.y-mapZoomBounds.x)*0.05*scaleA.x,
                                            mapZoomBounds.z-(mapZoomBounds.w-mapZoomBounds.z)*0.05*scaleA.y, 1.1*scaleA.x, 1.1*scaleA.y));
        SunnyDrawArrays(LOBackground_VAO);
        
        glEnable(GL_BLEND);
        SHTextureBase::bindTexture(menuTexture);
        
        //Trees
        if (animationTime >= loLCTreesAnimation.x)
        {
            rot = getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapZoomBounds.z,farDist));
            glUniformMatrix4fv(globalView, 1, GL_FALSE, &(rot.front.x));
            if (animationTime >= loLCTreesAnimation.y)
                glUniform4fv(tsa, 1, SunnyVector4D(0, 0, scaleA.x, scaleA.y));
            else
                glUniform4fv(tsa, 1, SunnyVector4D(0, -5*(loLCTreesAnimation.y - animationTime)/(loLCTreesAnimation.y - loLCTreesAnimation.x), scaleA.x, scaleA.y));
            SunnyDrawArrays(LOLCForest_VAO);
        }
        
        //top
        glUniformMatrix4fv(globalView, 1, GL_FALSE, &(m.front.x));
        glUniform4fv(tsa, 1, SunnyVector4D(mapSize.x/2, mapZoomBounds.w, scaleA.x, scaleA.y));
        SunnyDrawArrays(LOLCBorderVAO);
        //Bottom
        glUniform4fv(tsa, 1, SunnyVector4D(mapSize.x/2, mapZoomBounds.z, scaleA.x, scaleA.y));
        SunnyDrawArrays(LOLCBorderVAO+1);
        //Right
        glUniform4fv(tsa, 1, SunnyVector4D(0, 0, scaleA.y, scaleA.x));
        rot =  getRotationZMatrix(-M_PI_2) * getTranslateMatrix(SunnyVector3D(mapZoomBounds.y,mapSize.y/2,farDist));
        glUniformMatrix4fv(globalView, 1, GL_FALSE, &(rot.front.x));
        SunnyDrawArrays(LOLCBorderVAO+2);
        //Left
        rot =  getRotationZMatrix(M_PI_2) * getTranslateMatrix(SunnyVector3D(mapZoomBounds.x,mapSize.y/2,farDist));
        glUniformMatrix4fv(globalView, 1, GL_FALSE, &(rot.front.x));
        SunnyDrawArrays(LOLCBorderVAO+3);
    }
    const float mapKoef = mapSize.x/1920;
    if (animationTime > loLCButtonsAnimation.x + loLCNextCoinAnimationFinished)
    {
        if (!buttons[LOLCB_Skip]->hidden)
            activateButtons();
        const float delta = 0.1;
        
        //Buttons
        float btnScale;
        float animationScale = 1;
        if (animationTime < loLCButtonsAnimation.y + loLCNextCoinAnimationFinished)
            animationScale = (animationTime - loLCNextCoinAnimationFinished - loLCButtonsAnimation.x)/(loLCButtonsAnimation.y-loLCButtonsAnimation.x);
        glUniform1f(uniformTSA_A, animationScale);
        //Main
        rot = m;
        rot.pos = buttons[LOLCB_MainMenu]->matrix.pos;
        buttons[LOLCB_MainMenu]->active?btnScale = defaultButtonScale*animationScale:btnScale = animationScale;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, 6*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+1);
        
        
        animationScale = 0;
        if (animationTime > loLCButtonsAnimation.x + loLCNextCoinAnimationFinished + delta)
        {
            animationScale = 1;
            if (animationTime < loLCButtonsAnimation.y + loLCNextCoinAnimationFinished + delta)
                animationScale = (animationTime - loLCNextCoinAnimationFinished - loLCButtonsAnimation.x - delta)/(loLCButtonsAnimation.y-loLCButtonsAnimation.x);
        }
        glUniform1f(uniformTSA_A, animationScale);
        //Store
        buttons[LOLCB_Store]->active?btnScale = defaultButtonScale*animationScale:btnScale = animationScale;
        rot.pos = buttons[LOLCB_Store]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-5*mapKoef, -8*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+2);
        
        animationScale = 0;
        if (animationTime > loLCButtonsAnimation.x + loLCNextCoinAnimationFinished + 2*delta)
        {
            animationScale = 1;
            if (animationTime < loLCButtonsAnimation.y + loLCNextCoinAnimationFinished + 2*delta)
                animationScale = (animationTime - loLCNextCoinAnimationFinished - loLCButtonsAnimation.x - 2*delta)/(loLCButtonsAnimation.y-loLCButtonsAnimation.x);
        }
        glUniform1f(uniformTSA_A, animationScale);
        //Replay
        buttons[LOLCB_Replay]->active?btnScale = defaultButtonScale*animationScale:btnScale = animationScale;
        rot.pos = buttons[LOLCB_Replay]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, 4*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+3);

        animationScale = 0;
        if (animationTime > loLCButtonsAnimation.x + loLCNextCoinAnimationFinished + 3*delta)
        {
            animationScale = 1;
            if (animationTime < loLCButtonsAnimation.y + loLCNextCoinAnimationFinished + 3*delta)
                animationScale = (animationTime - loLCNextCoinAnimationFinished - loLCButtonsAnimation.x - 3*delta)/(loLCButtonsAnimation.y-loLCButtonsAnimation.x);
        }
        glUniform1f(uniformTSA_A, animationScale);
        //Next
        buttons[LOLCB_Next]->active?btnScale = defaultButtonScale*animationScale:btnScale = animationScale;
        rot.pos = buttons[LOLCB_Next]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        
        if (animationTime>loLCButtonsAnimation.x + loLCNextCoinAnimationFinished)
        {
            btnScale *= (1.05 - 0.05*cosf(2*(animationTime-(loLCButtonsAnimation.x + loLCNextCoinAnimationFinished))));
            glUniform1f(uniformTSA_A, 1-(btnScale-1)*2);
        }
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(8*mapKoef, 4*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+4);
    }
    
    //Bottom Table
    if (animationTime>loLCBottomTableAnimation.x + loLCCoinsAnimationFinished)
    {
        rot = getTranslateMatrix(SunnyVector3D(948*mapKoef, (1280-774)*mapKoef,farDist));
        
        if (animationTime<loLCBottomTableAnimation.y + loLCCoinsAnimationFinished)
        {
            sunnyUseShader(globalShaders[LOS_TexturedTS_Animation]);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTS_Animation], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTS_TR, 1, SunnyVector4D(0, 0, 1, 1));
            float position = rot.pos.x -loLCTableWidth/2 + loLCTableWidth*(animationTime-loLCCoinsAnimationFinished-loLCBottomTableAnimation.x)/(loLCBottomTableAnimation.y-loLCBottomTableAnimation.x);
            glUniform4fv(uniformTS_Params, 1, SunnyVector4D(position, position+5, 1, 0));
        } else
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniform1f(uniformTSA_A, 1);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        }
        SunnyDrawArrays(LOLCSnowTable_VAO);
    }
    
    SunnyMatrix bottomM = rot;
    
    //Top Table
    if (animationTime>loLCTopTableAnimation.x)
    {
        rot = getTranslateMatrix(SunnyVector3D(916*mapKoef, (1280-464)*mapKoef,farDist));
        if (animationTime<loLCTopTableAnimation.y)
        {
            sunnyUseShader(globalShaders[LOS_TexturedTS_Animation]);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTS_Animation], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTS_TR, 1, SunnyVector4D(0, 0, 1, 1));
            float position = rot.pos.x -loLCTableWidth/2 + loLCTableWidth*(animationTime-loLCTopTableAnimation.x)/(loLCTopTableAnimation.y-loLCTopTableAnimation.x);
            glUniform4fv(uniformTS_Params, 1, SunnyVector4D(position, position+5, 1, 0));
            SunnyDrawArrays(LOLCSnowTable_VAO);
        } else
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
            SunnyDrawArrays(LOLCSnowTable_VAO);
            
            //Timer
            if (animationTime >= loLCTimerAnimation.x)
            {
                if (animationTime >= loLCTimerAnimation.y)
                {
                    //glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-186*mapKoef, -4*mapKoef, 1, 1));
                    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-230*mapKoef, 10*mapKoef, 1, 1));
                    if (!timerPlayed)
                    {
                        losPlayHardSound();
                        timerPlayed = true;
                    }
                }
                else
                {
                    float alpha = (animationTime - loLCTimerAnimation.x)/(loLCTimerAnimation.y-loLCTimerAnimation.x);
                    float scale = 1 + 3*(1-alpha);
                    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-230*mapKoef, 10*mapKoef, scale, scale));
                    glUniform1f(uniformTSA_A, alpha);
                }
                SunnyDrawArrays(LOLCClock_VAO);
            }
        }
    }
    
    //Level Completed
    if (animationTime>=loLCLevelCompleteAnimation.x)
    {
        float alpha;
        float scale;
        if (animationTime>=loLCLevelCompleteAnimation.y)
        {
            scale = 1;
            alpha = 1;
            if (!levelLabelPlayed)
            {
                losPlayHardSound();
                levelLabelPlayed = true;
            }
        } else
        {
            alpha = (animationTime - loLCLevelCompleteAnimation.x)/(loLCLevelCompleteAnimation.y-loLCLevelCompleteAnimation.x);
            scale = 1 + 3*(1-alpha);
        }
        
        rot = getScaleMatrix(SunnyVector3D(0.715*scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef,(1280-170)*mapKoef,farDist));
        LOFont::writeText(&rot, LOTA_Center, true, labelLevelComplete, whiteOldInnerColor(alpha),whiteOldOuterColor(alpha));
    }
    
    SunnyMatrix mat;
    if (animationTime > loLCPerfectAnimation.x + loLCCoinsAnimationFinished)
    {
        //good->great->perfect
        if (levelCoins>=1 && levelCoins<=3)
        {
            float scale = 0.54;
            float alpha = 1;
            if (animationTime < loLCPerfectAnimation.y + loLCCoinsAnimationFinished) alpha = (animationTime - loLCCoinsAnimationFinished - loLCPerfectAnimation.x)/(loLCPerfectAnimation.y-loLCPerfectAnimation.x);

            if (levelCoins==3)
            {
                bottomM = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(0,8*mapKoef,0)) * bottomM;
                LOFont::writeText(&bottomM, LOTA_Center, true, labelResult[levelCoins-1],SunnyVector4D(260,250,20,255*alpha)/255,SunnyVector4D(245,148,69,255*alpha)/255);
            } else
            {
                mat = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(20*mapKoef,42*mapKoef,0)) * bottomM;
                LOFont::writeText(&mat, LOTA_Center, true, labelResult[levelCoins-1],SunnyVector4D(260,250,20,255*alpha)/255,SunnyVector4D(245,148,69,255*alpha)/255);
                scale = 0.45;
                mat = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(-nextCoinLength[1]/2*scale+15*mapKoef,-44*mapKoef,0)) * bottomM;
                if (animationTime < loLCNextCoinAnimationFinished)
                    alpha = (animationTime - loLCCoinsAnimationFinished - loLCPerfectAnimation.y)/(loLCNextCoinAnimationFinished - loLCCoinsAnimationFinished-loLCPerfectAnimation.y);
                LOFont::writeText(&mat, LOTA_Center, true, labelNextCoin,SunnyVector4D(260,250,20,255*alpha)/255,SunnyVector4D(245,148,69,255*alpha)/255);
            }
        }
    }
    
    bindGameGlobal();
    if (animationTime > loLCPerfectAnimation.y)
    {
        //Next Coin Time
        if (levelCoins==2 || levelCoins==1)
        {
            mat.pos.x += nextCoinLength[0]/2*mat.front.x;
            float alpha = 1;
            if (animationTime < loLCNextCoinAnimationFinished)
                alpha = (animationTime - loLCCoinsAnimationFinished - loLCPerfectAnimation.y)/(loLCNextCoinAnimationFinished - loLCCoinsAnimationFinished-loLCPerfectAnimation.y);
            LOFont::writeTime(&mat, true, nextCoinTime,SunnyVector4D(260,250,20,255*alpha)/255,SunnyVector4D(245,148,69,255*alpha)/255);
        }
    }
    
    //Time value
    if (animationTime >= loLCTimeValueAnimation.x)
    {
        mat = getScaleMatrix(0.9) * getTranslateMatrix(SunnyVector3D(982*mapKoef - timeLength/2, (1280-470+8)*mapKoef,farDist));
        float time = levelTime;
        if (animationTime < loLCTimeValueAnimation.y)
            time *= (animationTime-loLCTimeValueAnimation.x)/(loLCTimeValueAnimation.y-loLCTimeValueAnimation.x);
        
        LOFont::writeTime(&mat, true, time, whiteOldInnerColor(1),whiteOldOuterColor(1));
    }
    
    //Share
    if (animationTime > (loLCSnowballAnimation.x + loLCSnowballAnimation.y)/2 + loLCNextCoinAnimationFinished)
    {
        //bindGameGlobal();
        float scale;
        float alpha = fminf(1, (animationTime - ((loLCSnowballAnimation.x + loLCSnowballAnimation.y)/2 + loLCNextCoinAnimationFinished))/(loLCSnowballAnimation.y - loLCSnowballAnimation.x)*2);
        buttons[LOLCB_Share]->active?scale=defaultButtonScale:scale=1;
        m = sunnyIdentityMatrix;
        m.pos = buttons[LOLCB_Share]->matrix.pos;
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        
        SHTextureBase::bindTexture(menuTexture);
        glUniform1f(uniformTSA_A, alpha);
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
        SunnyDrawArrays(LOShareFB_VAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-164*mapKoef*scale, -12*mapKoef*scale, 0.65*scale, 0.65*scale));
        SunnyDrawArrays(LOSocialButtonFB_VAO);
        
        m = getScaleMatrix(0.4*scale*globalFontScale) *  m * getTranslateMatrix(SunnyVector3D(1.3,0,0));
        LOFont::writeText(&m,LOTA_Center, true, loLabelShare,whiteInnerColor(alpha),whiteOuterColor(alpha));
    }
    
    //Coins
    if (animationTime>=loLCCoinAnimation.x)
    {
        glEnable(GL_CULL_FACE);

        const float rotationSpeed = -M_PI/2;
        glEnable(GL_DEPTH_TEST);
        float coinScale = 2.5;
        const float smallScale = 0.9*coinScale;
        const float bigScale = 1.3*coinScale;
        LOGoldCoin::prepareForRender();
        SunnyMatrix rot = getRotationXMatrix(-M_PI/15) * getRotationYMatrix(-M_PI/15);
        SunnyMatrix rotB = getScaleMatrix(SunnyVector3D(bigScale,bigScale,bigScale))* rot;
        rot = getScaleMatrix(SunnyVector3D(smallScale,smallScale,smallScale))* rot;
        //SunnyVector4D center = SunnyVector4D(19.5, 10.5, farDist ,1);
        
        float blickAngle = -M_PI_2;
        if (animationTime>nextCoinAnimationTime)
        {
            blickAngle += (animationTime-nextCoinAnimationTime)*5;
            
            if (blickAngle + M_PI_2 > M_PI*1.3)
            {
                nextCoinAnimationTime = animationTime + 2 + sRandomf()*3;
                nextAnimatedCoin = sRandomi()%levelCoins;
                blickAngle = -M_PI_2;
            }
        }
        
        const SunnyVector2D coinsPositions[] = {SunnyVector2D(mapSize.x/2 + (1236-960)*mapKoef,mapSize.y/2 + (640-484)*mapKoef),
                                                SunnyVector2D(mapSize.x/2 + (1420-960)*mapKoef,mapSize.y/2 + (640-590)*mapKoef),
                                                SunnyVector2D(mapSize.x/2 + (1344-960)*mapKoef,mapSize.y/2 + (640-474)*mapKoef)};
        
        float scale,alpha = 1;
        if (levelCoins>0)
        {
            if (coinSoundPlayed<1)
            {
                coinSoundPlayed = 1;
                losPlayGoldCoinSound();
            }
            m = rot;
            //m.pos = center + SunnyVector4D(0.1,1.3,2,0)*coinScale;
            m.pos = SunnyVector4D(coinsPositions[0].x,coinsPositions[0].y,farDist + 4*coinScale,1);
            
            if (animationTime<loLCCoinAnimation.y)
            {
                alpha = (animationTime - loLCCoinAnimation.x)/(loLCCoinAnimation.y-loLCCoinAnimation.x);
                float angle = -M_PI_2 + (1-alpha) * M_PI*2;
                scale = 1 + 3*(1-alpha);
                m = getScaleMatrix(scale)* getRotationZMatrix(coinRotationAngle[0])* getRotationXMatrix(rotationSpeed*coinRotationSpeed[0]*(scale-1)) *getRotationZMatrix(-coinRotationAngle[0])*  m;
                alpha = fminf(1, alpha*7);
                LOGoldCoin::renderModelForMenu(&m,alpha,angle);
            } else
            if (nextAnimatedCoin==0) LOGoldCoin::renderModelForMenu(&m,alpha,blickAngle);
            else LOGoldCoin::renderModelForMenu(&m,alpha);
            if (levelCoins>1 && animationTime>loLCCoinAnimation.y)
            {
                if (coinSoundPlayed<2)
                {
                    coinSoundPlayed = 2;
                    losPlayGoldCoinSound();
                }
                m = rot;
                //m.pos = center + SunnyVector4D(1.3,0.6,4,0)*coinScale;
                m.pos = SunnyVector4D(coinsPositions[1].x,coinsPositions[1].y,farDist + 6*coinScale,1);
                
                if (animationTime<2*loLCCoinAnimation.y-loLCCoinAnimation.x)
                {
                    alpha = (animationTime - loLCCoinAnimation.y)/(loLCCoinAnimation.y-loLCCoinAnimation.x);
                    float angle = -M_PI_2 + (1-alpha) * M_PI*2;
                    scale = 1 + 3*(1-alpha);
                    m = getScaleMatrix(scale)* getRotationZMatrix(coinRotationAngle[1])* getRotationXMatrix(rotationSpeed*coinRotationSpeed[1]*(scale-1)) *getRotationZMatrix(-coinRotationAngle[1])*  m;
                    alpha = fminf(1, alpha*7);
                    LOGoldCoin::renderModelForMenu(&m,alpha,angle);
                } else
                if (nextAnimatedCoin==1) LOGoldCoin::renderModelForMenu(&m,alpha,blickAngle);
                else LOGoldCoin::renderModelForMenu(&m,alpha);
                
                if (levelCoins>2 && animationTime>2*loLCCoinAnimation.y-loLCCoinAnimation.x)
                {
                    if (coinSoundPlayed<3)
                    {
                        coinSoundPlayed = 3;
                        losPlayGoldCoinSound();
                    }
                    m = rotB;
                    //m.pos = center + SunnyVector4D(0.7,1.3,6,0)*coinScale;
                    m.pos = SunnyVector4D(coinsPositions[2].x,coinsPositions[2].y,farDist + 8*coinScale,1);
                    
                    if (animationTime<3*loLCCoinAnimation.y-2*loLCCoinAnimation.x)
                    {
                        alpha = (animationTime - 2*loLCCoinAnimation.y + loLCCoinAnimation.x)/(loLCCoinAnimation.y-loLCCoinAnimation.x);
                        float angle = -M_PI_2 + (1-alpha) * M_PI*2;
                        scale = 1 + 3*(1-alpha);
                        m = getScaleMatrix(scale)* getRotationZMatrix(coinRotationAngle[2])* getRotationXMatrix(rotationSpeed*coinRotationSpeed[2]*(scale-1)) *getRotationZMatrix(-coinRotationAngle[2])*  m;
                        alpha = fminf(1, alpha*7);
                        LOGoldCoin::renderModelForMenu(&m,alpha,angle);
                    } else
                    if (nextAnimatedCoin==2) LOGoldCoin::renderModelForMenu(&m,alpha,blickAngle);
                    else LOGoldCoin::renderModelForMenu(&m,alpha);
                }
            }
        }
        glDisable(GL_CULL_FACE);
        //shadows
        bindGameGlobal();
        const float shadow = 0.1;
        sunnyUseShader(globalShaders[LOS_TexturedTSC]);
        glUniform4fv(uniformTSC_C, 1, SunnyVector4D(1,1,1,0.7));
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(-shadow, -shadow,1.3,1.3));
        
        rot = getScaleMatrix(SunnyVector3D(smallScale,smallScale,smallScale));
        rotB = getScaleMatrix(SunnyVector3D(bigScale,bigScale,bigScale));
        
        SHTextureBase::bindTexture(objectsTexture);
        if (levelCoins>0)
        {
            m = rot;
            m.pos = SunnyVector4D(coinsPositions[0].x,coinsPositions[0].y,farDist + 1*coinScale,1);
            
            if (animationTime<loLCCoinAnimation.y)
            {
                alpha = (animationTime - loLCCoinAnimation.x)/(loLCCoinAnimation.y-loLCCoinAnimation.x);
                scale = 1 - (1-alpha);
                float move = -shadow - (1-alpha)*2;
                m = getScaleMatrix(SunnyVector3D(1,(0.4+0.6*fabsf(cosf((scale-1)/0.8*3*rotationSpeed*coinRotationSpeed[0]))),1)) * getRotationZMatrix(-coinRotationAngle[0]) * getTranslateMatrix(SunnyVector3D(move,move,0)) * m;
                glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, 0,1.3*scale,scale*1.3));
                glUniform4fv(uniformTSC_C, 1, SunnyVector4D(1,1,1,0.7*alpha));
            }
            
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
            SunnyDrawArrays(LOShadows_VAO);
            
            if (levelCoins>1 && animationTime>loLCCoinAnimation.y)
            {
                m.pos = SunnyVector4D(coinsPositions[1].x,coinsPositions[1].y,farDist + 3*coinScale,1);
                
                if (animationTime<2*loLCCoinAnimation.y - loLCCoinAnimation.x)
                {
                    alpha = (animationTime - loLCCoinAnimation.y)/(loLCCoinAnimation.y-loLCCoinAnimation.x);
                    scale = 1 - (1-alpha);
                    float move = -shadow - (1-alpha)*2;
                    m = getScaleMatrix(SunnyVector3D(1,(0.4+0.6*fabsf(cosf((scale-1)/0.8*3*rotationSpeed*coinRotationSpeed[1]))),1)) * getRotationZMatrix(-coinRotationAngle[1]) * getTranslateMatrix(SunnyVector3D(move,move,0)) * m;
                    glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, 0,1.3*scale,scale*1.3));
                    glUniform4fv(uniformTSC_C, 1, SunnyVector4D(1,1,1,0.7*alpha));
                }
                
                glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(LOShadows_VAO);
                if (levelCoins>2 && animationTime>2*loLCCoinAnimation.y - loLCCoinAnimation.x)
                {
                    m = rotB;
                    m.pos = SunnyVector4D(coinsPositions[2].x,coinsPositions[2].y,farDist + 5*coinScale,1);
                    
                    if (animationTime<3*loLCCoinAnimation.y - 2*loLCCoinAnimation.x)
                    {
                        alpha = (animationTime - 2*loLCCoinAnimation.y + loLCCoinAnimation.x)/(loLCCoinAnimation.y-loLCCoinAnimation.x);
                        scale = 1 - (1-alpha);
                        float move = -shadow - (1-alpha)*2;
                        m = getScaleMatrix(SunnyVector3D(1,(0.4+0.6*fabsf(cosf((scale-1)/0.8*3*rotationSpeed*coinRotationSpeed[2]))),1)) * getRotationZMatrix(-coinRotationAngle[2]) * getTranslateMatrix(SunnyVector3D(move,move,0)) * m;
                        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, 0,1.3*scale,scale*1.3));
                        glUniform4fv(uniformTSC_C, 1, SunnyVector4D(1,1,1,0.7*alpha));
                    }
                    
                    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
                    SunnyDrawArrays(LOShadows_VAO);
                }
            }
        }
        
        if (animationTime > loLCSnowballAnimation.x + loLCNextCoinAnimationFinished)
        {
            //SnowBall
            float d = (animationTime - loLCSnowballAnimation.x - loLCNextCoinAnimationFinished)/(loLCSnowballAnimation.y - loLCSnowballAnimation.x);
            if (d<1.1)
            {
                glDisable(GL_BLEND);
                rot = getScaleMatrix(4) * getRotationZMatrix(-(mapZoomBounds.y-mapZoomBounds.x)*d/4) * getTranslateMatrix(SunnyVector3D(mapZoomBounds.x + (mapZoomBounds.y-mapZoomBounds.x)*d,mapZoomBounds.z+2 + sinf(d*M_PI),-5));
                losSetRollingSnowballSpeed(sinf(d*M_PI) + 0.5);
                LOPlayer::renderForMenu(&rot,1);
                glEnable(GL_BLEND);
                playerPosition = rot;
            } else
                losSetRollingSnowballSpeed(0);
            //SnowBall...
        }
        
        glDisable(GL_DEPTH_TEST);
    }
}

void LOLevelComplete::renderOverlay()
{
    if (!isVisible) return;

    if (animationTime > loLCSnowballAnimation.x + loLCNextCoinAnimationFinished)
    {
        SunnyMatrix rot = sunnyIdentityMatrix;
        rot.pos.z = -1;
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(playerPosition.pos.x,playerPosition.pos.y,3,3));
        SunnyDrawArrays(LOOverlaySnowflake_VAO);
    }
}

void LOLevelComplete::setVisible(bool visible)
{
    if (isVisible == visible)
        return;
    
    isVisible = visible;
    
    for (short i = 0;i<LOLCB_Count;i++)
        buttons[i]->hidden = !visible;
    
    levelLabelPlayed = timerPlayed = false;
    
    losSetRollingSnowballSpeed(0);
    
    if (isVisible)
    {
        lastWind = mapWind;
        lastNewWind = newMapWind;
        
        mapWind.z = 1;
        mapWind.w = 300;
        newMapWind = SunnyVector2D(0,-0.3);
        coinSoundPlayed = 0;
    } else
    {
        mapWind = lastWind;
        newMapWind = lastNewWind;
    }
}




