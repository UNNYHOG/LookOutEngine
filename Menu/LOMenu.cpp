//
//  LOMenu.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 23.06.13.
//
//

#include "LOMenu.h"
#include "LOGlobalVar.h"
#include "LookOutEngine.h"
#include "iPhone_Strings.h"
#include "LOSounds.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"
#include "LOScore.h"
#include <stdio.h>
#include <stdlib.h>
#include "SunnyFunctions.h"
#include "LOTrace.h"
#include "LOFont.h"

#include "LOTutorialSnowflake.h"
#include "LOTutorialFireball.h"
#include "LOTutorialGroundball.h"
#include "LOTutorialWaterball.h"
#include "LOCoinsPurchased.h"
#include "LOTutorialSpell.h"
#include "LOTutorialMessage.h"
#include "LOTutorialPuzzle.h"
#include "LOSettingsWindow.h"
#include "LOMagicDropInfo.h"

#include "LOSettings.h"

const float buttonStandardScale = 0.9;
const float spellDissapearDelay = 0.4;
float spellRadius;
float *menuOffset;
extern float maxJoysticDistance;
extern float previewAlpha;
#ifndef DO_NOT_RENDER_HUD
const float minimumGamePlayAlpha = 0.4;
#else
const float minimumGamePlayAlpha = 0;
#endif

const SunnyVector2D starAnimationPosition = SunnyVector2D(1.5,mapSize.y-1.5);

LOMenu * sharedMenu = NULL;

LOMenu *LOMenu::getSharedMenu()
{
    return sharedMenu;
}

void updateLevelSelection2()
{
    sharedMenu->updateLevelSelection();
}

void LOMenu::setCurrentPage(short page)
{
    sharedMenu->scrollView->setCurrentPage(page);
}

void LOMenu::showGoldPurchasedWindow(int goldCoins)
{
    closeTutorial();
    tutorial = new LOCoinsPurchased;
    ((LOCoinsPurchased*)tutorial)->initValue(goldCoins);
}

void LOMenu::closeTutorial(bool forcePauseGame)
{
    if (tutorial)
        delete tutorial;
    tutorial = 0;
    
    if (forcePauseGame)
        gamePaused = true;
}

void renderCallbackStepButton(SunnyButton * btn)
{
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    
    if (btn->active)
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, buttonStandardScale, buttonStandardScale));
        glUniform1f(uniformTSA_A, 0.8);
    } else
    {
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        glUniform1f(uniformTSA_A, 1);
    }
    
    SunnyMatrix m = sunnyIdentityMatrix;
    m.pos = btn->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);

    SunnyDrawArrays(LONumbers_VAO+btn->tag);
}

char menuFirstS,menuLastS;

void LOMenu::renderLevelButtonBack()
{
    if (!isLevelSelectionShowed()) return;
    
    SHTextureBase::bindTexture(objectsTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, levelButtonsAlpha);
    
    
    const float scale = 1.6;
    SunnyMatrix m1;
    for (int j = 0;j<loLevelsPerStep;j++)
    {
        SunnyButton *btn = levelsButtons[j];
        SunnyMatrix m = sunnyIdentityMatrix;
        m.pos = btn->matrix.pos;
        m.pos.x -= scrollView->getOffsetX();
        m.pos.x+=(mapZoomBounds.y-mapZoomBounds.x)*menuFirstS;
        
        for (int i = menuFirstS;i<=menuLastS;i++)
        {
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
            SunnyDrawArrays(LOWarnTriangles_VAO+7);
            
            m.pos.x+=(mapZoomBounds.y-mapZoomBounds.x);
        }
    }
    
    if (selectedLevelNum>=0 && selectedStepNum<=menuLastS && LOScore::isMapActive(selectedStepNum*loLevelsPerStep))
    {
        SunnyButton *btn = levelsButtons[selectedLevelNum];
        SunnyVector4D p = btn->matrix.pos;
        p.x += (mapZoomBounds.y-mapZoomBounds.x)*selectedStepNum;
        SunnyVector2D direction = SunnyVector2D(p.x,p.y) - playerTargetPosition;
        float len = direction.length();
        if (len>0.01)
        {
            playerSelectedVelocity = fmaxf(len*2,20);
            playOnBallAlpha = 0;
        }
        playerTargetPosition = SunnyVector2D(p.x,p.y);
        
        p.x -= scrollView->getOffsetX();
        len = 1.0 + playOnBallAlpha/0.8*0.5;
        SunnyMatrix m = getScaleMatrix(SunnyVector3D(len,len,len));
        m.pos = p;
        float angle = getCurrentTime()-menuShowedTime;
        m1 = getRotationZMatrix(angle*20/180*M_PI) * m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m1.front.x));
        SunnyDrawArrays(LOWarnTriangles_VAO+4);
    }
}

void LOMenu::renderSpellOverlay(LevelAbility ability, SunnyMatrix *m, float alpha)
{
#ifndef DO_NOT_RENDER_HUD
    SunnyVector4D color;
    switch (ability) {
        case LA_Life:
            color = SunnyVector4D(248./255,151./255,36./255,alpha);
            break;
        case LA_Teleport:
            color = SunnyVector4D(255./255,243./255,67./255,alpha);
            break;
        case LA_Speed:
            color = SunnyVector4D(186./255,255./255,0./255,alpha);
            break;
        case LA_Shield:
            color = SunnyVector4D(158./255,227./255,255./255,alpha);
            break;
        case LA_Magnet:
            color = SunnyVector4D(21./255,131./255,253./255,alpha);
            break;
        case LA_Time:
            color = SunnyVector4D(255./255,213./255,21./255,alpha);
            break;
        case LA_Doubler:
            color = SunnyVector4D(200./255,32./255,237./255,alpha);
            break;
        default:
            return;
            break;
    }
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1.5,1.5));
    glUniform4fv(uniformOverlayTSA_C, 1, color);
    glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m->front.x));
    SunnyDrawArrays(LOOverlaySpells_VAO);
#endif
}

void LOMenu::renderPuzzleImage(SunnyMatrix *m, float alpha)
{
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, alpha);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m->front.x));
    SunnyDrawArrays(LOPuzzleButton_VAO);
}

void LOMenu::renderSpellButton(LevelAbility ability, SunnyMatrix *m, float staticScale, float activeScale,float offset, bool drawPrice, float alpha, bool enabled)
{
    if (ability>=LA_Doubler)
    {
        SHTextureBase::bindTexture(objectsTexture);
        const float mapKoef = mapSize.x/1920;
        
        if (enabled)
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniform1f(uniformTSA_A, alpha);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(offset, 0, staticScale, staticScale));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m->front.x));
        } else
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA_Middle]);
            glUniform1f(uniformTSAMiddle_A, 0.5);
            glUniform4fv(uniformTSAMiddle_TR, 1, SunnyVector4D(offset, 0, staticScale, staticScale));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA_Middle], 1, GL_FALSE, &(m->front.x));
        }
       
        short num = 0;
        SunnyVector2D move(0,0);
        LOPriceType type;
        unsigned short price;
        SunnyVector2D pricePos;
        float numberWidth;
        if (ability == LA_Magnet)
        {
            num = 8;
            move.x = 14;
            type  = LOScore::getPrice(LA_Magnet, price);
            //pricePos = SunnyVector2D(1284,1280-634)*mapKoef;
        }
        else
            if (ability == LA_Time)
            {
                num = 10;
                move = SunnyVector2D(14,4);
                type  = LOScore::getPrice(LA_Time, price);
                //pricePos = SunnyVector2D(1602,1280-634)*mapKoef;
            }
            else
                if (ability == LA_Doubler)
                {
                    num = 12;
                    type  = LOScore::getPrice(LA_Doubler, price);
                    move.x = 20;
                    //pricePos = SunnyVector2D(1436,1280-884)*mapKoef;
                }
        pricePos = SunnyVector2D(m->pos.x + 8*mapKoef,m->pos.y - 118*mapKoef);
        SunnyDrawArrays(LOSpellButtons_VAO + num);
        
        if (enabled)
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(move.x/640./2*mapSize.y + offset, move.y/640./2*mapSize.y, activeScale, activeScale));
        else
            glUniform4fv(uniformTSAMiddle_TR, 1, SunnyVector4D(move.x/640./2*mapSize.y + offset, move.y/640./2*mapSize.y, staticScale, staticScale));
        SunnyDrawArrays(LOSpellButtons_VAO + num +1);
        
        if (drawPrice)
        {
            //Number
            SunnyVector4D innerColor(1,1,1,1);
            SunnyVector4D outerColor(0.4,0.4,0.4,1);
            const float labelScale = 0.4;
            numberWidth = LOFont::getNumberSize(price)*labelScale;
            
            float goldW = goldCoinImageWidth;
            if (type == LOPT_Free)
                goldW = 0;
            
            SunnyMatrix m1 = getScaleMatrix(labelScale) * getTranslateMatrix(SunnyVector3D(pricePos.x  - (numberWidth-goldW*labelScale)/2 + offset,pricePos.y,-5));
            LOFont::writeNumber(&m1, true, price, innerColor, outerColor);
            
            if (type != LOPT_Free)
            {
                sunnyUseShader(globalShaders[LOS_TexturedTSA]);
                m1 = getTranslateMatrix(SunnyVector3D(-goldCoinImageWidth/2,0,0))*m1;
                glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m1.front.x));
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
                SunnyDrawArrays(LOGoldCoin_VAO);
            }
        }
    } else
    {
#ifndef DO_NOT_RENDER_HUD
        SHTextureBase::bindTexture(objectsTexture);
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        glUniform1f(uniformTSA_A, 0.7*alpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, staticScale, staticScale));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m->front.x));
        SunnyDrawArrays(LOSpellButtons_VAO + 3 - (int)ability);
        
        if (ability == LA_Speed)
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-18./640*mapSize.y*activeScale, -4./640*mapSize.y*activeScale, activeScale, activeScale));
        else
            if (ability == LA_Teleport)
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(9./640*mapSize.y*activeScale, 10./640*mapSize.y*activeScale, activeScale, activeScale));
            else
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, activeScale, activeScale));
        SunnyDrawArrays(LOSpellButtons_VAO + 3 + 4 - (int)ability);
#endif
    }
}

