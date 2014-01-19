//
//  LOTutorialGroundball.h
//  LookOut
//
//  Created by Pavel Ignatov on 07.11.13.
//
//

#ifndef __LookOut__LOTutorialWaterball__
#define __LookOut__LOTutorialWaterball__

#include <iostream>
#include "LOTutorial.h"
#include "LOExplosion.h"
#include "LOObject.h"

class LOTutorialWaterball: public LOTutorial
{
    SunnyVector2D currentSnowBallPosition;
    SunnyVector2D currentWaterBallPosition;

    unsigned short frameNum;
    
    LOExplosion * explosion;
    void crash();
public:
    LOTutorialWaterball();
    ~LOTutorialWaterball();
    void render();
    bool renderOverlay();
    void setVisible(bool visible);
    void reset();
    void update(float deltaTime);

};

#endif /* defined(__LookOut__LOTutorialGroundball__) */
