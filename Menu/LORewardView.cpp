//
//  LORewardView.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 27.10.13.
//
//

#include "LORewardView.h"
#include "SUnnyMatrix.h"
#include "LOScore.h"
#include "LOGlobalVar.h"
#include "iPhone_Strings.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"

LORewardView * sharedReward = 0;

const float scaleAllReward = 4./3.;

const float oneDropAppearTime = 0.4;
const float delaydropAppearTime = 1;

void rewardButtonPressed(SunnyButton * btn)
{
    if (sharedReward)
        sharedReward->buttonPressed(btn);
}

LORewardView::LORewardView()
{
    labelTap = getLocalizedLabel("Tap to break");
    labelYouFound = getLocalizedLabel("YouFound");
    labelTake = getLocalizedLabel("Take");
    tapButton = 0;
    takeButton = 0;
    
    sharedReward = this;
    
    crashModels = 0;
    modelsVelocity = 0;
    drop = 0;
    namePosition.y = mapSize.y*0.75;
}

LORewardView::~LORewardView()
{
    if (labelTap)
        delete [] labelTap;
    if (labelYouFound)
        delete [] labelYouFound;
    if (labelTake)
        delete [] labelTake;
    
    if (crashModels)
        delete []crashModels;
    if (modelsVelocity)
        delete [] modelsVelocity;
}

bool LORewardView::renderOverlay()
{
    if (!isVisible) return false;
    
    SunnyMatrix m = sunnyIdentityMatrix;
    if (overlayAlpha>0)
    {
        SunnyVector4D color;
        float scale = 2*scaleAllReward;
        float alpha = 255*(0.7+0.3*sinf(tutorialTime));
        
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,scale,scale));
        m = getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y/2,-1));
        if (magicBoxType==0) color = SunnyVector4D(255,239,53,alpha)/255;
        else if (magicBoxType==1) color = SunnyVector4D(57,232,255,alpha)/255;
        else color = SunnyVector4D(128,255,91,alpha)/255;
        color.w *= overlayAlpha;
        glUniform4fv(uniformOverlayTSA_C, 1, color);
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
    
    if (drop && iceState == IceState_Destroyed)
    {
        const float textHeight = mapSize.y/2/5;
        const float horizontalWidth = mapSize.x/4;
        short linesCount = (drop->dropCount+1)/2;
        //VS first in the center
        
        const float distToDrop = 1.5;
        float scale = 0.5;
        
        short isLeft = -1;
        float origin = mapSize.y/2 + (linesCount-1)*0.5*textHeight;
        const float originX = mapSize.x/2 + 1;
        float y = origin;
        
        float t = timer - delaydropAppearTime;
        if (t>0)
        {
            short activatedDrops = t/oneDropAppearTime;
            
            const float maxSize = 2;
            const float maxAngle = M_PI - asinf(1/maxSize);
            
            float alpha = maxSize*sinf(maxAngle* (t - activatedDrops*oneDropAppearTime)/oneDropAppearTime);
            
            for (short i = 0;i<drop->dropCount;i++)
            {
                float a = 1;
                if (i==activatedDrops)
                {
                    a = alpha;
                } else
                    if (i>activatedDrops)
                        break;
                if (i==0 && drop->dropCount%2==1)
                {
                    m = getScaleMatrix(scale*a) * getTranslateMatrix(SunnyVector3D(originX-distToDrop,y,-1));
                    y -= textHeight;
                } else
                {
                    m = getScaleMatrix(scale*a) * getTranslateMatrix(SunnyVector3D(originX + isLeft*horizontalWidth/2-distToDrop,y,-1));
                    isLeft = -isLeft;
                    if (isLeft<0)
                        y -= textHeight;
                }
                drawDropOverlay(&m, &(drop->drop[i]),fminf(1, a));
            }
        }
    }

    return true;
}

