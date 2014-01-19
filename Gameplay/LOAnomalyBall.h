
//
//  LOFireBall.h
//  LookOut
//
//  Created by Pavel Ignatov on 14.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOAnomalyBall_h
#define LookOut_LOAnomalyBall_h

#include "LOObject.h"

class LOAnomalyBall : public LOObject
{
    short smokeDelay;
//    short layerFrame;
    float rotationAngle;
public:
    bool update();
    void render(SunnyMatrix *MV);
    void renderElectricity();
    void initWithParams(SunnyVector2D pos, SunnyVector2D vel);
};

#endif
