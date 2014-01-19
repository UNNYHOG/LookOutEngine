//
//  LOExplosion.h
//  LookOut
//
//  Created by Pavel Ignatov on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOExplosion_h
#define LookOut_LOExplosion_h
#include "sunnyVector2D.h"
#include "sunnyVector4D.h"
#include "SunnyMatrix.h"

enum LOExplosionType
{
    LOExplosion_Fire = 0,
    LOExplosion_Water,
    LOExplosion_Ground,
    LOExplosion_Snow,
    LOExplosion_Steam,
    LOExplosion_Anomaly,
    LOExplosion_Teleportation,
    LOExplosion_TeleportationPost,
    LOExplosion_Shield,
    LOExplosion_Life,
    LOExplosion_Speed,
};

class LOExplosion
{
    SunnyVector4D cloudRotations;
    SunnyVector2D cloudPosition[3];
    
    SunnyVector4D beamPosition[15];
    SunnyVector3D splashVelocity[15];
    
    SunnyMatrix position;
    short frameNum;
    float frameNumFloat;
    LOExplosionType explosionType;
    void applySound();
public:
    void initWithParams(SunnyMatrix *pos, LOExplosionType type);
    bool update(float deltaTime);
    static void prepareForRender();
    void render(SunnyMatrix *MV);
    void renderUnderPlayer(SunnyMatrix *MV);
    void renderShadow(SunnyMatrix *MV);
    void renderOverlay(SunnyMatrix *MV);
    void render1();
    
    static void prepareExplosions();
    static void clearExplosions();
};

#endif