void LOMenu::renderLevelButton(SunnyButton * btn)
{
    const float mapKoef = mapSize.x/1920;
    
    if (btn == survivalButtons[LOSB_Back])
    {
        return;
    } else
        if (btn == survivalButtons[LOSB_Play])
        {
            return;
        } else
            if (btn == survivalButtons[LOSB_SpellDoubler] || btn == survivalButtons[LOSB_SpellMagnet] || btn == survivalButtons[LOSB_SpellTime])
            {
                for (short p = menuFirstS;p<=menuLastS;p++)
                {
                    float offset = -scrollView->getOffsetX() + scrollView->getFrame().x*p;
                    bindGameGlobal();
                    SunnyMatrix m = sunnyIdentityMatrix;
                    float scale = 1;
                    m.pos = btn->matrix.pos;
                    if (btn->active)
                        scale *= 1.2;
                    LevelAbility ability;
                    if (btn==survivalButtons[LOSB_SpellMagnet])
                        ability = LA_Magnet;
                    else
                        if (btn==survivalButtons[LOSB_SpellTime])
                            ability = LA_Time;
                        else
                            //if (btn==survivalButtons[LOSB_SpellDoubler])
                                ability = LA_Doubler;
                    
                    float scale2 = scale;
                    if (btn->active)
                        scale2 *= (0.75-0.25*sin((getCurrentTime()-btn->tag*0.5)*M_PI));

                    bool enabled = (btn->enable && LOScore::isSpellEnabled(ability) && LOScore::isStageEnabled(p));
                    renderSpellButton(ability,&m,scale,scale2,offset,enabled,1,enabled);
                }
                return;
            } else
    if (btn == levelsButtons[loLevelsPerStep])
    {
        SHTextureBase::bindTexture(menuTexture);
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        
        float scale = 1;
        if (btn->active)
            scale *= buttonStandardScale;
        glUniform1f(uniformTSA_A, levelButtonsAlpha);
        
        SunnyMatrix m = sunnyIdentityMatrix;
        m.pos = btn->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
        
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-2*mapKoef, 4*mapKoef, scale, scale));
        SunnyDrawArrays(LOLCButtonsVAO+6);
        return;
    }
    
    if (menuFirstS<0) return;
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSC]);
    
    float scale = 0.5;
    if (btn->active)
    {
        scale *= buttonStandardScale;
    }
    
    SunnyMatrix m = sunnyIdentityMatrix;
    m.pos = btn->matrix.pos;
    m.pos.x -= scrollView->getOffsetX();
    m.pos.x+=(mapZoomBounds.y-mapZoomBounds.x)*menuFirstS;
    for (int i = menuFirstS;i<=menuLastS;i++)
    {
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, m.front);
        
        short color = numbersCount;
        
        if (LOScore::isMapActive(i*loLevelsPerStep + btn->tag-1))
        {
            glUniform4fv(uniformTSC_C, 1, SunnyVector4D(1,1,1,levelButtonsAlpha));
            btn->enable = true;
        }
        else
        {
            glUniform4fv(uniformTSC_C, 1, SunnyVector4D(0.15,0.15,0.15,levelButtonsAlpha));
            btn->enable = false;
        }

        if (btn->tag<10)
        {
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(0, 0, scale, scale));
            SunnyDrawArrays(LONumbers_VAO + color + btn->tag);
        } else
        {
            float pixelToWorld2 = pixelToWorld*scale;
            short n1 = btn->tag/10;
            short n2 = btn->tag%10;
            float totalSize = numbersSizes[n1] + numbersSizes[n2];
            
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D((-numbersSizes[n1]/2 + (numbersSizes[n1]-totalSize/2))*pixelToWorld2, 0, scale, scale));
            SunnyDrawArrays(LONumbers_VAO + color + n1);
            glUniform4fv(uniformTSC_TR, 1, SunnyVector4D((numbersSizes[n2]/2 - (numbersSizes[n2]-totalSize/2))*pixelToWorld2, 0, scale, scale));
            SunnyDrawArrays(LONumbers_VAO + color + n2);
        }
        
        m.pos.x+=(mapZoomBounds.y-mapZoomBounds.x);
    }
}

void renderCallbackEmpty(SunnyButton * btn)
{
}

void renderCallbackLevelButton(SunnyButton * btn)
{
    sharedMenu->renderLevelButton(btn);
}

void renderCallbackSpellButton2(SunnyButton * btn)
{
    sharedMenu->renderCallbackSpellButton(btn);
}

void LOMenu::renderCallbackSpellButton(SunnyButton * btn)
{
    if (menuMode) return;
#ifdef DO_NOT_RENDER_HUD
    return;
#endif
    
    
    SunnyMatrix m = sunnyIdentityMatrix;
    float scale = 0.9;
    if (screenSize.y>=750) scale *= 0.7;
    float numberScale = scale;

    SHTextureBase::bindTexture(objectsTexture);

    {
        m.pos = btn->matrix.pos;
        if (btn->active)
            scale *= 1.5;
        
        if (btn->enable && !player->isAbilityActive((LevelAbility)btn->tag))
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            glUniform1f(uniformTSA_A, 0.7);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        } else
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA_Middle]);
            glUniform1f(uniformTSAMiddle_A, 0.5);
            glUniform4fv(uniformTSAMiddle_TR, 1, SunnyVector4D(0, 0, scale, scale));
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA_Middle], 1, GL_FALSE, &(m.front.x));
        }
        SunnyDrawArrays(LOSpellButtons_VAO + 3 - btn->tag);
        
        if (btn->enable)
        {
            scale *= (0.75-0.25*sinf((realPlayingTime-btn->tag*0.5)*M_PI));
            if (btn==spellButtons[LOSpellSpeed])
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-18./640*mapSize.y*scale, -4./640*mapSize.y*scale, scale, scale));
            else
            if (btn==spellButtons[LOSpellTeleport])
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(9./640*mapSize.y*scale, 10./640*mapSize.y*scale, scale, scale));
            else
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
        } else
        {
            if (btn->tag==LOSpellSpeed) glUniform4fv(uniformTSAMiddle_TR, 1, SunnyVector4D(-18./640*mapSize.y*scale, -4./640*mapSize.y*scale, scale, scale));
            else glUniform4fv(uniformTSAMiddle_TR, 1, SunnyVector4D(0, 0, scale, scale));
        }
        SunnyDrawArrays(LOSpellButtons_VAO + 3 + 4 - btn->tag);
        
        //Number
        if (btn->enable)
        {
            //SunnyMatrix m1 = getTranslateMatrix(SunnyVector3D(-1.2,-0.5,0)*0.7) * btn->matrix;
            float tr;
            LOSettings::isLeftHanded()?tr = 1.8:tr=-1.8;
            SunnyMatrix m1 = getTranslateMatrix(SunnyVector3D(tr,-1,0)*numberScale) * m;
            m = getScaleMatrix(0.3*numberScale/0.63) * m1;
            if (priceType[btn->tag]!=LOPT_Free || price[btn->tag]!=0)
                LOFont::writeNumber(&m, true, price[btn->tag], SunnyVector4D(1,1,1,1), SunnyVector4D(0.4,0.4,0.4,1));
            
            if (priceType[btn->tag]==LOPT_GoldCoins)
            {
                sunnyUseShader(globalShaders[LOS_TexturedTSA]);
                m = getTranslateMatrix(SunnyVector3D(-goldCoinImageWidth/2,0,0))*m;
                glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
                glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
                SunnyDrawArrays(LOGoldCoin_VAO);
            } else
                if (priceType[btn->tag]==LOPT_VioletSnowflakes)
                {
                    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
                    m = getTranslateMatrix(SunnyVector3D(-snowflakeImageWidth/2,0,0))*m;
                    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
                    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
                    SunnyDrawArrays(LOVioletSnowflake_VAO);
                }
        }
    }
}

void LOMenu::renderOverlay()
{
    glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1,1));
    
    if (!menuMode)
    {
#ifndef DO_NOT_RENDER_HUD
        for (int i = 0;i<LOSpellCount;i++)
        if (!spellButtons[i]->hidden && spellButtons[i]->enable && !player->isAbilityActive((LevelAbility)i))
        {
            SunnyVector4D color;
            switch (i+1) {
                case 1:
                    color = SunnyVector4D(248./255,151./255,36./255,1);
                    break;
                case 2:
                    color = SunnyVector4D(255./255,243./255,67./255,1);
                    break;
                case 3:
                    color = SunnyVector4D(186./255,255./255,0./255,1);
                    break;
                case 4:
                    color = SunnyVector4D(158./255,227./255,255./255,1);
                    break;
                default:
                    break;
            }
            glUniform4fv(uniformOverlayTSA_C, 1, color);
            glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(spellButtons[i]->matrix.front.x));
            SunnyDrawArrays(LOOverlaySpells_VAO);
        }
#endif
    }
    
    if (isSurvivalSelectionShowed())
    {
        for (short p = menuFirstS;p<=menuLastS;p++)
        {
            if (!LOScore::isStageEnabled(p)) continue;
            float offset = -scrollView->getOffsetX() + scrollView->getFrame().x*p;

            float scale = 0.55;
            glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
            SunnyMatrix m;
            if (LOScore::isSpellEnabled(LA_Magnet))
            {
                glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(21./255,131./255,253./255,1));
                m = survivalButtons[LOSB_SpellMagnet]->matrix;
                m.pos.x +=offset;
                glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(LOOverlaySpells_VAO);
            }
            
            if (LOScore::isSpellEnabled(LA_Time))
            {
                glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,213./255,21./255,1));
                m = survivalButtons[LOSB_SpellTime]->matrix;
                m.pos.x +=offset;
                glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(LOOverlaySpells_VAO);
            }
            
            if (LOScore::isSpellEnabled(LA_Doubler))
            {
                glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(200./255,32./255,237./255,1));
                m = survivalButtons[LOSB_SpellDoubler]->matrix;
                m.pos.x +=offset;
                glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
                SunnyDrawArrays(LOOverlaySpells_VAO);
            }
        }
    } else
    if (isLevelSelectionShowed())
    {
        //SnowBall
        SunnyMatrix rot = sunnyIdentityMatrix;
        rot.pos.z = -1;
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(1,1,1,1));
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(playerPositionInLevels.pos.x - scrollView->getOffsetX(),playerPositionInLevels.pos.y,2,2));
        SunnyDrawArrays(LOOverlaySnowflake_VAO);
    }
    
    mainWindow->renderOverlay();
    levelComplete->renderOverlay();
    deathWindow->renderOverlay();
    if (!tutorial || !tutorial->blockRenderOverlays())
        rewardView->renderOverlay();
    if (!isAnyWindowOn())
        if (store)
            store->renderOverlay();
    if (tutorial)
        tutorial->renderOverlay();
    tasksView->renderOverlay();
    if (survComplete)
        survComplete->renderOverlay();
    
    if (isStarAnimated)
    {
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(0,0,-1));
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,210./255,0,starAnimationScale*.5));
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(starAnimationPosition.x-horizontalMapAdd,starAnimationPosition.y+verticalMapAdd,1,1));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
}

bool LOMenu::isAnyWindowOn()
{
    return (rewardView->isVisible || messageWindow->isVisible || tutorial);
}

bool LOMenu::isTutorial()
{
    return (tutorial!=NULL);
}

/*void renderCallbackPauseButton(SunnyButton * btn)
{
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    float scale = 1;
    if (btn->active)
        scale = buttonStandardScale;
    SunnyMatrix m = sunnyIdentityMatrix;
    m.pos = btn->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, m.front);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
    SunnyDrawArrays(LOGameplayButtons_VAO+1);
}*/

