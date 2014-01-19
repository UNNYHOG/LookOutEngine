//
//  LODeathWindow.cpp
//  LookOut
//
//  Created by Ignatov on 10.10.13.
//
//

#include "LODeathWindow.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "iPhone_Strings.h"
#include "LOFont.h"
#include "LOScore.h"
#include "LOGlobalVar.h"
#include "LookOutEngine.h"
#include "LOTasksView.h"
#include "LOSounds.h"
#include "LOTasksView.h"

const short tasksToUnlockMagnet = 10;
const short tasksToUnlockTime = 20;
const short tasksToUnlockDoubler = 30;

float ressurectionButtonTime = 3;
const float ressurectioDelay = 1;

LODeathWindow * sharedDeathWindow = NULL;

void loDWButtonPressed(SunnyButton *btn)
{
    sharedDeathWindow->buttonPressed(btn);
}

void loDWButtonRender(SunnyButton *btn)
{
    sharedDeathWindow->renderCallBack(btn);
}

void LODeathWindow::renderCallBack(SunnyButton *btn)
{
    if (globalMoneyAlpha<=0 || !isVisible) return;
    
    if (btn == buttons[LODWB_Spell_Doubler] || btn == buttons[LODWB_Spell_Time] || btn == buttons[LODWB_Spell_Magnet])
    {
        bindGameGlobal();
        SunnyMatrix m = sunnyIdentityMatrix;
        float scale = 1;
        m.pos = btn->matrix.pos;
        if (btn->active)
            scale *= 1.2;
        LevelAbility ability;
        if (btn==buttons[LODWB_Spell_Magnet])
            ability = LA_Magnet;
        else
            if (btn==buttons[LODWB_Spell_Time])
                ability = LA_Time;
            else
//                if (btn==buttons[LODWB_Spell_Doubler])
                    ability = LA_Doubler;
        
        float scale2 = scale;
        if (btn->active)
            scale2 *= (0.75-0.25*sin((getCurrentTime()-btn->tag*0.5)*M_PI));
        
        bool enabled = (btn->enable && LOScore::isSpellEnabled(ability));
        LOMenu::getSharedMenu()->renderSpellButton(ability,&m,scale,scale2,0,enabled,globalMoneyAlpha,enabled);
        return;
    }
}

void LODeathWindow::recalculateSurvivalPrices(SunnyButton * btn)
{
    LOPriceType type;
    unsigned short price;
    unsigned short totalPrice = 0;
    if (buttons[LODWB_Spell_Magnet]->active)
    {
        type = LOScore::getPrice(LA_Magnet, price);
        if (type==LOPT_GoldCoins)
            totalPrice += price;
    }
    if (buttons[LODWB_Spell_Time]->active)
    {
        type = LOScore::getPrice(LA_Time, price);
        if (type==LOPT_GoldCoins)
            totalPrice += price;
    }
    if (buttons[LODWB_Spell_Doubler]->active)
    {
        type = LOScore::getPrice(LA_Doubler, price);
        if (type==LOPT_GoldCoins)
            totalPrice += price;
    }
    
    if (totalPrice>LOScore::getTotalCoins())
    {
        btn->active = false;
        LOMenu::getSharedMenu()->showNotEnoughGoldHint(loDWButtonPressed,buttons[LODWB_Store]);
    }
}