void LORewardView::render()
{
    if (!isVisible) return;
    glDisable(GL_DEPTH_TEST);
    LOTutorial::render();
    
    SunnyMatrix m;
    float alpha = 1;
    if (crashModels) alpha = (1 - timer*2);
    if (alpha>0)
    {
        m = getScaleMatrix(0.6) * getTranslateMatrix(SunnyVector3D(namePosition.x,namePosition.y,-1));
        LOFont::writeText(&m, LOTA_Center, true, labelTap, SunnyVector4D(255,172,42,255*alpha)/255, SunnyVector4D(241,241,241,255*alpha)/255);
    }
    
    glDisable(GL_DEPTH_TEST);
    //Buttons
    bindGameGlobal();
    SHTextureBase::bindTexture(menuTexture);
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    glUniform1f(uniformTSA_A, overlayAlpha);
    m = getScaleMatrix(scaleAllReward) * getTranslateMatrix(SunnyVector3D(tapButton->matrix.pos.x,tapButton->matrix.pos.y,-7));
    
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOMagicBoxMagic_VAO+magicBoxType);
    
    if (takeButton && !takeButton->hidden)
    {
        float scale = 0.75*takeButton->alpha;
        if (takeButton->active) scale*=0.85;
        m = getScaleMatrix(scale);
        m.pos = takeButton->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        
        glUniform1f(uniformTSA_A, appearAlpha*takeButton->alpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        SunnyDrawArrays(LOOkButtonBack_VAO);
        
        m = getScaleMatrix(0.5/0.75) * m;
        LOFont::writeText(&m, LOTA_Center, true, labelTake, whiteInnerColor(appearAlpha*takeButton->alpha), whiteOuterColor(appearAlpha*takeButton->alpha));
    }
    
    m = getScaleMatrix(6*scaleAllReward)*getRotationXMatrix(M_PI_4/4);
    SunnyMatrix m1;
    
    m.pos = SunnyVector4D(tapButton->matrix.pos.x-0.5,tapButton->matrix.pos.y+0.5,-7,1);
    m1 = getRotationYMatrix(-M_PI_4/4)*m;
    if (crashModels)
    {
        LOMagicBox::renderCrashedMagicBox(crashModels,&m1);
    } else
    {
        LOMagicBox::renderMagicBox(iceState, &m1);
    }
    
    glDisable(GL_DEPTH_TEST);
    
    if (drop && iceState == IceState_Destroyed)
    {
        if (!magicSoundPlayed && timer>=0.3)
        {
            magicSoundPlayed = true;
            losPlayMagicBoxOpenedSound(magicBoxType);
        }
            
        float scale = 0.5;
        m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(namePosition.x,namePosition.y,-1));
        LOFont::writeText(&m, LOTA_Center, true, labelYouFound,yellowInnerColor(1),yellowOuterColor(1));
        
        bindGameGlobal();
        const float textHeight = mapSize.y/2/5;
        const float horizontalWidth = mapSize.x/4;
        short linesCount = (drop->dropCount+1)/2;
        //VS first in the center
        
        const float distToDrop = 1.5;
        
        SunnyVector4D innerC = SunnyVector4D(250,251,253,255*appearAlpha)/255;
        SunnyVector4D outerC = SunnyVector4D(161,174,188,255*appearAlpha)/255;
        
        float t = timer - delaydropAppearTime;
        short activatedDrops = -1;
        if (t>=0) activatedDrops = t/oneDropAppearTime;
        float alpha = (t - activatedDrops*oneDropAppearTime)/oneDropAppearTime;
        
        if (activatedDrops>=drop->dropCount && takeButton)
        {
            if (takeButton->hidden)
            {
                takeButton->hidden = false;
//                activeMap->dropPuzzlePart(mapSize/2, -1);
            }
            if (activatedDrops==drop->dropCount)
                takeButton->alpha = alpha;
            else
                takeButton->alpha = 1;
        }
        
        //Numbers
        short isLeft = -1;
        float origin = mapSize.y/2 + (linesCount-1)*0.5*textHeight;
        const float originX = mapSize.x/2 + 1;
        float y = origin;
        
        for (short i = 0;i<drop->dropCount;i++)
        {
            float a = 1;
            if (i==activatedDrops)
            {
                a = alpha;
                if (!soundsPlayed[i])
                {
                    soundsPlayed[i] = true;
                    losPlayDropSound();
                }
            } else
                if (i>activatedDrops)
                    break;
            if (i==0 && drop->dropCount%2==1)
            {
                m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(originX,y,-1));
                y -= textHeight;
            } else
            {
                m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(originX + isLeft*horizontalWidth/2,y,-1));
                isLeft = -isLeft;
                if (isLeft<0)
                    y -= textHeight;
            }
            innerC.w = outerC.w = a;
            LOFont::writeNumber(&m, true, drop->drop[i].ammount,innerC,outerC);
        }
        
        //Images
        isLeft = -1;
        y = origin;
        
        for (short i = 0;i<drop->dropCount;i++)
        {
            float a = 1;
            if (i==activatedDrops)
            {
                a = alpha;
            } else
                if (i>activatedDrops)
                    break;
            if (i==0 && drop->dropCount%2==1)
            {
                m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(originX-distToDrop,y,-1));
                y -= textHeight;
            } else
            {
                m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(originX + isLeft*horizontalWidth/2-distToDrop,y,-1));
                isLeft = -isLeft;
                if (isLeft<0)
                    y -= textHeight;
            }
            drawDrop(&m, &(drop->drop[i]),a);
        }

    }
}