void LOMenu::renderLevelSelectionLastPage()
{
    if (previewAlpha>=1) return;
    
    float alpha = 1 - previewAlpha;
    
    float offset = (scrollView->getPagesCount()-1)*scrollView->getFrame().x - scrollView->getOffsetX();
    
    const float mapKoef = mapSize.x/1920;
    SunnyMatrix m;
    bindGameGlobal();
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    SHTextureBase::bindTexture(menuTexture);
    m = getTranslateMatrix(SunnyVector3D(offset,0,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform1f(uniformTSA_A, alpha);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,1,1));
   
    //Buttons
    {
        m = getTranslateMatrix(SunnyVector3D(0,0,-5));
        float btnScale;
        SunnyMatrix rot;
        //FB
        rot = m;
        rot.pos = socialButtons[LOSocialButton_FB]->matrix.pos;
        socialButtons[LOSocialButton_FB]->active?btnScale = defaultButtonScale:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0+offset, 0, btnScale, btnScale));
        SunnyDrawArrays(LOSocialButtonBack_VAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(48*mapKoef+offset, -22*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOSocialButtonFB_VAO);

        //TW
        rot = m;
        rot.pos = socialButtons[LOSocialButton_TW]->matrix.pos;
        socialButtons[LOSocialButton_TW]->active?btnScale = defaultButtonScale:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0+offset, 0, btnScale, btnScale));
        SunnyDrawArrays(LOSocialButtonBack_VAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(2*mapKoef+offset, 6*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOSocialButtonTW_VAO);
        //VK
        rot = m;
        rot.pos = socialButtons[LOSocialButton_VK]->matrix.pos;
        socialButtons[LOSocialButton_VK]->active?btnScale = defaultButtonScale:btnScale = 1;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0+offset, 0, btnScale, btnScale));
        SunnyDrawArrays(LOSocialButtonBack_VAO);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(10*mapKoef+offset, 6*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOSocialButtonVK_VAO);
    }
    
    float scale = .52;
    m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(960*mapKoef + offset,(1280-556)*mapKoef,-2));
    LOFont::writeText(&m, LOTA_Center, true, labelStayTuned,SunnyVector4D(255,245,58,255*alpha)/255,SunnyVector4D(254,162,47,255*alpha)/255);
    
    scale = .86;
    m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(1032*mapKoef + offset,(1280-238)*mapKoef,-2));
    LOFont::writeText(&m, LOTA_Center, true, labelToBeContinued,SunnyVector4D(255,255,255,255*alpha)/255,SunnyVector4D(153,167,182,255*alpha)/255);
    
    m.pos.x += LOFont::getTextSize(labelToBeContinued)*scale/2;
    bindGameGlobal();
    LOFont::writeTime(&m, true, "...",SunnyVector4D(255,255,255,255*alpha)/255,SunnyVector4D(153,167,182,255*alpha)/255);
}

void LOMenu::renderLevelSelection()
{
    SunnyMatrix m;
    
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    
    SHTextureBase::bindTexture(menuTexture);
    short j = scrollView->getCurrentPage();
    //Preview
    glUniform1f(uniformTSA_A, previewAlpha);
    m = getTranslateMatrix(SunnyVector3D(980./1920*mapSize.x,(1280+100)/1280.*mapSize.y,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    SunnyDrawArrays(LOMapPreviewVAO);
    //Preview
    
    SunnyMatrix leftArrowMat;
    SunnyMatrix rotationMatrix = getRotationZMatrix(4./180*M_PI);
    //Arrows
    {
        if (scrollView->getOffsetX()>=(mapZoomBounds.y-mapZoomBounds.x))
            glUniform1f(uniformTSA_A, 1);
        else
            glUniform1f(uniformTSA_A, scrollView->getOffsetX()/(mapZoomBounds.y-mapZoomBounds.x));
        m = rotationMatrix* getTranslateMatrix(SunnyVector3D((108)/1920.*mapSize.x,(1280-237)/1280.*mapSize.y,-1));
        leftArrowMat = m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (scrollButtons[0]->active)
        {
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, defaultButtonScale, defaultButtonScale));
            SunnyDrawArrays(LOLSArrowVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        } else
            SunnyDrawArrays(LOLSArrowVAO);
    }
    {
        glUniform1f(uniformTSA_A, previewAlpha);
        //m = getRotationZMatrix(-7./180*M_PI)* getScaleMatrix(SunnyVector3D(-1,1,1))* getTranslateMatrix(SunnyVector3D(1013./1920.*mapSize.x,(1280-184)/1280.*mapSize.y,-1));
        m = getScaleMatrix(SunnyVector3D(-1,1,1)) * getTranslateMatrix(SunnyVector3D(14,0,0)) * leftArrowMat;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (scrollButtons[1]->active)
        {
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, defaultButtonScale, defaultButtonScale));
            SunnyDrawArrays(LOLSArrowVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        } else
            SunnyDrawArrays(LOLSArrowVAO);
    }
    
    //Arrows...
    
    //Dots
    for (short i=0;i<scrollView->getPagesCount();i++)
    {
        m = getRotationZMatrix(78./180*M_PI*i)* getTranslateMatrix(SunnyVector3D((802.+122*i)/1920*mapSize.x,(1280-1188)/1280.*mapSize.y,-1));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (i==j)
            glUniform1f(uniformTSA_A, 1);
        else
            glUniform1f(uniformTSA_A, 0.5);
        SunnyDrawArrays(LOLSDotVAO);
    }
    //Dots...
    
    //Coins
    if (menuFirstS>=0)
    {
        glEnable(GL_CULL_FACE);

        float blickAngle = -M_PI_2;
        float animationTime = getCurrentTime() - menuShowedTime;
        if (animationTime > nextCoinAnimationTime && LOScore::getStageCoins(menuFirstS)>0)
        {
            if (nextAnimatedCoin<0)
                nextAnimatedCoin = sRandomi()%LOScore::getStageCoins(menuFirstS);
            
            blickAngle += (animationTime - nextCoinAnimationTime)*5;
            
            if (blickAngle + M_PI_2 > M_PI*1.3)
            {
                nextCoinAnimationTime = animationTime + 1 + sRandomf();
                nextAnimatedCoin = -1;
                blickAngle = -M_PI_2;
            }
        }
        
        const float smallScale = 0.9;
        const float bigScale = 1.3;
        glEnable(GL_DEPTH_TEST);
        LOGoldCoin::prepareForRender();
        SunnyMatrix rot = getRotationXMatrix(-M_PI/15) * getRotationYMatrix(-M_PI/15);
        SunnyMatrix rotB = getScaleMatrix(SunnyVector3D(bigScale,bigScale,bigScale))* rot;
        rot = getScaleMatrix(SunnyVector3D(smallScale,smallScale,smallScale))* rot;
        float coinNumber = 0;
        for (short k = menuFirstS;k<=menuLastS;k++)
        {
            float offset = -scrollView->getOffsetX() + k*(mapZoomBounds.y-mapZoomBounds.x);
            for (short i = 0;i<loLevelsPerStep;i++)
            {
                short coins = LOScore::getLevelCoins(k, i);
                if (coins>0)
                {
                    m = rot;
                    m.pos = levelsButtons[i]->matrix.pos + SunnyVector4D(0.1+offset,1.3,1,0);
                    if (nextAnimatedCoin == coinNumber++) LOGoldCoin::renderModelForMenu(&m,levelButtonsAlpha,blickAngle);
                    else LOGoldCoin::renderModelForMenu(&m,levelButtonsAlpha);
                    if (coins>1)
                    {
                        m.pos = levelsButtons[i]->matrix.pos + SunnyVector4D(1.3+offset,0.6,1,0);
                        if (nextAnimatedCoin == coinNumber++) LOGoldCoin::renderModelForMenu(&m,levelButtonsAlpha,blickAngle);
                        else LOGoldCoin::renderModelForMenu(&m,levelButtonsAlpha);
                        if (coins>2)
                        {
                            m = rotB;
                            m.pos = levelsButtons[i]->matrix.pos + SunnyVector4D(0.7+offset,1.3,2,0);
                            if (nextAnimatedCoin == coinNumber++) LOGoldCoin::renderModelForMenu(&m,levelButtonsAlpha,blickAngle);
                            else LOGoldCoin::renderModelForMenu(&m,levelButtonsAlpha);
                        }
                    }
                }
            }
        }
        //SnowBall
        glDisable(GL_BLEND);
        rot = playerPositionInLevels;
        rot.pos.x -= scrollView->getOffsetX();
        LOPlayer::renderForMenu(&rot);
        glEnable(GL_BLEND);
        //SnowBall...
        
        glDisable(GL_CULL_FACE);

        
        bindGameGlobal();
        const float shadow = 0.2;
        sunnyUseShader(globalShaders[LOS_TexturedTSC]);
        glUniform4fv(uniformTSC_C, 1, SunnyVector4D(1,1,1,0.7*levelButtonsAlpha));
        glUniform4fv(uniformTSC_TR, 1, SunnyVector4D(-shadow, -shadow,1.3,1.3));
        
        rot = getScaleMatrix(SunnyVector3D(smallScale,smallScale,smallScale));
        rotB = getScaleMatrix(SunnyVector3D(bigScale,bigScale,bigScale));
        
        SHTextureBase::bindTexture(objectsTexture);
        for (short k = menuFirstS;k<=menuLastS;k++)
        {
            float offset = -scrollView->getOffsetX() + k*(mapZoomBounds.y-mapZoomBounds.x);
            for (short i = 0;i<loLevelsPerStep;i++)
            {
                short coins = LOScore::getLevelCoins(k, i);
                if (coins>0)
                {
                    m = rot;
                    m.pos = levelsButtons[i]->matrix.pos + SunnyVector4D(0.1+offset,1.3,0.5,0);
                    glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
                    SunnyDrawArrays(LOShadows_VAO);
                    if (coins>1)
                    {
                        m.pos = levelsButtons[i]->matrix.pos + SunnyVector4D(1.3+offset,0.6,0.5,0);
                        glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
                        SunnyDrawArrays(LOShadows_VAO);
                        if (coins>2)
                        {
                            m = rotB;
                            m.pos = levelsButtons[i]->matrix.pos + SunnyVector4D(0.7+offset,1.3,1.5,0);
                            glUniformMatrix4fv(globalModelview[LOS_TexturedTSC], 1, GL_FALSE, &(m.front.x));
                            SunnyDrawArrays(LOShadows_VAO);
                        }
                    }
                }
            }
        }
        
        SunnyMatrix shadowM = sunnyIdentityMatrix;
        shadowM.pos = playerPositionInLevels.pos;
        shadowM.pos.x -= scrollView->getOffsetX();
        shadowM.pos.z -= 5;
        activeMap->mapInfo.renderPlayerShadow(&shadowM);
        
        SHTextureBase::bindTexture(menuTexture);
        glDisable(GL_DEPTH_TEST);
        
        if (playOnBallAlpha>0)
        {
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            float t = 0.2*sin(getCurrentTime()*2);
            glUniform1f(uniformTSA_A, playOnBallAlpha + t);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(shadowM.front.x));
            t = 0.3*(1-t/2);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0.2, 0, t, t));
            SunnyDrawArrays(LOLSPlayButtonVAO);
        }
    }
    //Coins...
    
    
    if (selectedStepNum<loStepsCount)
    {
        //Number
        {
            bindGameGlobal();
            char number[3];
            number[0] = '1' + selectedStepNum;
            number[1] = '.';
            number[2] = '\0';
            
            float size = LOFont::getTimeSize(number);
            leftArrowMat = getTranslateMatrix(SunnyVector3D(size/4,0,0)) * getScaleMatrix(0.5) * leftArrowMat;
            LOFont::writeTime(&leftArrowMat, true, number,SunnyVector4D(255,172,42,255*previewAlpha)/255,SunnyVector4D(241,241,241,255*previewAlpha)/255);
            leftArrowMat = getTranslateMatrix(SunnyVector3D(size*1.2,0,0)) * leftArrowMat;
        }
        
        const float mapKoef = mapSize.x/1920;
        if (selectedStepNum<loStepsCount)
        {
            SunnyMatrix coinsMatrix;
            SunnyMatrix snowflakeMatrix;

            //GoldCoin
            if (LOScore::getTotalCoins())
            {
                coinsMatrix = getScaleMatrix(0.86) * rotationMatrix * getTranslateMatrix(SunnyVector3D((796-100)*mapKoef,(1280-390-30)*mapKoef,-1));
                loRenderPriceLabel(&coinsMatrix, LOPT_GoldCoins, (int)LOScore::getTotalCoins(),previewAlpha,LOTA_LeftCenter);
            }
            
            //Snowflake
            if (LOScore::getTotalSnowflakes())
            {
                snowflakeMatrix = getScaleMatrix(0.86) * rotationMatrix * getTranslateMatrix(SunnyVector3D((230-100)*mapKoef,(1280-426-30)*mapKoef,-1));
                loRenderPriceLabel(&snowflakeMatrix, LOPT_VioletSnowflakes, (int)LOScore::getTotalSnowflakes(),previewAlpha,LOTA_LeftCenter);
            }
        }
        
        //Label
        LOFont::writeText(&leftArrowMat, LOTA_LeftTop, true, episodeWName,SunnyVector4D(255,172,42,255*previewAlpha)/255,SunnyVector4D(241,241,241,255*previewAlpha)/255);
    }
    
    renderLevelSelectionLastPage();
}