void LODeathWindow::buttonPressed(SunnyButton *btn)
{
    ressurectionWasUsed = false;
    
    if (btn == buttons[LODWB_Spell_Doubler])
    {
        if (!LOScore::isSpellEnabled(LA_Doubler))
        {
            btn->active = false;
            LOMenu::getSharedMenu()->showSpellInactive();
        }
        recalculateSurvivalPrices(btn);
    } else
        if (btn == buttons[LODWB_Spell_Time])
        {
            if (!LOScore::isSpellEnabled(LA_Time))
            {
                btn->active = false;
                LOMenu::getSharedMenu()->showSpellInactive();
            }
            recalculateSurvivalPrices(btn);
        } else
            if (btn == buttons[LODWB_Spell_Magnet])
            {
                if (!LOScore::isSpellEnabled(LA_Magnet))
                {
                    btn->active = false;
                    LOMenu::getSharedMenu()->showSpellInactive();
                }
                recalculateSurvivalPrices(btn);
            } else
    if (buttons[LODWB_MainMenu] == btn)
    {
        LOMenu::getSharedMenu()->showGameOverButtons(false);
        loLoadEmptyMap();
        gamePaused = true;
        LOMenu::getSharedMenu()->showGamePlayButtons(false);
        if (isSurvival)
            LOMenu::getSharedMenu()->showSurvivalButtons(true);
        else
            LOMenu::getSharedMenu()->showLevelsButtons(true);
    } else
        if (buttons[LODWB_Ressurect] == btn)
        {
            if (player->activateAbility(LA_Ressurection))
            {
                ressurectionWasUsed = true;
                LOMenu::getSharedMenu()->showGameOverButtons(false);
                gamePaused = false;
                LOMenu::getSharedMenu()->showGameSpellButtons(true);
                gameOvered = false;
                gameOveredMultiPlayer = false;
            } else
            {
                pausedForBuy = true;
                LOMenu::getSharedMenu()->showNotEnoughGoldHint(loDWButtonPressed,buttons[LODWB_Store]);
                buttons[LODWB_Lock]->hidden = true;
            }
        } else
            if (buttons[LODWB_Replay] == btn)
            {
                resetType = RT_Death;
            } else
                if (buttons[LODWB_Store] == btn)
                {
                    pausedForBuy = true;
                    LOMenu::getSharedMenu()->showStoreMenu(true, StoreBackDeathWindow);
                    isVisible = false;
                    if (isSurvival)
                        LOTasksView::getSharedView()->isVisible = false;
                    buttons[LODWB_Lock]->hidden = true;
                } else
                    if (buttons[LODWB_Lock] == btn)
                    {
                        if (animationTime>1)
                        {
                            updateTasks(false);
                            animationTime = ressurectionButtonTime;
                            btn->hidden = true;
                        }
                    }
}

