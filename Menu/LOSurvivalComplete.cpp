//
//  LOSurvivalComplete.cpp
//  LookOut
//
//  Created by Ignatov on 12.11.13.
//
//

#include "LOSurvivalComplete.h"
#include "LOGlobalVar.h"
#include "SunnyShaders.h"
#include "SHTextureBase.h"

const float survC_waitTime = 0.3;
const float survC_appearTime = 0.5;

float survivalTimeStatic;

void shareButtonPressed(SunnyButton *btn)
{
    char ep[32];
    strcpy(ep, "Episode.");
    int len = (int)strlen(ep);
    ep[len] = '1' + selectedStepNum;
    ep[len+1] = '\0';
    char * episodeName = getLocalizedLabel((const char*)ep);
    char * messageName = getLocalizedLabel("FACEBOOK_SURV_NAME");
    char * messageCaption = getLocalizedLabel("FACEBOOK_LC_CAPTION");
    char * messageDesc = getLocalizedLabel("FACEBOOK_SURV_DESC");
    
    loShareFacebookSurvival(episodeName, survivalTimeStatic, messageName, messageCaption, messageDesc);
    
    delete []episodeName;
    delete []messageName;
    delete []messageCaption;
    delete []messageDesc;
}

void survCButtonOkPressed(SunnyButton * btn)
{
    LOMenu::getSharedMenu()->closeSurvivalCompleteAndUpdateTasks();
}

LOSurvivalComplete::LOSurvivalComplete()
{
    setName("YourScores",0.6);
}

LOSurvivalComplete::~LOSurvivalComplete()
{
    sunnyRemoveButton(shareFBButton);
}

const float distanceFromCenterToIcon = 2;

void LOSurvivalComplete::render()
{
    LOTutorial::render();
    
    float numbersAlpha[3] = {0,0,0};
    float time = tutorialTime;
    if (time>survC_waitTime)
    {
        time-=survC_waitTime;
        for (short i = 0;i<3;i++)
        {
            if (time>survC_appearTime)
            {
                time-=survC_appearTime;
                numbersAlpha[i] = 1;
            } else
            {
                numbersAlpha[i] = time/(survC_appearTime);
                break;
            }
        }
    }
    
    float scale = 0.7;
    const float moveDown = 0.5;
    const float verticalDistance = 2-moveDown;
    
    const float mapKoef = mapSize.x/1920;
    //Time
    SunnyMatrix m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(mapSize.x/2-distanceFromCenterToIcon,mapSize.y/2 + verticalDistance,-1));
    bindGameGlobal();
    
    sunnyUseShader(globalShaders[LOS_TexturedTSA]);
    SHTextureBase::bindTexture(menuTexture);
    glUniform1f(uniformTSA_A, appearAlpha);
    glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
    glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
    SunnyDrawArrays(LOLCClock_VAO);

    m = m * getTranslateMatrix(SunnyVector3D(distanceFromCenterToIcon,0,0));
    float t = survivalTimeStatic * numbersAlpha[0];
    LOFont::writeTime(&m, true, t,yellowInnerColor(appearAlpha),yellowOuterColor(appearAlpha),false);
    
    //Snowflakes
    if (numbersAlpha[1]>0)
    {
        m = getScaleMatrix(scale) * getTranslateMatrix(SunnyVector3D(mapSize.x/2-distanceFromCenterToIcon,mapSize.y/2-moveDown,-1));
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        glUniform1f(uniformTSA_A, 1);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LOVioletSnowflake_VAO);
        
        int count = survivalSnowflakes * numbersAlpha[1];
        m = m * getTranslateMatrix(SunnyVector3D(distanceFromCenterToIcon,0,0));
        LOFont::writeNumber(&m, true, count,yellowInnerColor(appearAlpha),yellowOuterColor(appearAlpha));
        
        numbersAlpha[2] = fminf(1, numbersAlpha[2]*2);
        if (numbersAlpha[2]>0)
        {
            shareFBButton->active?scale=defaultButtonScale:scale=1;
            m = getScaleMatrix(numbersAlpha[2]*scale);
            m.pos = shareFBButton->matrix.pos;
            sunnyUseShader(globalShaders[LOS_TexturedTSA]);
            
            glUniform1f(uniformTSA_A, numbersAlpha[2]);
            glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
            SunnyDrawArrays(LOShareFB_VAO);
            glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(-164*mapKoef*scale, -12*mapKoef*scale, 0.65*scale, 0.65*scale));
            SunnyDrawArrays(LOSocialButtonFB_VAO);
            
            m =getScaleMatrix(0.4*scale*globalFontScale) *  m * getTranslateMatrix(SunnyVector3D(1.3,0,0));
            LOFont::writeText(&m,LOTA_Center, true, loLabelShare,whiteInnerColor(numbersAlpha[2]),whiteOuterColor(numbersAlpha[2]));
        }
    }
    
    //New record
    recordAlpha = numbersAlpha[1];
    if (survivalTimeStatic > activeMap->previousSurvivalRecord && recordAlpha>0)
    {
        bindGameGlobal();
        
        float k = 0.7;
        float s = 1.3;
        if (numbersAlpha[1]<k)
            scale = s * (numbersAlpha[1]/(k));
        else
            if (numbersAlpha[1]<1.001)
            {
                scale = s - (s-1)*(numbersAlpha[1]-k)/(1-k);
            } else
                scale = 1;
        
        m = getTranslateMatrix(SunnyVector3D(mapSize.x/2-distanceFromCenterToIcon-3,mapSize.y/2 + 1,-1));
        
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SHTextureBase::bindTexture(menuTexture);
        glUniform1f(uniformTSA_A, scale);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, scale, scale));
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        SunnyDrawArrays(LONewRecordImage_VAO);
        
        m.pos.y -= 2;
        m = getScaleMatrix(0.22) * m;
        LOFont::writeTextInRect(&m, SunnyVector2D(1,0), LOTA_Center, true, loLabelNewRecord,whiteOldInnerColor(recordAlpha),yellowOuterColor(recordAlpha));
    }
}