void LOMenu::renderLevelSelectionSurvival()
{
    const float mapKoef = mapSize.x/1920;
    
    SunnyMatrix m;
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    SHTextureBase::bindTexture(menuTexture);
    glUniform1f(uniformTSA_A, previewAlpha);
    
    SunnyMatrix rot = sunnyIdentityMatrix;
    float btnScale = 1;
    if (LOScore::isStageEnabled(selectedStepNum))
    {
        survivalButtons[LOSB_Play]->active?btnScale = defaultButtonScale:btnScale = 1;
        rot.pos = survivalButtons[LOSB_Play]->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO);
        btnScale *= (1.05 - 0.05*cos(2*getCurrentTime()));
        glUniform1f(uniformTSA_A, (1-(btnScale-1)*2)*previewAlpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(8*mapKoef, 4*mapKoef, btnScale, btnScale));
        SunnyDrawArrays(LOLCButtonsVAO+4);
    }
    
    survivalButtons[LOSB_Back]->active?btnScale = defaultButtonScale:btnScale = 1;
    rot.pos = survivalButtons[LOSB_Back]->matrix.pos;
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(rot.front.x));
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
    glUniform1f(uniformTSA_A, 1);
    SunnyDrawArrays(LOLCButtonsVAO);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-4*mapKoef, 6*mapKoef, btnScale, btnScale));
    SunnyDrawArrays(LOLCButtonsVAO+6);
    
    btnScale = 1.1;
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, btnScale, btnScale));
    for (short p = menuFirstS;p<=menuLastS;p++)
    {
        if (!LOScore::isStageEnabled(p)) continue;
        float offset = -scrollView->getOffsetX() + scrollView->getFrame().x*p;

        m = getScaleMatrix(0.55) * getTranslateMatrix(SunnyVector3D(1582./1920.*mapSize.x + offset,(1280-302)/1280.*mapSize.y,-1));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOLCClock_VAO);
    }
    
    m = getTranslateMatrix(SunnyVector3D(0,0,-1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyVector2D coinsPos = SunnyVector2D(1164,1280-1076)*mapKoef;
    SunnyVector2D snowflakesPos = SunnyVector2D(648,1280-1076)*mapKoef;
    
    SunnyMatrix leftArrowMat;
    //Arrows
    {
        if (scrollView->getOffsetX()>=(mapZoomBounds.y-mapZoomBounds.x))
            glUniform1f(uniformTSA_A, 1);
        else
            glUniform1f(uniformTSA_A, scrollView->getOffsetX()/(mapZoomBounds.y-mapZoomBounds.x)*previewAlpha);
        m = getTranslateMatrix(SunnyVector3D((466)/1920.*mapSize.x,(1280-92)/1280.*mapSize.y + verticalMapAdd,-1));
        leftArrowMat = m;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (scrollButtonsSurv[0]->active)
        {
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, defaultButtonScale, defaultButtonScale));
            SunnyDrawArrays(LOLSArrowVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        } else
        {
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
            SunnyDrawArrays(LOLSArrowVAO);
        }
    }
    {
        glUniform1f(uniformTSA_A, previewAlpha);
        //m = getRotationZMatrix(-7./180*M_PI)* getScaleMatrix(SunnyVector3D(-1,1,1))* getTranslateMatrix(SunnyVector3D(1013./1920.*mapSize.x,(1280-184)/1280.*mapSize.y,-1));
        m = getScaleMatrix(SunnyVector3D(-1,1,1)) * getTranslateMatrix(SunnyVector3D(14,0,0)) * leftArrowMat;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (scrollButtonsSurv[1]->active)
        {
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, defaultButtonScale, defaultButtonScale));
            SunnyDrawArrays(LOLSArrowVAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        } else
            SunnyDrawArrays(LOLSArrowVAO);
    }
    
    short j = scrollView->getCurrentPage();
    //Arrows...
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    //Dots
    for (short i=0;i<scrollView->getPagesCount();i++)
    {
        m = getRotationZMatrix(78./180*M_PI*i)* getTranslateMatrix(SunnyVector3D((802.+122*i)/1920*mapSize.x,(1280-1188)/1280.*mapSize.y,-1));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        if (i==j)
            glUniform1f(uniformTSA_A, 1);
        else
            glUniform1f(uniformTSA_A, 0.5);
        SunnyDrawArrays(LOLSDotVAO);
    }
    //Dots...
    
    //Number
    float scale = 0.79;
    {
        if (selectedStepNum<loStepsCount)
        {
            char number[3];
            number[0] = '1' + selectedStepNum;
            number[1] = '.';
            number[2] = '\0';
            
            float size = LOFont::getTimeSize(number);
            leftArrowMat = getTranslateMatrix(SunnyVector3D(size/4,0,0)) * getScaleMatrix(0.5) * leftArrowMat;
            LOFont::writeTime(&leftArrowMat, true, number,SunnyVector4D(255,172,42,255*previewAlpha)/255,SunnyVector4D(241,241,241,255*previewAlpha)/255);
            leftArrowMat = getTranslateMatrix(SunnyVector3D(size*1.2,0,0)) * leftArrowMat;
        }
        
        for (short p = menuFirstS;p<=menuLastS;p++)
        {
            if (!LOScore::isStageEnabled(p)) continue;
            float offset = -scrollView->getOffsetX() + scrollView->getFrame().x*p;
            m = getScaleMatrix(0.55) * getTranslateMatrix(SunnyVector3D(1650./1920.*mapSize.x + offset,(1280-304)/1280.*mapSize.y,-1));
            LOFont::writeTime(&m, true, LOScore::getSurvivalRecord(selectedStepNum),SunnyVector4D(255,255,255,255)/255,SunnyVector4D(186,196,204,255)/255);
        }
        
        if (LOScore::getTotalCoins())
        {
            m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(coinsPos.x,coinsPos.y,-5));
            loRenderPriceLabel(&m, LOPT_GoldCoins, (int)LOScore::getTotalCoins(), previewAlpha);
        }
        
        if (LOScore::getTotalSnowflakes())
        {
            m = getScaleMatrix(SunnyVector3D(scale)) * getTranslateMatrix(SunnyVector3D(snowflakesPos.x,snowflakesPos.y,-5));
            loRenderPriceLabel(&m, LOPT_VioletSnowflakes, (int)LOScore::getTotalSnowflakes(),previewAlpha);
        }
    }

    //Episode Name
    if (selectedStepNum<loStepsCount)
        LOFont::writeText(&leftArrowMat, LOTA_LeftTop, true, episodeWName,SunnyVector4D(255,172,42,255*previewAlpha)/255,SunnyVector4D(241,241,241,255*previewAlpha)/255);
    
    for (short p = menuFirstS;p<=menuLastS;p++)
    {
        if (!LOScore::isStageEnabled(p)) continue;
        float offset = -scrollView->getOffsetX() + scrollView->getFrame().x*p;
        m = getTranslateMatrix(SunnyVector3D(labelTimePosX,0,0)) * getScaleMatrix(0.90/2) * getTranslateMatrix(SunnyVector3D(1268./1920*mapSize.x+offset+3.8,(1280-304)/1280.*mapSize.y,-1));
        LOFont::writeText(&m, LOTA_LeftCenter, true, labelTimeRecord,SunnyVector4D(255,255,255,255)/255,SunnyVector4D(186,196,204,255)/255);
    }

    renderLevelSelectionLastPage();
}

void LOMenu::animateStar()
{
    isStarAnimated = true;
    starAnimationTime = 0;
    losPlayStarSound(true);
}

