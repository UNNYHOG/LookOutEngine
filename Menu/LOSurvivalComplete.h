//
//  LOSurvivalComplete.h
//  LookOut
//
//  Created by Ignatov on 12.11.13.
//
//

#ifndef __LookOut__LOSurvivalComplete__
#define __LookOut__LOSurvivalComplete__

#include <iostream>
#include "LOTutorial.h"

class LOSurvivalComplete :public LOTutorial
{
    unsigned long survivalSnowflakes,survivalGoldCoins;
    SunnyButton * shareFBButton;
    float recordAlpha;
public:
    
    LOSurvivalComplete();
    ~LOSurvivalComplete();
    void render();
    bool renderOverlay();
    void update(float deltaTime);
    void initWithResults(float time, unsigned long snowflakes, unsigned long coins);
};

#endif /* defined(__LookOut__LOSurvivalComplete__) */
