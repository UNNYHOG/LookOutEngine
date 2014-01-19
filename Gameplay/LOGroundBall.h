//
//  LOGroundBall.h
//  LookOut
//
//  Created by Pavel Ignatov on 15.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOGroundBall_h
#define LookOut_LOGroundBall_h

#include "LOObject.h"
//#include "LOGlobalVar.h"

const short smallPartsCount = 10;

struct LOOneGroundPart
{
    SunnyVector4D pos;//xy,size,lifetime
    SunnyVector2D vel;
};

class LOGroundBall:public LOObject
{
    SunnyVector2D angles[stonesInGroundModelCount];
    SunnyVector2D anglesV[stonesInGroundModelCount];
    
    LOOneGroundPart groundParts[smallPartsCount];
    
    float centerAngle;
    short smokeDelay;
public:
    void render(SunnyMatrix *MV);
    static void renderForMenu(SunnyMatrix *m,float centerAngle, SunnyVector2D*angles);
    void renderShadow(SunnyMatrix *MV);
    void renderParts();
    bool update();
    void initWithParams(SunnyVector2D pos, SunnyVector2D vel);
};


#endif