void LORewardView::drawDropOverlay(SunnyMatrix * m,LODropInfo *info, float alpha)
{
    switch (info->dropType) {
        case LODrop_VS:
        {
            LOVioletSnowflake::renderOverlay(SunnyVector2D(m->pos.x,m->pos.y), 1.5);
            break;
        }
        case LODrop_Spell:
        {
            LOMenu::renderSpellOverlay((LevelAbility)info->additionalType, m,alpha);
            break;
        }
            
        default:
            break;
    }
}

void LORewardView::drawDrop(SunnyMatrix * m,LODropInfo *info, float alpha)
{
    switch (info->dropType) {
        case LODrop_VS:
        {
            //LOVioletSnowflake::render(SunnyVector2D(m->pos.x,m->pos.y), tutorialTime, 1.5, alpha);
            LOVioletSnowflake::renderForMenu(SunnyVector2D(m->pos.x,m->pos.y),1,alpha);
            break;
        }
        case LODrop_Spell:
        {
            LOMenu::renderSpellButton((LevelAbility)info->additionalType, m, 1, 1, 0,false,alpha);
            break;
        }
        case LODrop_Puzzle:
        {
            SunnyMatrix m1 = getScaleMatrix(1.5)**m;
            LOMenu::renderPuzzleImage(&m1,alpha);
            break;
        }
            
        default:
            break;
    }
}

void LORewardView::update(float deltaTime)
{
    if (!isVisible) return;
    LOTutorial::update(deltaTime);
    
    if (crashModels)
    {
        for (short i = 0;i<LOMagicBox::getDestroyedObjectsCount();i++)
        {
            crashModels[i] = getRotationXMatrix(modelsVelocity[i].rotation.x) * getRotationYMatrix(modelsVelocity[i].rotation.y) * getRotationZMatrix(modelsVelocity[i].rotation.z) * getTranslateMatrix(modelsVelocity[i].position);
            modelsVelocity[i].position += modelsVelocity[i].velocity*deltaTime;
            modelsVelocity[i].velocity.y -= 6*deltaTime;
            modelsVelocity[i].rotation += modelsVelocity[i].rotationVelocity*deltaTime;
        }
        
        timer += deltaTime;
    }
    
    if (iceState == IceState_Destroyed)
    {
        overlayAlpha -= deltaTime;
        if (overlayAlpha<0)
            overlayAlpha = 0;
    }
    
    if (playTapSound)
    {
        playTapSound = false;
        losPlayIceTapSound();
    }
    if (playDestroySound)
    {
        playDestroySound = false;
        losPlayIceBreakSound();
    }
    
    if (needToDropPuzzle)
    {
        if (takeButton && !takeButton->hidden)
        {
            needToDropPuzzle = false;
            activeMap->dropPuzzlePart(mapSize/2, -1, true);
        }
    }
}

