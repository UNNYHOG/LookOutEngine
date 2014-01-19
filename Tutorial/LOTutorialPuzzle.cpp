//
//  LOTutorialPuzzle.cpp
//  LookOut
//
//  Created by Pavel Ignatov on 17.11.13.
//
//

#include "LOTutorialPuzzle.h"
#include "LOGlobalVar.h"
#include "SHTextureBase.h"
#include "SunnyShaders.h"

void emptyRenderFunction(SunnyButton * btn);
void tutorialButtonPressed(SunnyButton *btn);

LOTutorialPuzzle::LOTutorialPuzzle(SunnyVector2D pos, unsigned short number, bool _justClose)
{
    setName("Congratulations",0.6);
    namePosition.y = mapSize.y*0.75;
    if (_justClose)
        puzzleText = getLocalizedLabel("PuzzleFoundText_Short");
    else
        puzzleText = getLocalizedLabel("PuzzleFoundText");
    puzzleButtonText = getLocalizedLabel("ShowMe");
    laterText = getLocalizedLabel("Ok");
    
    setVisible(true);
    
    puzzleDelayTime = 2.3;
    
    puzzleObject = new LOPuzzleObject;
    puzzleObject->initWithPosition(pos, number);
    
    puzzleButton = 0;
    justClose = _justClose;
    laterButton = 0;
}

LOTutorialPuzzle::~LOTutorialPuzzle()
{
    delete [] puzzleButtonText;
    delete [] puzzleText;
    delete [] laterText;
    
    delete puzzleObject;
    
    sunnyRemoveButton(puzzleButton);
    if (laterButton)
        sunnyRemoveButton(laterButton);
}

void LOTutorialPuzzle::buttonPressed(SunnyButton *btn)
{
    if (btn == puzzleButton)
    {
        if (!justClose)
            LOMenu::getSharedMenu()->showPuzzleMenu(true);
        LOMenu::getSharedMenu()->closeTutorial(true);
    } else
        if (btn == laterButton)
        {
            LOMenu::getSharedMenu()->closeTutorial();
        }
}

bool LOTutorialPuzzle::blockRenderOverlays()
{
    return (puzzleButton!=NULL);
}

void LOTutorialPuzzle::update(float deltaTime)
{
    puzzleDelayTime-= deltaTime;
    if (puzzleDelayTime<=0)
    {
        LOTutorial::update(deltaTime);
        if (!puzzleButton)
        {
            float distBetweenButtons = 500;
            if (justClose)
                distBetweenButtons = 0;
            else
            {
                laterButton = sunnyAddButton(SunnyVector2D(mapSize.x/2 + distBetweenButtons/2/1920*mapSize.x,windowCenter.y-frameSize.y/2 + backBorderSize/4), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,okButtonSize*pixelToScale);
                sunnyButtonSetCallbackFunc(laterButton, tutorialButtonPressed);
                sunnyButtonSetRenderFunc(laterButton, emptyRenderFunction);
                laterButton->matrix.pos.z = -2;
            }
            
            puzzleButton = sunnyAddButton(SunnyVector2D(mapSize.x/2 - distBetweenButtons/2/1920*mapSize.x,windowCenter.y-frameSize.y/2 + backBorderSize/4), objectsTexture, SunnyVector4D(0,0,0.25,0.25),SB_Normal,okButtonSize*pixelToScale);
            sunnyButtonSetCallbackFunc(puzzleButton, tutorialButtonPressed);
            sunnyButtonSetRenderFunc(puzzleButton, emptyRenderFunction);
            puzzleButton->matrix.pos.z = -2;
        }
    }
    
    puzzleObject->update(deltaTime);
}

void LOTutorialPuzzle::render()
{
    LOTutorial::render();
   
    puzzleObject->render();
    
    SunnyMatrix m = getScaleMatrix(0.3) * getTranslateMatrix(SunnyVector3D(mapSize.x/2,mapSize.y*0.35,-1));
    LOFont::writeTextInRect(&m, SunnyVector2D(mapSize.x*0.6,0), LOTA_Center, true, puzzleText, whiteInnerColor(appearAlpha),whiteOuterColor(appearAlpha));
    
    if (puzzleButton)
    {
        bindGameGlobal();
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SHTextureBase::bindTexture(menuTexture);
        float scale = appearAlpha/1.5;
        if (puzzleButton->active) scale*=0.85;
        m = getScaleMatrix(scale);
        m.pos = puzzleButton->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform1f(uniformTSA_A, appearAlpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        SunnyDrawArrays(LOOkButtonBack_VAO);
        
        m = getScaleMatrix(0.65) * m;
        char * text = puzzleButtonText;
        if (justClose)
            text = loLabelOk;
        LOFont::writeText(&m, LOTA_Center, true, text, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    }
    
    if (laterButton)
    {
        bindGameGlobal();
        sunnyUseShader(globalShaders[LOS_TexturedTSA]);
        SHTextureBase::bindTexture(menuTexture);
        float scale = appearAlpha/1.5;
        if (laterButton->active) scale*=0.85;
        m = getScaleMatrix(scale);
        m.pos = laterButton->matrix.pos;
        glUniformMatrix4fv(globalModelview[LOS_TexturedTSA], 1, GL_FALSE, &(m.front.x));
        glUniform1f(uniformTSA_A, appearAlpha);
        glUniform4fv(uniformTSA_TR, 1, SunnyVector4D(0, 0, 1, 1));
        SunnyDrawArrays(LOOkButtonBack_VAO);
        
        m = getScaleMatrix(0.65) * m;
        char * text = laterText;
        LOFont::writeText(&m, LOTA_Center, true, text, SunnyVector4D(250,251,253,255*appearAlpha)/255,SunnyVector4D(161,174,188,255*appearAlpha)/255);
    }
}