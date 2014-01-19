//
//  LOBonfire.h
//  LookOut
//
//  Created by Pavel Ignatov on 03.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOBonfire_h
#define LookOut_LOBonfire_h

#ifdef WIN32
#define SKIP_PHYSICS
#endif

#include "SunnyVector2D.h"
#include "SunnyVector4D.h"
#ifndef SKIP_PHYSICS
#include "SunnyOpengl.h"
#endif
#include "LOSmoke.h"

const short maxBondireSmokesCount = 60;

class LOBonfire
{
    SunnyVector3D fireRotations;
    
    LOOneSmoke smokes[maxBondireSmokesCount];
    short smokesCount;
    short smokeDelay;
    
    float randomAngle;
    short lightRotate;
    
    short animationFrame;
    SunnyVector2D fireTranslate[2], fireVelocity[2];
    bool noFire;
    float noFireAlpha;
public:
    bool isActive();
    SunnyVector2D position;

//    static void prepareBonfires();
//    static void clearBonfires();
    LOBonfire();
    
    void initWithPosition(SunnyVector2D pos);
//    static void prepareForRender();
    void render(SunnyMatrix *MV);
    void renderAfterPlayer(SunnyMatrix *MV);
    void renderOverlay(SunnyMatrix *MV);
    void update();
    void extinguishFire();
    void reset();
    
    friend class LOMapInfo;
};


#endif