void LORewardView::closeWindow()
{
    setVisible(false);
    if (tapButton)
        sunnyRemoveButton(tapButton);
    if (takeButton)
        sunnyRemoveButton(takeButton);
    tapButton = 0;
    takeButton = 0;
    
    if (crashModels)
        delete []crashModels;
    if (modelsVelocity)
        delete [] modelsVelocity;
    
    crashModels = 0;
    modelsVelocity = 0;
}

void LORewardView::buttonPressed(SunnyButton * btn)
{
    if (btn == tapButton)
    {
        if (iceState != IceState_Destroyed)
        {
            iceState = (LOIceState)((int)iceState + 1);
            if (iceState == IceState_Destroyed)
            {
                prepareToCrash();
                playDestroySound = true;
            } else
                playTapSound = true;
        }
    } else
        if (btn == takeButton)
        {
            closeWindow();
        }
}

void LORewardView::prepareToCrash()
{
    short count = LOMagicBox::getDestroyedObjectsCount();
    SunnyMatrix * matrices = LOMagicBox::getDestroyedMatrices();
    crashModels = new SunnyMatrix[count];
    modelsVelocity = new LOCrashBox[count];
    for (short i = 0;i<count;i++)
    {
        crashModels[i] = matrices[i];
        modelsVelocity[i].position = Sunny4Dto3D(matrices[i].pos);
        modelsVelocity[i].velocity.x = (crashModels[i].pos.x + (sRandomf()-0.5))*4;
        modelsVelocity[i].velocity.y = (crashModels[i].pos.y + (sRandomf()-0.5))*4;
        modelsVelocity[i].velocity.z = 0;
        modelsVelocity[i].rotation = SunnyVector3D(0,0,0);
        modelsVelocity[i].rotationVelocity = SunnyVector3D((sRandomf()-0.5),(sRandomf()-0.5),(sRandomf()-0.5))*10;
    }
    
    timer = 0;
    
    drop = LOScore::openMagicBox(magicBoxType);
    
    for (short i = 0;i<drop->dropCount;i++)
        if (drop->drop[i].dropType == LODrop_Puzzle)
        {
            needToDropPuzzle = true;
            break;
        }
}

void LORewardView::initWithMagicBox(LOMagicBoxType type)
{
    if (isVisible)
        return;
    
    overlayAlpha = 1;
    magicBoxType = type;
    iceState = IceState_Solid;
    
    setVisible(true);
    
    tapButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,mapSize.y*0.44), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,SunnyVector2D(4.5, 4.5));
    sunnyButtonSetRenderFunc(tapButton, emptyRenderFunction);
    sunnyButtonSetCallbackFunc(tapButton, rewardButtonPressed);
    tapButton->soundTag = 0;
    
    takeButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,windowCenter.y-frameSize.y/2 + backBorderSize/4), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,okButtonSize*pixelToScale);
    sunnyButtonSetRenderFunc(takeButton, emptyRenderFunction);
    sunnyButtonSetCallbackFunc(takeButton, rewardButtonPressed);
    takeButton->hidden = true;
    takeButton->matrix.pos.z = -2;
    takeButton->soundTag = 0;
    
    for (short i = 0;i<maxDropCount;i++)
        soundsPlayed[i] = false;
    magicSoundPlayed = false;
    
    playTapSound = playDestroySound = false;
    needToDropPuzzle = false;
}