void LOMenu::render()
{
    //if (!gamePauseButton->hidden || !menuMode)
    if (isStarAnimated)
    {
        float k = 0.7;
        float s = 1.3;
        if (starAnimationTime<taskViewStarAppearTime*k)
            starAnimationScale = s * (starAnimationTime/(taskViewStarAppearTime*k));
        else
            if (starAnimationTime<taskViewStarAppearTime)
            {
                starAnimationScale = s - (s-1)*(starAnimationTime-taskViewStarAppearTime*k)/(taskViewStarAppearTime*(1-k));
            } else
            {
                if (starAnimationTime<=3)
                    starAnimationScale = 1;
                else
                {
                    if (starAnimationTime<3.5)
                        starAnimationScale = (3.5-starAnimationTime)/0.5;
                    else
                    {
                        isStarAnimated = false;
                    }
                }
            }
        
        bindGameGlobal();
        SHTextureBase::bindTexture(menuTexture);
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(starAnimationPosition.x-horizontalMapAdd,starAnimationPosition.y+verticalMapAdd,-1));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform1f(uniformTSA_A, starAnimationScale);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0,0,starAnimationScale,starAnimationScale));
        SunnyDrawArrays(LOYellowStar_VAO);
    }
    
    if (!menuMode)
    {
#ifndef DO_NOT_RENDER_HUD
        
        float newScale = 1;
        SunnyVector4D color = SunnyVector4D(1,1,1,1);
        if (isSurvival)
        {
            if (playingTime>=activeMap->previousSurvivalRecord)
            {
                if (playingTime-activeMap->previousSurvivalRecord<2*M_PI/7)
                {
                    newScale += (1 - cosf((playingTime-activeMap->previousSurvivalRecord)*7))/2*0.2;
                }
                color = yellowOuterColor(1);
            }
        }
        //Playing time
        char time[30];
        unsigned int tt = playingTime;
        float scale = 0.5*newScale;
        float w;
        if (tt<60)
        {
            sprintf(time, "%d",tt);
            strcat(time, ".");
            int len = (int)strlen(time);
            int t = ((int)(playingTime*100))%100;
            time[len] = '0' + t/10;
            time[len+1] = '0' + t%10;
            time[len+2] = '\0';
            w = numbersSizes[0]*pixelToWorld*strlen(time)*scale;
        } else
            w = LOFont::getTimeSize(tt,false)*scale+0.5;
        
        SunnyMatrix m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(mapSize.x+horizontalMapAdd-w,mapSize.y-1.5+verticalMapAdd,-2));
        if (tt<60)
            LOFont::writeTime(&m, false, time, color);
        else
            LOFont::writeTime(&m, false, tt, color, color,false);
#endif
//        
//        if (isSurvival && !gamePauseButton->hidden)
//        {
//            m.pos.x = mapSize.x/4;
//            loRenderPriceLabel(&m, LOPT_VioletSnowflakes, LOScore::getTotalSnowflakes(),1 ,LOTA_Center,scale*1.3);
//        }
    }
    
    if (puzzleWindow)
        puzzleWindow->render();
    else
        if (!tutorial || !tutorial->isVisible)
            LOPuzzle::removeData();
    
    if (store)
        store->render();

    if (episodeWName==0 || episodeWNumber!=selectedStepNum)
    {
        if (episodeWName)
            delete [] episodeWName;
        episodeWNumber = selectedStepNum;
        
        char ep[32];
        strcpy(ep, "Episode.");
        int len = (int)strlen(ep);
        ep[len] = '1' + selectedStepNum;
        ep[len+1] = '\0';
        episodeWName = getLocalizedLabel((const char*)ep);
    }
    
    if (scrollView->getOffsetX()<=(mapZoomBounds.y-mapZoomBounds.x)*(scrollView->getPagesCount()-2))
        previewAlpha = 1.0;
    else
        previewAlpha = 1.0 - (scrollView->getOffsetX() - (mapZoomBounds.y-mapZoomBounds.x)*(scrollView->getPagesCount()-2))/(mapZoomBounds.y-mapZoomBounds.x);

    if (isSurvivalSelectionShowed())
    {
        renderLevelSelectionSurvival();
    } else
    if (isLevelSelectionShowed())
    {
        renderLevelSelection();
    }
    
    mainWindow->render();
    levelComplete->render();
    pauseWindow->render();
    deathWindow->render();
    if (isLevelSelectionShowed() || isSurvivalSelectionShowed())
        tasksView->render(menuFirstS,menuLastS,scrollView->getOffsetX()-scrollView->getFrame().x*menuFirstS);
    else
        tasksView->render(0,0,0);
    rewardView->render();
    messageWindow->render();
    if (tutorial)
        tutorial->render();
    
    if (survComplete)
        survComplete->render();
}

void buttonPressed2(SunnyButton* btn)
{
    sharedMenu->buttonPressed(btn);
}

void levelButtonPressed2(SunnyButton* btn)
{
    sharedMenu->levelButtonPressed(btn);
}

void spellButtonPressed2(SunnyButton* btn)
{
    sharedMenu->spellButtonPressed(btn);
}

void gamePlayButtonPressed2(SunnyButton* btn)
{
    sharedMenu->gamePlayButtonPressed(btn);
}

void controlButtonPressed2(SunnyButton* btn)
{
    sharedMenu->controlButtonPressed(btn);
}

void LOMenu::controlButtonPressed(SunnyButton* btn)
{
    LOSettings::changeControlType();
    loAccelerometer();
}

void LOMenu::pauseTheGame()
{
    if (levelComplete->isVisible || deathWindow->isVisible)
    {
        printf("\n Cancel Pause");
        return;
    }
    showGameSpellButtons(false);
    gamePaused = true;
    showGamePlayButtons(true);
}

void LOMenu::gamePlayButtonPressed(SunnyButton* btn)
{
    if (gamePauseButton == btn)
    {
        pauseTheGame();
    } 
}

bool LOMenu::canCast()
{
    return (!spellButtons[LOSpellTeleport]->hidden && spellButtons[LOSpellTeleport]->enable);
}

void LOMenu::spellButtonPressed(SunnyButton* btn)
{
    if (player->isAbilityActive((LevelAbility)btn->tag))
        return;
    
    if (btn == spellButtons[LOSpellTeleport])
    {
        player->activateAbility(LA_Teleport);
    } else
    if (btn == spellButtons[LOSpellSpeed])
    {
        player->activateAbility(LA_Speed);
    } else
        if (btn == spellButtons[LOSpellLife])
        {
            player->activateAbility(LA_Life);
        }else
            if (btn == spellButtons[LOSpellShield])
            {
                player->activateAbility(LA_Shield);
            }
}

void LOMenu::recalculateSurvivalPrices(SunnyButton * btn)
{
    LOPriceType type;
    unsigned short price;
    unsigned short totalPrice = 0;
    if (survivalButtons[LOSB_SpellMagnet]->active)
    {
        type = LOScore::getPrice(LA_Magnet, price);
        if (type==LOPT_GoldCoins)
            totalPrice += price;
    }
    if (survivalButtons[LOSB_SpellTime]->active)
    {
        type = LOScore::getPrice(LA_Time, price);
        if (type==LOPT_GoldCoins)
            totalPrice += price;
    }
    if (survivalButtons[LOSB_SpellDoubler]->active)
    {
        type = LOScore::getPrice(LA_Doubler, price);
        if (type==LOPT_GoldCoins)
            totalPrice += price;
    }
    
    if (totalPrice>LOScore::getTotalCoins())
    {
        btn->active = false;
        showNotEnoughGoldHint();
    }
}

void LOMenu::buttonPressed(SunnyButton* btn)
{
    if (btn == survivalButtons[LOSB_SpellDoubler])
    {
        if (!LOScore::isSpellEnabled(LA_Doubler))
        {
            btn->active = false;
            showSpellInactive();
        }
        recalculateSurvivalPrices(btn);
    } else
        if (btn == survivalButtons[LOSB_SpellTime])
        {
            if (!LOScore::isSpellEnabled(LA_Time))
            {
                btn->active = false;
                showSpellInactive();
            }
            recalculateSurvivalPrices(btn);
        } else
            if (btn == survivalButtons[LOSB_SpellMagnet])
            {
                if (!LOScore::isSpellEnabled(LA_Magnet))
                {
                    btn->active = false;
                    showSpellInactive();
                }
                recalculateSurvivalPrices(btn);
            } else
        if (btn==survivalButtons[LOSB_Back])
        {
            showMainMenu(true);
            showSurvivalButtons(false);
            tasksView->setVisible(false);
        } else
            if (btn == survivalButtons[LOSB_Play])
            {
                if (selectedStepNum<loStepsCount && LOScore::isStageEnabled(selectedStepNum))
                {
                    showSurvivalButtons(false);
                    loLoadSurvivalMap(selectedStepNum,false,survivalButtons[LOSB_SpellDoubler]->active,survivalButtons[LOSB_SpellMagnet]->active,survivalButtons[LOSB_SpellTime]->active);
                    showGameSpellButtons(true);
                }
            } else
                if (btn == socialButtons[LOSocialButton_FB])
                {
                    loOpenFBFunPage(social_dodge_fb,social_dodge_fb_id);
                } else
                    if (btn == socialButtons[LOSocialButton_TW])
                    {
                        loOpenTWFunPage(social_dodge_tw,social_dodge_tw_id);
                    } else
                        if (btn == socialButtons[LOSocialButton_VK])
                        {
                            loOpenURL(social_unnyhog_vk);
                        }
}

SunnyButton ** LOMenu::getSurvivalButtons()
{
    return survivalButtons;
}

void LOMenu::levelButtonPressed(SunnyButton* btn)
{
    if (levelButtonsAlpha<0.99) return;
    
    if (scrollButtons[0] == btn || scrollButtonsSurv[0] == btn)
    {
        levelButtonsAlpha = 0.98;
        scrollView->moveToTheLeft();
        return;
    }
    
    if (levelsButtons[loLevelsPerStep] == btn)
    {
        showLevelsButtons(false);
        showMainMenu(true);
        return;
    }

    if (selectedStepNum==loStepsCount) return;//last page

    if (scrollButtons[1] == btn || scrollButtonsSurv[1] == btn)
    {
        levelButtonsAlpha = 0.98;
        scrollView->moveToTheRight();
        return;
    }

    for (int i = 0;i<loLevelsPerStep;i++)
        if (levelsButtons[i] == btn)
        {
            if (selectedLevelNum==i)
            {
                showLevelsButtons(false);
                loLoadMap(selectedStepNum*loLevelsPerStep + selectedLevelNum);
                showGameSpellButtons(true);
            } else
            {
                selectedLevelNum = i;
                loLoadPreview(selectedStepNum*loLevelsPerStep + selectedLevelNum);
            }
            break;
        }
}

void LOMenu::showGameOver()
{
    showGamePlayButtons(false);
    showGameSpellButtons(false);
    showGameOverButtons(true);
}

void LOMenu::backButtonPressed()
{
    if (isLevelSelectionShowed())
    {
        levelButtonPressed(levelsButtons[loLevelsPerStep]);
    } else
        if (isSurvivalSelectionShowed())
        {
            buttonPressed(survivalButtons[LOSB_Back]);
        } else
            if (pauseWindow->isVisible)
            {
                pauseWindow->hideWindow();
            } else
                if (!menuMode && !gamePaused)
                {
                    pauseTheGame();
                } else
                    if (store)
                    {
                        store->hideWindow();
                    } else
                        if (mainWindow->isVisible)
                        {
                            loExitTheApp();
                        }
}

void LOMenu::showLevelComplete(float time, short coins,short time2, short time3)
{
    showGamePlayButtons(false);
    showGameSpellButtons(false);
    levelComplete->prepareWindow(time, coins, time2, time3);
    showLevelCompleteButtons(true);
}

