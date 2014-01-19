//
//  LOCrashBalls.h
//  LookOut
//
//  Created by Pavel Ignatov on 16.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOCrashBalls_h
#define LookOut_LOCrashBalls_h

#include "LOObject.h"

const short maxCrashCount = 12;

struct LOCrashFireBall
{
    SunnyMatrix pos[stonesInFireModelCount];
    float angles[stonesInFireModelCount];
    float rotationSpeed[stonesInFireModelCount];
    SunnyVector3D velocity[stonesInFireModelCount];
};

struct LOCrashGroundBall
{
    SunnyMatrix pos[stonesInGroundModelCount];
    float angles[stonesInGroundModelCount];
    float rotationSpeed[stonesInGroundModelCount];
    SunnyVector3D velocity[stonesInGroundModelCount];
};

struct LOCrashSnow
{
    SunnyMatrix pos[partsInSnowWall];
    float angles[partsInSnowWall];
    float rotationSpeed[partsInSnowWall];
    SunnyVector3D velocity[partsInSnowWall];
};

class LOCrashBalls
{
    LOCrashFireBall fireBalls[maxCrashCount];
    LOCrashGroundBall groundBalls[maxCrashCount];
    LOCrashSnow snowWall[maxCrashCount];
    
    short fireBallsCount;
    short groundBallsCount;
    short snowWallCount;
public:
    LOCrashBalls();
    ~LOCrashBalls();
    static LOCrashBalls *sharedCrashBalls();
    
    void reset();
    void addFireBallCrash(SunnyMatrix *pos);
    void addGroundBallCrash(SunnyMatrix *pos);
    void addSnowWallCrash(SunnyMatrix *pos);
    
    void update();
    void render(SunnyMatrix *MV);
    void renderShadows();
};

#endif