//
//  LOTutorialTeleport.cpp
//  LookOut
//
//  Created by Ignatov on 11.11.13.
//
//

#include "LOTutorialTeleport.h"
#include "LOScore.h"
#include "LOSettings.h"
#include "LOMenu.h"
#include "iPhone_Strings.h"

const SunnyVector3D tut5_spellPosition = SunnyVector3D(mapSize.x*0.8,mapSizeY*0.4,-1);
const SunnyVector2D tut5_snowballPosition = SunnyVector2D(mapSize.x*0.6,mapSizeY*0.5);
const SunnyVector2D tut5_snowballTeleportation = SunnyVector2D(mapSize.x*0.3,mapSizeY*0.5);


const float tut5_waitingTime = 0.3;
const float tut5_teleportationHold = 0.3;
const float tut5_waiting2Time = 1;
const float tut5_waiting3Time = 0.5;

void LOTutorialTeleport::setVisible(bool visible)
{
    LOTutorial::setVisible(visible);
    
    reset();
    
    if (LOSettings::checkTurotialFlag(TUTORIAL_TELEPORTATION))
        addStandartOkButtonToClose();
    else
        LOSettings::addTutorialFlag(TUTORIAL_TELEPORTATION);
}

LOTutorialTeleport::LOTutorialTeleport()
{
    setName("Teleportation");
    
    explosion = 0;
}

LOTutorialTeleport::~LOTutorialTeleport()
{
    if (explosion)
        delete explosion;
}

void LOTutorialTeleport::reset()
{
    LOTutorial::reset();
    
    if (explosion)
        delete explosion;
    explosion = 0;
    
    isSpellActivated = isSpellCasted = false;
}

void LOTutorialTeleport::update(float deltaTime)
{
    LOTutorial::update(deltaTime);
    if (explosion) explosion->update(deltaTime);
}

void LOTutorialTeleport::render()
{
    LOTutorial::render();
    
    SunnyMatrix spellM = getTranslateMatrix(tut5_spellPosition);
    float scale = 2;
    float scale2 = scale*(0.75-0.25*sin(getCurrentTime()*M_PI));
    
    
    float time = tutorialTime;
    if (time<tut5_waitingTime)
    {
        currentSnowBallPosition = tut5_snowballPosition;
    } else
    {
        
    }
    
    LOMenu::getSharedMenu()->renderSpellButton(LA_Teleport, &spellM, scale, scale2, 0, false, 1, true);
    
    SunnyMatrix m = getScaleMatrix(cellSize*tutorialScale) * getTranslateMatrix(SunnyVector3D(currentSnowBallPosition.x,currentSnowBallPosition.y,-5));
    glEnable(GL_DEPTH_TEST);
    LOPlayer::renderForMenu(&m);
}

bool LOTutorialTeleport::renderOverlay()
{
    return true;
}


