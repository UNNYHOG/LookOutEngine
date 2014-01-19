//
//  LOTutorialTeleport.h
//  LookOut
//
//  Created by Ignatov on 11.11.13.
//
//

#ifndef __LookOut__LOTutorialTeleport__
#define __LookOut__LOTutorialTeleport__

#include <iostream>

#include "LOTutorial.h"
#include "LOExplosion.h"

class LOTutorialTeleport: public LOTutorial
{
    SunnyVector2D currentSnowBallPosition;
    bool isSpellActivated;
    bool isSpellCasted;
    
    LOExplosion * explosion;
    void crash();
public:
    LOTutorialTeleport();
    ~LOTutorialTeleport();
    void render();
    bool renderOverlay();
    void setVisible(bool visible);
    void reset();
    void update(float deltaTime);
    
};

#endif /* defined(__LookOut__LOTutorialTeleport__) */
