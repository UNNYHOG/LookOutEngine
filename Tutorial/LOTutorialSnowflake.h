//
//  LOTutorialSnowflake.h
//  LookOut
//
//  Created by Ignatov on 01.11.13.
//
//

#ifndef __LookOut__LOTutorialSnowflake__
#define __LookOut__LOTutorialSnowflake__

#include <iostream>
#include "LOTutorial.h"

class LOTutorialSnowflake : public LOTutorial
{
    SunnyVector2D currentSnowBallPosition;
    short showflakeState[3];//0-normal,1,2-dissapear,3-nothing
public:
    LOTutorialSnowflake();
    void render();
    bool renderOverlay();
    void setVisible(bool visible);
    void reset();
    void update(float deltaTime);
};

#endif /* defined(__LookOut__LOTutorialSnowflake__) */
