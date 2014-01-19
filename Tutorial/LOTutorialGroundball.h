//
//  LOTutorialGroundball.h
//  LookOut
//
//  Created by Pavel Ignatov on 07.11.13.
//
//

#ifndef __LookOut__LOTutorialGroundball__
#define __LookOut__LOTutorialGroundball__

#include <iostream>
#include "LOTutorial.h"
#include "LOExplosion.h"
#include "LOObject.h"

class LOTutorialGroundball: public LOTutorial
{
    SunnyVector2D currentSnowBallPosition;
    SunnyVector2D currentGroundBallPosition;
    SunnyVector2D angles[stonesInGroundModelCount];
    SunnyVector2D anglesV[stonesInGroundModelCount];

    unsigned short frameNum;
    
    LOExplosion * explosion;
    void crash();
    
    float centerAngle;
public:
    LOTutorialGroundball();
    ~LOTutorialGroundball();
    void render();
    bool renderOverlay();
    void setVisible(bool visible);
    void reset();
    void update(float deltaTime);

};

#endif /* defined(__LookOut__LOTutorialGroundball__) */
