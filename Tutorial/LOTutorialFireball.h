//
//  LOTutorialFireball.h
//  LookOut
//
//  Created by Ignatov on 06.11.13.
//
//

#ifndef __LookOut__LOTutorialFireball__
#define __LookOut__LOTutorialFireball__

#include <iostream>
#include "LOTutorial.h"
#include "LOExplosion.h"

class LOTutorialFireball : public LOTutorial
{
    SunnyVector2D currentSnowBallPosition;
    SunnyVector2D currentFireBallPosition;
    unsigned short frameNum;
    
    LOExplosion * explosion;
    void crash();
public:
    LOTutorialFireball();
    ~LOTutorialFireball();
    void render();
    bool renderOverlay();
    void setVisible(bool visible);
    void reset();
    void update(float deltaTime);
};

#endif /* defined(__LookOut__LOTutorialFireball__) */