void LOMenu::showSettingsWindow()
{
    if (tutorial) delete tutorial;
    
    tutorial = new LOSettingsWindow();
}

void LOMenu::showMagicDropInfo(LOMagicBoxType type)
{
    if (!tutorial)
    {
        tutorial = new LOMagicDropInfo(type);
    }
}

void LOMenu::leftHandedChanged()
{
    float dSpells = 0.05*mapSize.x;
    if (screenSize.y>=750) dSpells *= 0.7;
    
    if (LOSettings::isLeftHanded())
    {
        for (short i = 0;i<LOSpellCount;i++)
            spellButtons[i]->matrix.pos.x = mapZoomBounds.x + dSpells;
    } else
    {
        for (short i = 0;i<LOSpellCount;i++)
            spellButtons[i]->matrix.pos.x = mapZoomBounds.y - dSpells;
    }
}

LOMenu::LOMenu()
{
    survComplete = 0;
    isStarAnimated = false;
    tutorial = 0;
    
    episodeWName = 0;
    episodeWNumber = -1;
    
    puzzleWindow = 0;
    store = 0;
    
    levelButtonsAlpha = 1;
    sharedMenu = this;
    levelCompleteShown = false;
    
    scrollView = new SunnyScrollView();
    scrollView->setFrame(SunnyVector2D(mapZoomBounds.y - mapZoomBounds.x,0));
    scrollView->setPagesCount(loStepsCount+1);
    scrollView->setScrollStoppedCallback(updateLevelSelection2);
    menuOffset = scrollView->getOffsetXPointer();
    
    //Survival
    float btnSize = 340/2;
    survivalButtons[LOSB_Play] = sunnyAddButton(SunnyVector2D(1716./1920*mapSize.x+horizontalMapAdd,(1280-1076)/1280.*mapSize.y), menuTexture, SunnyVector4D(0,110,206,206)/2048.,SB_Normal,SunnyVector2D(btnSize,btnSize)*pixelToScale);
    sunnyButtonSetActiveState(survivalButtons[LOSB_Play], SunnyVector4D(206,122,206,206)/2048.);
    
    survivalButtons[LOSB_Back] = sunnyAddButton(SunnyVector2D(206./1920*mapSize.x-horizontalMapAdd,(1280-1076)/1280.*mapSize.y), menuTexture, SunnyVector4D(0,110,206,206)/2048.,SB_Normal,SunnyVector2D(btnSize,btnSize)*pixelToScale);
    sunnyButtonSetActiveState(survivalButtons[LOSB_Back], SunnyVector4D(0,0.5,0.25,0.25));

    float ipadScale = 1.0;
    float dSpells = 0.05*mapSize.x*ipadScale;
    survivalButtons[LOSB_SpellDoubler] = sunnyAddButton(SunnyVector2D(1430./1920*mapSize.x,(1280-766)/1280.*mapSize.y) , menuTexture, SunnyVector4D(0,0,118,110)/2048,SB_Check,SunnyVector2D(118*pixelToScale,110*pixelToScale));
    survivalButtons[LOSB_SpellMagnet] = sunnyAddButton(SunnyVector2D(1276./1920*mapSize.x,(1280-516)/1280.*mapSize.y) , menuTexture, SunnyVector4D(118,0,106,110)/2048,SB_Check,SunnyVector2D(106*pixelToScale,110*pixelToScale));
    survivalButtons[LOSB_SpellTime] = sunnyAddButton(SunnyVector2D(1594./1920*mapSize.x,(1280-516)/1280.*mapSize.y) , menuTexture, SunnyVector4D(224,0,112,122)/2048,SB_Check,SunnyVector2D(112*pixelToScale,122*pixelToScale));
    sunnyButtonSetScale(survivalButtons[LOSB_SpellDoubler], dSpells, dSpells);
    sunnyButtonSetScale(survivalButtons[LOSB_SpellMagnet], dSpells, dSpells);
    sunnyButtonSetScale(survivalButtons[LOSB_SpellTime], dSpells, dSpells);

    survivalButtons[LOSB_SpellDoubler]->soundTag = survivalButtons[LOSB_SpellTime]->soundTag = survivalButtons[LOSB_SpellMagnet]->soundTag = LOBT_DropSound;
    for (int i = 0;i<LOSB_Count;i++)
    {
        survivalButtons[i]->hidden = true;
        survivalButtons[i]->tag = i+1;
        sunnyButtonSetCallbackFunc(survivalButtons[i], buttonPressed2);
        sunnyButtonSetRenderFunc(survivalButtons[i], renderCallbackLevelButton);
        
    }
    float scale = 2;
    //Survival...

    scale = 1.5;
    levelsButtons[0] = sunnyAddButton(SunnyVector2D(234./1920*mapSize.x,(1280-706)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[1] = sunnyAddButton(SunnyVector2D(488./1920*mapSize.x,(1280-696)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[2] = sunnyAddButton(SunnyVector2D(766./1920*mapSize.x,(1280-696)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[3] = sunnyAddButton(SunnyVector2D(1034./1920*mapSize.x,(1280-698)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[4] = sunnyAddButton(SunnyVector2D(1308./1920*mapSize.x,(1280-698)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[5] = sunnyAddButton(SunnyVector2D(408./1920*mapSize.x,(1280-894)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[6] = sunnyAddButton(SunnyVector2D(660./1920*mapSize.x,(1280-866)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[7] = sunnyAddButton(SunnyVector2D(940./1920*mapSize.x,(1280-854)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[8] = sunnyAddButton(SunnyVector2D(1208./1920*mapSize.x,(1280-846)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[9] = sunnyAddButton(SunnyVector2D(1468./1920*mapSize.x,(1280-830)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[10] = sunnyAddButton(SunnyVector2D(602./1920*mapSize.x,(1280-1086)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[11] = sunnyAddButton(SunnyVector2D(848./1920*mapSize.x,(1280-1044)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[12] = sunnyAddButton(SunnyVector2D(1120./1920*mapSize.x,(1280-1016)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[13] = sunnyAddButton(SunnyVector2D(1388./1920*mapSize.x,(1280-986)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    levelsButtons[14] = sunnyAddButton(SunnyVector2D(1660./1920*mapSize.x,(1280-960)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    
    //back
    levelsButtons[loLevelsPerStep] = sunnyAddButton(SunnyVector2D(130/1920.*mapSize.x-horizontalMapAdd,(1280-1158)/1280.*mapSize.y-verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize, btnSize)*pixelToScale);

    for (int i = 0;i<=loLevelsPerStep;i++)
    {
//        levelsButtons[i] = sunnyAddButton(SunnyVector2D(5+5*(i%5),10 - 3*(i/5)), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(1.5,1.5));
        levelsButtons[i]->hidden = true;
        sunnyButtonSetCallbackFunc(levelsButtons[i], levelButtonPressed2);
        sunnyButtonSetRenderFunc(levelsButtons[i], renderCallbackLevelButton);
        levelsButtons[i]->tag = i+1;
        levelsButtons[i]->matrix.pos.z = -20;
    }
    
    //Social
    socialButtons[LOSocialButton_FB] = sunnyAddButton(SunnyVector2D(582./1920*mapSize.x,(1280-798)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize*0.9, btnSize*0.9)*pixelToScale);
    socialButtons[LOSocialButton_TW] = sunnyAddButton(SunnyVector2D(960./1920*mapSize.x,(1280-798)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize*0.9, btnSize*0.9)*pixelToScale);
    socialButtons[LOSocialButton_VK] = sunnyAddButton(SunnyVector2D(1340./1920*mapSize.x,(1280-798)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(btnSize*0.9, btnSize*0.9)*pixelToScale);
    for (int i = 0;i<LOSocialButton_Count;i++)
    {
        socialButtons[i]->hidden = true;
        sunnyButtonSetCallbackFunc(socialButtons[i], buttonPressed2);
        sunnyButtonSetRenderFunc(socialButtons[i], renderCallbackEmpty);
    }
    //Social...
    
    scale = 2;
    scrollButtons[0] = sunnyAddButton(SunnyVector2D((108)/1920.*mapSize.x,(1280-237)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    scrollButtons[1] = sunnyAddButton(SunnyVector2D((108+900)/1920.*mapSize.x,(1280-237+50)/1280.*mapSize.y), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    scrollButtonsSurv[0] = sunnyAddButton(SunnyVector2D((466)/1920.*mapSize.x,(1280-92)/1280.*mapSize.y + verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));
    scrollButtonsSurv[1] = sunnyAddButton(SunnyVector2D((466+900)/1920.*mapSize.x,(1280-92)/1280.*mapSize.y + verticalMapAdd), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(scale, scale));

    for (short i = 0;i<2;i++)
    {
        scrollButtons[i]->hidden = true;
        scrollButtons[i]->matrix.pos.z = -10;
        sunnyButtonSetCallbackFunc(scrollButtons[i], levelButtonPressed2);
        sunnyButtonSetRenderFunc(scrollButtons[i], renderCallbackEmpty);
        
        scrollButtonsSurv[i]->hidden = true;
        scrollButtonsSurv[i]->matrix.pos.z = -10;
        sunnyButtonSetCallbackFunc(scrollButtonsSurv[i], levelButtonPressed2);
        sunnyButtonSetRenderFunc(scrollButtonsSurv[i], renderCallbackEmpty);
    }
    
    if (screenSize.y>=750) dSpells *= 0.7;
    spellButtons[LOSpellShield]     = sunnyAddButton(SunnyVector2D(mapZoomBounds.y-1*dSpells,mapZoomBounds.z + 7*dSpells));
    spellButtons[LOSpellLife]       = sunnyAddButton(SunnyVector2D(mapZoomBounds.y-1*dSpells,mapZoomBounds.z + 5*dSpells));
    spellButtons[LOSpellSpeed]      = sunnyAddButton(SunnyVector2D(mapZoomBounds.y-1*dSpells,mapZoomBounds.z + 3*dSpells));
    spellButtons[LOSpellTeleport]   = sunnyAddButton(SunnyVector2D(mapZoomBounds.y-1*dSpells,mapZoomBounds.z + 1*dSpells));
    
    spellButtons[LOSpellShield]->soundTag = LOBT_SpellShield;
    spellButtons[LOSpellLife]->soundTag = LOBT_SpellLife;
    spellButtons[LOSpellSpeed]->soundTag = LOBT_SpellSpeed;
    spellButtons[LOSpellTeleport]->soundTag = LOBT_SpellTeleport;
    
    for (int i = 0;i<LOSpellCount;i++)
    {
        sunnyButtonSetScale(spellButtons[i], dSpells, dSpells);//1.5 was before
        spellButtons[i]->hidden = true;
        sunnyButtonSetCallbackFunc(spellButtons[i], spellButtonPressed2);
        spellButtons[i]->tag = i;
        sunnyButtonSetRenderFunc(spellButtons[i], renderCallbackSpellButton2);
    }
    
    //Gameplay
    gamePauseButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapZoomBounds.w-1.5) , menuTexture, SunnyVector4D(706,160,108,108)/2048.,SB_Normal,SunnyVector2D(108*pixelToScale,108*pixelToScale));
    gamePauseButton->hidden = true;
    sunnyButtonSetCallbackFunc(gamePauseButton, gamePlayButtonPressed2);
    
    //Controls
    playerPositionInLevels = getScaleMatrix(SunnyVector3D(3,3,3));
    playerPositionInLevels.pos = levelsButtons[selectedLevelNum]->matrix.pos + SunnyVector4D(0,0,10,0);
    
    labelTimeRecord = getLocalizedLabel("Time_Record");
    
    labelTimePosX = - LOFont::getTextSize(labelTimeRecord);
    
    labelToBeContinued = getLocalizedLabel("To be Continued");
    labelStayTuned = getLocalizedLabel("Stay Tuned");
    
    mainWindow = new LOMainWindow();
    showMainMenu(true);
    levelComplete = new LOLevelComplete();
    pauseWindow = new LOPause();
    deathWindow = new LODeathWindow();
    messageWindow = new LOMessageWindow();
    tasksView = new LOTasksView();
    rewardView = new LORewardView();
}

void LOMenu::resetHintFlags()
{
    for (short i = 0;i<hintFlagsCount;i++)
        hintFlags[i] = false;
    
    hintTimer = -100;
    
    updateSpells();
}

void LOMenu::dontShowTutorial()
{
    //level 10 is an exception
    if (selectedStepNum==0 && selectedLevelNum==9) return;
    hintFlags[0] = true;
}

void LOMenu::showMessage(const char * notLocalizedString)
{
    messageWindow->setText(notLocalizedString);
    messageWindow->setVisible(true);
}

void LOMenu::showSpellInactive()
{
    showMessage("SPELL_NOT_AVAILABLE");
}

void LOMenu::showSurvivalTutorial()
{
    if (tutorial) delete tutorial;
    
    char ep[32];
    strcpy(ep, "Episode.");
    int len = (int)strlen(ep);
    ep[len] = '1' + selectedStepNum;
    ep[len+1] = '\0';

    tutorial = new LOTutorialMessage("SURVIVAL_UNLOCKED","Survival_Description",ep);
}

void LOMenu::showSurvivalHelper()
{
    if (!LOSettings::checkTurotialFlag(TUTORIAL_SURVIVAL))
    {
        messageWindow->setText("Survival_Helper");
        messageWindow->setVisible(true);
        LOSettings::addTutorialFlag(TUTORIAL_SURVIVAL);
    }
}

void LOMenu::showPuzzleTutorial(SunnyVector2D pos, unsigned short number, bool justClose)
{
    if (tutorial) delete tutorial;
    tutorial = new LOTutorialPuzzle(pos,number,justClose);
}

bool LOMenu::showSpellTutorial(LevelAbility spell)
{
    if (tutorial) return false;
    //delete tutorial;
    tutorial = new LOTutorialSpell(spell);
    return true;
}

void LOMenu::showRessurectionHint()
{
    messageWindow->setText("GiftRessurection");
    messageWindow->setVisible(true);
}

void LOMenu::showNotEnoughGoldHint(void (*func)(SunnyButton*),SunnyButton * btn)
{
    messageWindow->setText("NotEnoughGold");
    messageWindow->setVisible(true,LOMessage_NoGold);
    messageWindow->changeButtonText("YES");
    messageWindow->addButton("NO", 0, 0);
    if (func)
        messageWindow->setCallback(func, btn);
}

void LOMenu::show5StarsHint()
{
    messageWindow->setText("Collect5Stars");
    messageWindow->setVisible(true);
}

void LOMenu::showMagicBox(LOMagicBoxType type)
{
    rewardView->initWithMagicBox(type);
    rewardView->setVisible(true);
}

void LOMenu::showHelpMessage()
{
    switch (selectedStepNum) {
        case 0:
        {
            switch (selectedLevelNum) {
                case 0:
                {
                    if (hintFlags[0]) return;
                    hintFlags[0] = true;
                    if (tutorial) delete tutorial;
                    tutorial = new LOTutorialSnowflake();
                    tutorial->setVisible(true);
                    break;
                }
                case 1:
                {
                    if (hintFlags[0]) return;
                    hintFlags[0] = true;
                    if (tutorial) delete tutorial;
                    tutorial = new LOTutorialFireball();
                    tutorial->setVisible(true);
                    break;
                }
                case 5:
                {
                    if (hintFlags[0]) return;
                    hintFlags[0] = true;
                    if (tutorial) delete tutorial;
                    tutorial = new LOTutorialGroundball();
                    tutorial->setVisible(true);
                    break;
                }
                case 6:
                {
                    if (hintFlags[0]) return;
                    hintFlags[0] = true;
                    tutorial = new LOTutorialWaterball();
                    tutorial->setVisible(true);
                    break;
                }
                case 9:
                {
                    if (activeMap->mapInfo.snowflakeCondition!=1)// && !LOScore::checkFlag(SPELL_TELEPORT))
                    {
                        if (hintTimer<-10)
                            hintTimer = 3;
                        else
                        {
                            hintTimer-=deltaTime;
                            if (hintTimer<=0)
                            {
                                if (hintFlags[0]) return;
                                hintFlags[0] = true;
                                if (!LOScore::checkFlag(SPELL_TELEPORT))
                                {
                                    LOScore::addAbilityCharges(LA_Teleport,freeSpellsCount);
                                    LOScore::addFlag(SPELL_TELEPORT);
                                }
                                showSpellTutorial(LA_Teleport);
                            }
                        }
                    }
                    break;
                }
                case 11:
                {
                    if (hintFlags[0]) return;
                    hintFlags[0] = true;
                    if (!LOScore::checkFlag(SPELL_SPEED))
                    {
                        LOScore::addAbilityCharges(LA_Speed,freeSpellsCount);
                        LOScore::addFlag(SPELL_SPEED);
                    }
                    showSpellTutorial(LA_Speed);
                    break;
                }
                    
                default:
                    break;
            }
            break;
        }
        case 1:
        {
            switch (selectedLevelNum) {
                case 6:
                {
                    if (hintFlags[0]) return;
                    hintFlags[0] = true;
                    if (!LOScore::checkFlag(SPELL_LIFE))
                    {
                        LOScore::addAbilityCharges(LA_Life,freeSpellsCount);
                        LOScore::addFlag(SPELL_LIFE);
                    }
                    showSpellTutorial(LA_Life);
                    break;
                }
            }
            break;
        }
        case 2:
        {
            switch (selectedLevelNum) {
                case 3:
                {
                    if (hintFlags[0]) return;
                    hintFlags[0] = true;
                    if (!LOScore::checkFlag(SPELL_SHIELD))
                    {
                        LOScore::addAbilityCharges(LA_Shield,freeSpellsCount);
                        LOScore::addFlag(SPELL_SHIELD);
                    }
                    showSpellTutorial(LA_Shield);
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
}

LOMenu::~LOMenu()
{
    for (int i = 0;i<LOSpellCount;i++)
        sunnyRemoveButton(spellButtons[i]);
    
    delete mainWindow;
    delete levelComplete;
    delete pauseWindow;
    delete deathWindow;
    delete messageWindow;
    if (episodeWName)
        delete []episodeWName;
    delete []labelTimeRecord;
    
    delete []labelToBeContinued;
    delete []labelStayTuned;
}

bool LOMenu::isSurvivalSelectionShowed()
{
    return !survivalButtons[0]->hidden;
}

bool LOMenu::isGamePlay()
{
    return !gamePauseButton->hidden;
}

bool LOMenu::isLevelSelectionShowed()
{
    return !levelsButtons[0]->hidden;
}

void LOMenu::touchBegan(void * touch, float x, float y)
{
    if (puzzleWindow)
        puzzleWindow->touchBegan(touch, x, y);
    if (isLevelSelectionShowed() || isSurvivalSelectionShowed())
    {
        scrollView->touchBegan(touch, x, y);
    }
    
    if (!isAnyWindowOn())
        if (store)
            store->scroll->touchBegan(touch, x, y);
}

void LOMenu::touchMoved(void * touch, float x, float y)
{
    if (puzzleWindow)
        puzzleWindow->touchMoved(touch, x, y);

    if (isLevelSelectionShowed() || isSurvivalSelectionShowed())
    {
        scrollView->touchMoved(touch, x, y);
    }
    
    if (scrollView->getTotalMoveByTouch()>0.5)
        levelButtonsAlpha = 0.49;
    if (!isAnyWindowOn())
        if (store)
            store->scroll->touchMoved(touch, x, y);
}

void LOMenu::touchEnded(void * touch, float x, float y)
{
    if (puzzleWindow)
        puzzleWindow->touchEnded(touch, x, y);

    if (levelButtonsAlpha<0.99)
    {
        if (scrollView->getTotalMoveByTouch() > 0.5)
            levelButtonsAlpha = 0.51;
        else
            levelButtonsAlpha = 1.0;
    }
    if (isLevelSelectionShowed() || isSurvivalSelectionShowed())
        scrollView->touchEnded(touch, x, y);
    if (!isAnyWindowOn())
        if (store)
            store->scroll->touchEnded(touch, x, y);
}

bool LOMenu::isLevelCompletedShown()
{
    return levelComplete->didHideEverything();
}

void LOMenu::updateSpells()
{
    if (menuMode || gamePaused) return;
    
    spellButtons[LOSpellTeleport]->enable   = LOScore::isSpellEnabled(LA_Teleport);
    spellButtons[LOSpellSpeed]->enable      = LOScore::isSpellEnabled(LA_Speed);
    spellButtons[LOSpellLife]->enable       = LOScore::isSpellEnabled(LA_Life);
    spellButtons[LOSpellShield]->enable     = LOScore::isSpellEnabled(LA_Shield);
    
    priceType[LOSpellTeleport]  = LOScore::getPrice(LA_Teleport,    price[LOSpellTeleport]);
    priceType[LOSpellSpeed]     = LOScore::getPrice(LA_Speed,       price[LOSpellSpeed]);
    priceType[LOSpellLife]      = LOScore::getPrice(LA_Life,        price[LOSpellLife]);
    priceType[LOSpellShield]    = LOScore::getPrice(LA_Shield,      price[LOSpellShield]);
    
    //update price
    
    bool anySpellEnabled = false;
    for (short i = 0; i<LOSpellCount; i++)
        if (spellButtons[i]->enable)
        {
            anySpellEnabled = true;
            break;
        }
    
    for (short i = 0; i<LOSpellCount; i++)
        spellButtons[i]->hidden = !anySpellEnabled;
}

void LOMenu::updateLevelSelection()
{
    selectedStepNum = scrollView->getCurrentPage();
    bool social = (selectedStepNum==loStepsCount);//last page
    for (short i = 0;i<LOSocialButton_Count;i++)
        socialButtons[i]->hidden = !social;
    
    if (social) return;
    
    if (isSurvival)
    {
        tasksView->updateInformation();
    } else
    {
        loLoadPreview(selectedStepNum*loLevelsPerStep + selectedLevelNum);
    }
}

void LOMenu::update()
{
    if (isStarAnimated)
        starAnimationTime += deltaTime;
    
    //hardcode spells buttons
    if (!menuMode)
    {
        if (!isSurvival)
            showHelpMessage();
        else
            showSurvivalHelper();
    }
    
    if (puzzleWindow)
        puzzleWindow->update();
    
    if (scrollView)
    {
        scrollView->update(deltaTime);
        if (scrollView->isStopped())
        {
            if (levelButtonsAlpha>0.5 && levelButtonsAlpha<1)
            {
                levelButtonsAlpha += deltaTime;
                if (levelButtonsAlpha>1) levelButtonsAlpha = 1;
            }
        }
        
        if (levelButtonsAlpha>0.99)
        {
            if (selectedStepNum>=loStepsCount)
            {
                menuFirstS = -1;
            } else
                menuFirstS = menuLastS = selectedStepNum;
        }
        else
        {
            if (scrollView->isMovingRight())
            {
                menuFirstS = selectedStepNum;
                menuLastS = sunnyMin(selectedStepNum+1,loStepsCount-1);
            } else
            {
                menuFirstS = sunnyMax(selectedStepNum-1, 0);
                menuLastS = selectedStepNum;
            }
        }
        
        if (menuLastS>=loStepsCount)
            menuLastS=loStepsCount-1;
        
        if (scrollView->getTotalMoveByTouch()>0.4)
        {
            survivalButtons[LOSB_SpellDoubler]->active  = survivalButtons[LOSB_SpellDoubler]->lastActiveState;
            survivalButtons[LOSB_SpellTime]->active     = survivalButtons[LOSB_SpellTime]->lastActiveState;
            survivalButtons[LOSB_SpellMagnet]->active   = survivalButtons[LOSB_SpellMagnet]->lastActiveState;
        }
    }
    
    if (store)
        store->update(deltaTime);
    
    if (tutorial)
        tutorial->update(deltaTime);
    
    if (survComplete)
        survComplete->update(deltaTime);
    
    if (isLevelSelectionShowed())
    {
        SunnyVector2D direction = playerTargetPosition - SunnyVector2D(playerPositionInLevels.pos.x, playerPositionInLevels.pos.y);
        float len = direction.length();
        if (len>0.01 && LOScore::isMapActive(selectedStepNum*loLevelsPerStep))
        {
            if (!LOScore::isMapActive(selectedStepNum*loLevelsPerStep + selectedLevelNum))
            {
                selectedLevelNum = LOScore::getLastActiveMap() - selectedStepNum*loLevelsPerStep;
            }
            float playerVelocity = playerSelectedVelocity*deltaTime;
            if (len>playerVelocity)
                direction *= playerVelocity/len;
            playerPositionInLevels.pos.x += direction.x;
            playerPositionInLevels.pos.y += direction.y;
            
            SunnyVector3D rotVec = SunnyVector3D(direction.x,direction.y, 0) ^ SunnyVector3D(0,0,-1);
            rotVec = -rotVec;
            float sinTetaAngle = sin(rotVec.x);
            float cosTetaAngle = cos(rotVec.x);
            float sinPhiAngle = sin(-rotVec.y);
            float cosPhiAngle = cos(-rotVec.y);
            SunnyVector4D m = playerPositionInLevels.pos;
            playerPositionInLevels.pos = SunnyVector4D(0,0,0,1);
            playerPositionInLevels = playerPositionInLevels * SunnyMatrix(SunnyVector4D(1,0,0,0),SunnyVector4D(0,cosTetaAngle,-sinTetaAngle,0),SunnyVector4D(0,sinTetaAngle,cosTetaAngle,0),SunnyVector4D(0,0,0,1));
            playerPositionInLevels = playerPositionInLevels * SunnyMatrix(SunnyVector4D(cosPhiAngle,0,-sinPhiAngle,0),SunnyVector4D(0,1,0,0),SunnyVector4D(sinPhiAngle,0,cosPhiAngle,0),SunnyVector4D(0,0,0,1));
            playerPositionInLevels.pos = m;
            
            SunnyVector2D p = mapSize/2;
            p.x += selectedStepNum*mapSize.x;
            p = p - (p - SunnyVector2D(playerPositionInLevels.pos.x,playerPositionInLevels.pos.y))/(1.5*mapScale);
            LOTrace::sharedTrace()->addRandomTrace(p,1.5,2);
        } else
        {
            playOnBallAlpha+=deltaTime*3;
            if (playOnBallAlpha>0.8) playOnBallAlpha = 0.8;
        }
    }
    
    if (isGamePlay())
    {
        if (gamePauseButton->active)
        {
            gamePauseButton->alpha = 1;
            gamePauseAlphaTime = 1;
        }
        else
        {
            if (gamePauseAlphaTime>0)
                gamePauseAlphaTime-=deltaTime;
            else
                gamePauseButton->alpha = fmaxf(gamePauseButton->alpha-deltaTime, minimumGamePlayAlpha);
        }
    }
    
    mainWindow->update();
    levelComplete->update();
    deathWindow->update();
    pauseWindow->update();
    rewardView->update(deltaTime);
}

void LOMenu::showGamePlayButtons(bool visible)
{
    pauseWindow->setVisible(visible);
}

void LOMenu::showStoreMenu(bool visible, LOStoreBack back)
{
    if (visible)
    {
        //sunnyTemproraryDisableAllButtons();
        if (!store)
        {
            store = new LOStore;
            store->storeBack = back;
        }
    } else
    {
        //sunnyReenableAllButtons();
        if (store)
        {
            delete store;
            store = 0;
        }
    }
}

void LOMenu::showPuzzleMenu(bool visible)
{
    if (visible)
    {
        puzzleMenuMode = menuMode;
        showGameSpellButtons(false);
        menuMode = true;
        puzzleWindow = new LOPuzzle;
        puzzleWindow->applyPuzzleData(LOScore::getPuzzleData());
    } else
    {
        delete puzzleWindow;
        puzzleWindow = 0;
        if (puzzleMenuMode)
            showMainMenu(true);
        else
        {
            menuMode = puzzleMenuMode;
            pauseTheGame();
        }
    }
}

bool LOMenu::isSurvivalButtonsVisible()
{
    return !survivalButtons[0]->hidden;
}

void LOMenu::showSurvivalButtons(bool visible, bool updatePrices)
{
    for (int i = 0;i<LOSB_Count;i++)
        survivalButtons[i]->hidden = !visible;
    
    for (short i = 0;i<2;i++)
        scrollButtonsSurv[i]->hidden = !visible;
    
    tasksView->setVisible(visible);
    
    if (visible)
    {
        LOPriceType type;
        unsigned short price;
        type = LOScore::getPrice(LA_Doubler, price);survivalButtons[LOSB_SpellDoubler]->active = (type==LOPT_Free) && LOScore::isSpellEnabled(LA_Doubler);
        type = LOScore::getPrice(LA_Magnet, price);survivalButtons[LOSB_SpellMagnet]->active = (type==LOPT_Free) && LOScore::isSpellEnabled(LA_Magnet);
        type = LOScore::getPrice(LA_Time, price);survivalButtons[LOSB_SpellTime]->active = (type==LOPT_Free) && LOScore::isSpellEnabled(LA_Time);
    }
}

void LOMenu::showGameSpellButtons(bool visible)
{
    isStarAnimated = false;
    if (!visible)
    {
        for (int i = 0;i<LOSpellCount;i++)
            spellButtons[i]->hidden = !visible;
    } else
        updateSpells();
    gamePauseButton->hidden = !visible;
    
    gamePauseAlphaTime = 1;
}

void LOMenu::showGameOverButtons(bool visible)
{
    deathWindow->setVisible(visible);
}

void LOMenu::showLevelCompleteButtons(bool visible)
{
    levelCompleteShown = visible;
    levelComplete->setVisible(visible);
}

void LOMenu::showMainMenu(bool visible)
{
    mainWindow->setVisible(visible);
}

void LOMenu::showSurvivalCompleteWindow()
{
    if (survComplete==0)
    {
        survComplete = new LOSurvivalComplete;
        survComplete->initWithResults(playingTime,activeMap->survivalSnowflakesCollected,activeMap->survivalGoldCoinsCollected);
        LOSendFlurryMessageInt("SurvivalPlayed","SurvivalNumber",selectedStepNum+1,"Time",(int)playingTime,NULL);
    }
}

void LOMenu::closeSurvivalCompleteAndUpdateTasks()
{
    if (survComplete)
    {
        tasksView->updateTasksProgress();
        delete survComplete;
        survComplete = 0;
    }
}

bool LOMenu::isRewardViewVisible()
{
    return rewardView->isVisible;
}

void LOMenu::showLevelsButtons(bool visible)
{
    for (int j = 0;j<=loLevelsPerStep;j++)
        levelsButtons[j]->hidden = !visible;
    
    for (short i = 0;i<2;i++)
        scrollButtons[i]->hidden = !visible;
    
    if (!visible)
    for (short i = 0;i<LOSocialButton_Count;i++)
        socialButtons[i]->hidden = !visible;
    
    drawPreview = visible;
    
    if (visible)
    {
        bool q = false;
        for (short i = 0;i<loStepsCount;i++)
        {
            if (q)
                break;
            for (short j = 0;j<loLevelsPerStep;j++)
            if (LOScore::getLevelCoins(i, j)==0)
            {
                selectedLevelNum = j;
                selectedStepNum = i;
                scrollView->setCurrentPage(selectedStepNum);
                q = true;
                break;
            }
        }
        loLoadPreview(selectedStepNum*loLevelsPerStep + selectedLevelNum);
        menuShowedTime = getCurrentTime();
        nextCoinAnimationTime = 1 + sRandomf()*2;
        nextAnimatedCoin = -1;
    }
}

short firstTag;

void checkSound(SunnyButton* btn)
{
    playTestSound(btn->tag-firstTag);
}

void LOMenu::makeSoundCheckButtons()
{
    SunnyButton *btn;
    for (int i = 0;i<40;i++)
    {
        btn = sunnyAddButton(SunnyVector2D(4.5+3*(i%8),2+4*(i/8)) , 0, SunnyVector4D(0,0,0.25,0.25));
        sunnyButtonSetCallbackFunc(btn, checkSound);
        sunnyButtonSetActiveState(btn, SunnyVector4D(0,0.25,0.25,0.25));
        if (i==0)
            firstTag = btn->tag;
    }
}

LOLevelComplete * LOMenu::getLevelCompleteWindow()
{
    return levelComplete;
}

LODeathWindow * LOMenu::getDeathWindow()
{
    return deathWindow;
}