bool LOSurvivalComplete::renderOverlay()
{
    if (survivalTimeStatic > activeMap->previousSurvivalRecord && recordAlpha>0)
    {
        SunnyMatrix m = getTranslateMatrix(SunnyVector3D(mapSize.x/2-distanceFromCenterToIcon-3,mapSize.y/2 + 1,-1));
        float scale;
        scale = recordAlpha + 0.1*sinf(tutorialTime);
        
        bindGameGlobal();
        glUniformMatrix4fv(globalModelview[LOS_OverlayTSA], 1, GL_FALSE, &(m.front.x));
        glUniform4fv(uniformOverlayTSA_C, 1, SunnyVector4D(255./255,210./255,0,scale*0.5));
        glUniform4fv(uniformOverlayTSA_TR, 1, SunnyVector4D(0,0,1.5,1.5));
        
        SunnyDrawArrays(LOOverlaySpells_VAO);
    }
    
    return true;
}

void LOSurvivalComplete::update(float deltaTime)
{
    LOTutorial::update(deltaTime);
}

void LOSurvivalComplete::initWithResults(float time, unsigned long snowflakes, unsigned long coins)
{
    survivalTimeStatic = time;
    survivalSnowflakes = snowflakes;
    survivalGoldCoins = coins;
    
    setVisible(true);
    addStandartOkButtonToClose(survCButtonOkPressed);
    
    shareFBButton = sunnyAddButton(SunnyVector2D(mapSize.x/2,windowCenter.y-frameSize.y/2 + backBorderSize/4 + 3.5 - 20./2048*mapSize.x), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,okButtonSize*pixelToScale);
    sunnyButtonSetCallbackFunc(shareFBButton, shareButtonPressed);
    sunnyButtonSetRenderFunc(shareFBButton, emptyRenderFunction);
    shareFBButton->matrix.pos.z = -2;
}


