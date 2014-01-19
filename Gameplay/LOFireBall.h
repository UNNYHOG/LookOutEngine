
//
//  LOFireBall.h
//  LookOut
//
//  Created by Pavel Ignatov on 14.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOFireBall_h
#define LookOut_LOFireBall_h

#include "LOObject.h"

class LOFireBall : public LOObject
{
    short smokeDelay;
    short layerFrame;
    float rotationAngle;
public:
    short anomaled;
    
    LOFireBall();
    
    bool update();
    void renderOverlay(SunnyMatrix *MV);
    void renderLight(SunnyMatrix *MV);
    void render(SunnyMatrix *MV);
    void renderBall(SunnyMatrix *MV);
    void initWithParams(SunnyVector2D pos, SunnyVector2D vel);
    
    static void renderForMenu(SunnyMatrix *m, short frameNum);
    static void renderOverlayForMenu(SunnyMatrix *m);
    
    void applyAnomaly();
};

#endif