LODeathWindow::LODeathWindow()
{
    sharedDeathWindow = this;
    
    float btnSize = 340/2;
    float btnBig = 778/2;
    buttons[LODWB_MainMenu] = sunnyAddButton(SunnyVector2D(960/1920.*mapSize.x,(1280-1062)/1280.*mapSize.y-verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    buttons[LODWB_Replay] = sunnyAddButton(SunnyVector2D(1718/1920.*mapSize.x+horizontalMapAdd,(1280-1062)/1280.*mapSize.y-verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    
    float dSpells = 0.05*mapSize.x;
    const float translateY = mapSize.y*0.25;
    buttons[LODWB_Spell_Doubler] = sunnyAddButton(SunnyVector2D(1430./1920*mapSize.x,(1280-641)/1280.*mapSize.y+translateY) , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Check,SunnyVector2D(118*pixelToScale,110*pixelToScale));
    buttons[LODWB_Spell_Magnet] = sunnyAddButton(SunnyVector2D((1276.-140)/1920*mapSize.x,(1280-641)/1280.*mapSize.y+translateY) , menuTexture, SunnyVector4D(118,0,106,110)/2048,SB_Check,SunnyVector2D(106*pixelToScale,110*pixelToScale));
    buttons[LODWB_Spell_Time] = sunnyAddButton(SunnyVector2D((1594.+140)/1920*mapSize.x,(1280-641)/1280.*mapSize.y+translateY) , menuTexture, SunnyVector4D(224,0,112,122)/2048,SB_Check,SunnyVector2D(112*pixelToScale,122*pixelToScale));
    sunnyButtonSetScale(buttons[LODWB_Spell_Doubler], dSpells, dSpells);
    sunnyButtonSetScale(buttons[LODWB_Spell_Magnet], dSpells, dSpells);
    sunnyButtonSetScale(buttons[LODWB_Spell_Time], dSpells, dSpells);
    buttons[LODWB_Spell_Doubler]->soundTag = buttons[LODWB_Spell_Time]->soundTag = buttons[LODWB_Spell_Magnet]->soundTag = LOBT_DropSound;
    
    buttons[LODWB_Lock] = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapSize.y/2), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(100, 100));
    buttons[LODWB_Lock]->soundTag = 0;
    buttons[LODWB_Ressurect] = sunnyAddButton(SunnyVector2D(960/1920.*mapSize.x,(1280-680)/1280.*mapSize.y-verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnBig, btnSize)*pixelToScale);
    buttons[LODWB_Ressurect]->soundTag = LOBT_Ressurection;
    buttons[LODWB_Store] = sunnyAddButton(SunnyVector2D(206/1920.*mapSize.x-horizontalMapAdd,(1280-1062)/1280.*mapSize.y-verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);
    for (short i = 0;i<LODWB_Count;i++)
    {
        sunnyButtonSetCallbackFunc(buttons[i], loDWButtonPressed);
        sunnyButtonSetRenderFunc(buttons[i], loDWButtonRender);
        buttons[i]->tag = i+1;
        buttons[i]->matrix.pos.z = -2;
        buttons[i]->hidden = true;
    }
    
    isVisible = false;
    
    labelGameOver = getLocalizedLabel("Game Over");
    labelSaveMe = getLocalizedLabel("Save Me");
}

LODeathWindow::~LODeathWindow()
{
    for (short i = 0;i<LODWB_Count;i++)
        sunnyRemoveButton(buttons[i]);
    
    delete []labelGameOver;
    delete []labelSaveMe;
}

void LODeathWindow::checkForSpellsUpdates()
{
    if (!isSurvival) return;
    
    int tasksCount = LOScore::getCompletedTasksCount(selectedStepNum);
    if (!LOScore::isSpellEnabled(LA_Magnet))
    {
        if (tasksCount>=tasksToUnlockMagnet)
        {
            if (LOMenu::getSharedMenu()->showSpellTutorial(LA_Magnet))
            {
                LOScore::addAbilityCharges(LA_Magnet,freeSpellsCount);
                LOScore::addFlag(SPELL_MAGNET);
                buttons[LODWB_Spell_Magnet]->active = true;
            }
        }
    } else
        if (!LOScore::isSpellEnabled(LA_Time))
        {
            if (tasksCount>=tasksToUnlockTime)
            {
                if (LOMenu::getSharedMenu()->showSpellTutorial(LA_Time))
                {
                    LOScore::addAbilityCharges(LA_Time,freeSpellsCount);
                    LOScore::addFlag(SPELL_TIME);
                    buttons[LODWB_Spell_Time]->active = true;
                }
            }
        } else
            if (!LOScore::isSpellEnabled(LA_Doubler))
            {
                if (tasksCount>=tasksToUnlockDoubler)
                {
                    if (LOMenu::getSharedMenu()->showSpellTutorial(LA_Doubler))
                    {
                        LOScore::addAbilityCharges(LA_Doubler,freeSpellsCount);
                        LOScore::addFlag(SPELL_DOUBLER);
                        buttons[LODWB_Spell_Doubler]->active = true;
                    }
                }
            }
}

void LODeathWindow::setVisible(bool visible)
{
    if (isVisible == visible)
        return;
    
    isVisible = visible;
    
    for (short i = 0;i<LODWB_Count;i++)
        buttons[i]->hidden = !visible;
    
    if (isVisible)
    {
        if (cheatProtection3 && sRandomf()<0.5)
            sunnyRemoveButton(buttons[LODWB_Replay]);

        pausedForBuy = false;
        ressurectionButtonTime = 3;
        ressurectionButtonTime+=ressurectioDelay;
        animationTime = 0;
        rotationTime = 0;
        crashed = false;
        appearAlpha = 0;
        hintShowedUp = false;
        
        if (!LOScore::isSpellEnabled(LA_Ressurection))
        {
            if (selectedStepNum>0 || selectedLevelNum>=5)
            {
                hintShowedUp = true;
                for (short i = 1;i<LODWB_Count;i++)
                    buttons[i]->hidden = true;
                LOMenu::getSharedMenu()->showRessurectionHint();
                LOScore::addAbilityCharges(LA_Ressurection,freeSpellsCount);
                LOScore::addFlag(SPELL_RESSURECTION);
            } else
                animationTime = ressurectionButtonTime + 2;
        }
        
        ressurectType = LOScore::getPrice(LA_Ressurection, ressurectionPrice);
        
        lastPlaySound = -1;
    }
    
    if (isSurvival)
    {
        LOTasksView::getSharedView()->setVisible(isVisible);
        
        sunnyButtonSetPosition(buttons[LODWB_Ressurect], SunnyVector2D((960+426)/1920.*mapSize.x,(1280-680)/1280.*mapSize.y-verticalMapAdd));
        
        if (isVisible)
        {
            LOPriceType type;
            unsigned short price;
            type = LOScore::getPrice(LA_Doubler, price);buttons[LODWB_Spell_Doubler]->active = (type==LOPT_Free) && LOScore::isSpellEnabled(LA_Doubler);;
            type = LOScore::getPrice(LA_Time, price);buttons[LODWB_Spell_Time]->active = (type==LOPT_Free) && LOScore::isSpellEnabled(LA_Time);;
            type = LOScore::getPrice(LA_Magnet, price);buttons[LODWB_Spell_Magnet]->active = (type==LOPT_Free) && LOScore::isSpellEnabled(LA_Magnet);;
        }
    } else
    {
        buttons[LODWB_Spell_Magnet]->hidden = buttons[LODWB_Spell_Doubler]->hidden = buttons[LODWB_Spell_Time]->hidden = true;
        buttons[LODWB_Lock]->hidden = true;
        sunnyButtonSetPosition(buttons[LODWB_Ressurect], SunnyVector2D(960/1920.*mapSize.x,(1280-680)/1280.*mapSize.y-verticalMapAdd));
    }
}

void LODeathWindow::update()
{
    if (!isVisible) return;
    
    if (!hintShowedUp && !pausedForBuy)
        animationTime += deltaTime;
    rotationTime += deltaTime;
    
    if (animationTime>ressurectionButtonTime)
    {
        if (!crashed)
        {
            snowBallCrashObjForMenu->prepareToCrashForMenu(snowballMatrix);
            crashed = true;
            updateTasks(true);
            losPlaySnowballDeathSound();
            buttons[LODWB_Lock]->hidden = true;
        } else
        {
            //if (animationTime<ressurectionButtonTime+1)
                snowBallCrashObjForMenu->updateForMenu();
        }
    }
    
    appearAlpha += deltaTime*5;
    if (appearAlpha>1)
        appearAlpha = 1;
    
    if (isSurvival)
    {
        if (buttons[LODWB_Lock]->hidden && !LOMenu::getSharedMenu()->survComplete)
        {
            if (!LOTasksView::getSharedView()->isLocked() && !LOMenu::getSharedMenu()->isRewardViewVisible())
            {
                checkForSpellsUpdates();
            }
        }
    }
}

void LODeathWindow::updateTasks(bool animated)
{
    if (!isSurvival) return;
    
    //Show results
    LOMenu::getSharedMenu()->showSurvivalCompleteWindow();
}

void LODeathWindow::renderOverlay()
{
    if (!isVisible || !isSurvival || globalMoneyAlpha<=0) return;
    
    float scale = 0.55;
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
    SunnyMatrix m;
    if (LOScore::isSpellEnabled(LA_Magnet))
    {
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(21./255,131./255,253./255,globalMoneyAlpha));
        m = buttons[LODWB_Spell_Magnet]->matrix;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
    
    if (LOScore::isSpellEnabled(LA_Time))
    {
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,213./255,21./255,globalMoneyAlpha));
        m = buttons[LODWB_Spell_Time]->matrix;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
    
    if (LOScore::isSpellEnabled(LA_Doubler))
    {
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(200./255,32./255,237./255,globalMoneyAlpha));
        m = buttons[LODWB_Spell_Doubler]->matrix;
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
}

void LODeathWindow::render()
{
    if (!isVisible) return;
    
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    
    glDisable(GL_DEPTH_TEST);
    const float mapKoef = mapSize.x/1920;
    const float farDist = -20;
    //fade
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    SunnyMatrix m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y/2,farDist));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform1f(uniformTSA_A, 0.5*appearAlpha);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
    SunnyDrawArrays(LODeath_VAO+1);
    
    float survivalXTranslate = 0;
    if (isSurvival)
        survivalXTranslate = 426*mapKoef;
    
    SunnyMatrix rot;
    float btnScale;
    float resAlpha;
    float moneyAlpha = -1;
    //Buttons
    {
        if (animationTime<ressurectionButtonTime)
        {
            if (hintShowedUp)
                glUniform1f(uniformTSA_A, 0.7*appearAlpha/2);
            else
                glUniform1f(uniformTSA_A, 0.7*appearAlpha);
        }
        else
        {
            float a = 0.7 + (animationTime-ressurectionButtonTime)*2;
            glUniform1f(uniformTSA_A, fminf(a, 1));
        }
        //Store
        m = getTranslateMatrix(SunnyVector3D(0,0,farDist));
        
        rot = m;
        rot.pos = buttons[LODWB_Store]->matrix.pos;
        buttons[LODWB_Store]->active?btnScale = defaultButtonScale:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, -2*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+2);
        
        //Main
        buttons[LODWB_MainMenu]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = buttons[LODWB_MainMenu]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-6*mapKoef, -6*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+1);
        
        //Replay
        buttons[LODWB_Replay]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = buttons[LODWB_Replay]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        if (animationTime>ressurectionButtonTime)
        {
            btnScale *= (1.05 - 0.05*cosf(2*(animationTime-ressurectionButtonTime)));
            glUniform1f(uniformTSA_A, 1-(btnScale-1)*2);
        }
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, 4*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+3);
        
        //Ressurect
        if (animationTime<ressurectionButtonTime+0.2)
        {
            if (animationTime<ressurectionButtonTime)
                resAlpha = 1*appearAlpha;
            else
                resAlpha = 1.0 - (animationTime-ressurectionButtonTime)*5;
            glUniform1f(uniformTSA_A, resAlpha);
            buttons[LODWB_Ressurect]->active?btnScale = defaultButtonScale:btnScale = 1;
            rot.pos = buttons[LODWB_Ressurect]->matrix.pos;
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
            SunnyDrawArrays(LOBigButton_VAO);
        } else
        {
            buttons[LODWB_Ressurect]->hidden = true;
            resAlpha = -1;
        }
    }
    
