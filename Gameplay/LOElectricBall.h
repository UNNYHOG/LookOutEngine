//
//  LOElectricBall.h
//  LookOut
//
//  Created by Pavel Ignatov on 04.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOElectricBall_h
#define LookOut_LOElectricBall_h
#include "LOObject.h"

const short maxLOEAttackCount = 3;

class LOElectricBall : public LOObject
{
    SC_Circle * attack[maxLOEAttackCount];
public:
    short attackCount;
    LOElectricBall();
    bool update();
    void renderOverlay(SunnyMatrix *MV);
    void render(SunnyMatrix *MV);
    void initWithParams(SunnyVector2D pos, SunnyVector2D vel);
    
    void addTarget(SC_Circle * target);
    void removeTarget(SC_Circle * target);
};

#endif
