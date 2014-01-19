//
//  LOWaterBall.h
//  LookOut
//
//  Created by Pavel Ignatov on 14.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOWaterBall_h
#define LookOut_LOWaterBall_h

#include "LOObject.h"


class LOWaterBall:public LOObject
{
public:
    short anomaled;
    
    LOWaterBall();

    static void renderForMenu(SunnyMatrix *m);
    static void render(SunnyMatrix *MV, short waterBallsCount);
    void renderOverlay(SunnyMatrix *MV);
    void setPositionForNum(short num);
    void renderShadow();
    void render(SunnyMatrix *MV);
    bool update();
    void initWithParams(SunnyVector2D pos, SunnyVector2D vel);
    void renderCharge(SunnyMatrix *MV);
};


#endif
