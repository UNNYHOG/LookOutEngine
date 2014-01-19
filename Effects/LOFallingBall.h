//
//  LOFallingBall.h
//  LookOut
//
//  Created by Pavel Ignatov on 15.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOFallingBall_h
#define LookOut_LOFallingBall_h

#include "SunnyVector2D.h"
#include "SunnyMatrix.h"

class LOFallingBall
{
public:
    static void addFallingBallFromFireball(SunnyMatrix *pos, SunnyVector2D vel);
    static void addFallingBallFromGroundball(SunnyMatrix *pos, SunnyVector2D vel);
    static void render(SunnyMatrix *MV);
    static void renderShadows(SunnyMatrix *MV);
    static void update();
    static void reset();
};

#endif