//    float coinsLabelWidth = goldCoinImageWidth;
//    float coinsNumberWidth;
//    
//    float snowflakesLabelWidth = snowflakeImageWidth;
//    float snowflakesNumberWidth;

    float moneyTranslate = 0;
    if (!LOScore::isSpellEnabled(LA_Ressurection))
        moneyTranslate = 200;

    //GoldCoin
    if (resAlpha>0)
    {
        unsigned int price = ressurectionPrice;
//        if (ressurectType!=LOPT_Free)
//            price = ressurectionPrice * powf(2, player->ressurectionsCount);
//        else
//            price = ressurectionPrice;
        m = getScaleMatrix(SunnyVector3D(0.62*btnScale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef + survivalXTranslate,(1280-748+15)*mapKoef-verticalMapAdd,farDist));
        loRenderPriceLabel(&m, ressurectType, price,resAlpha);
    } else
    if (animationTime - ressurectionButtonTime>0.6)
    {
        moneyAlpha = fminf((animationTime-ressurectionButtonTime-0.6)*5,1);
    }
    
    //if (false)
    {
        //SnowBall
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        if (animationTime<ressurectionButtonTime)
        {
            snowballMatrix = getScaleMatrix(4) * getRotationZMatrix(-rotationTime*3) * getRotationYMatrix(M_PI_4) * getTranslateMatrix(SunnyVector3D(mapSize.x/2 + survivalXTranslate,(1280-370)*mapKoef-verticalMapAdd/2,farDist/2));
            LOPlayer::renderForMenu(&snowballMatrix);
        } else
        if (LOScore::isSpellEnabled(LA_Ressurection) && !hintShowedUp)
        {
            if (isSurvival) glEnable(GL_BLEND);
            
            sunnyUseShader(globalShaders[LOS_Textured3DSnow]);
            m = sunnyIdentityMatrix;
            glUniform4fv(uniform3DS_C, 1, SunnyVector4D(1,1,1, ressurectionAlpha));
            snowBallCrashObjForMenu->renderForMenu(&m);
        }
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        //SnowBall...
    }
    
    //Numbers
    bindGameGlobal();
    if (animationTime>=ressurectioDelay && animationTime<ressurectionButtonTime && !pausedForBuy)
    {
        short number = ressurectionButtonTime-animationTime + 1;
        if (number!=lastPlaySound)
        {
            lastPlaySound = number;
            losPlayDeathSecond();
        }
        float scale = 1.0 + 3*(animationTime - (int)animationTime);
        scale-= 0.4;
        if (scale<1) scale = 1;
        float length = LOFont::getNumberSize(number)*scale;
        m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef - length/2 + survivalXTranslate ,(1280-370)*mapKoef-verticalMapAdd/2,farDist/4));
        LOFont::writeNumber(&m, true, number,SunnyVector4D(255,250,20,255*(1+(1-scale)/1.5)*appearAlpha)/255,SunnyVector4D(245,148,69,255*(1+(1-scale)/1.5)*appearAlpha)/255);
    }
    
    if (moneyAlpha>0)
    {
        float scale = 0.79;
        if (LOScore::getTotalCoins())
        {
            m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef + survivalXTranslate,(1280-680-100+moneyTranslate)*mapKoef-verticalMapAdd,farDist));
            loRenderPriceLabel(&m, LOPT_GoldCoins, (int)LOScore::getTotalCoins(),moneyAlpha);
        }
        if (LOScore::getTotalSnowflakes())
        {
            m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(960*mapKoef + survivalXTranslate,(1280-512-100+moneyTranslate)*mapKoef-verticalMapAdd,farDist));
            loRenderPriceLabel(&m, LOPT_VioletSnowflakes, (int)LOScore::getTotalSnowflakes(),moneyAlpha);
        }
    }
    
    //Game over label
    if (!isSurvival)
        rot = getTranslateMatrix(SunnyVector3D(960*mapKoef,(1280-142)*mapKoef,farDist));
    else
        rot = getTranslateMatrix(SunnyVector3D(960*mapKoef,(1280-100)*mapKoef,farDist));
    LOFont::writeText(&rot, LOTA_Center, true, labelGameOver, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    
    //Save me Label
    if (resAlpha>0)
    {
        float scale = (1.0-cosf(animationTime*3))*0.05;
        rot = getScaleMatrix(0.78*btnScale) * getTranslateMatrix(SunnyVector3D(960*mapKoef + survivalXTranslate,(1280-608)*mapKoef-verticalMapAdd,farDist));
        LOFont::writeText(&rot, LOTA_Center, true, labelSaveMe, SunnyVector4D(255,177,27,255*(resAlpha-scale))/255,SunnyVector4D(241,241,241,255*(resAlpha-scale))/255);
    }
    
    ressurectionAlpha = resAlpha;
    globalMoneyAlpha = moneyAlpha;
}



