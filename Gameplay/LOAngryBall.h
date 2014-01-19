//
//  LOAngryBall.h
//  LookOut
//
//  Created by Pavel Ignatov on 30.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOAngryBall_h
#define LookOut_LOAngryBall_h

#include "SunnyVector2D.h"
#include "SunnyVector4D.h"
#include "LOObject.h"
#include "SunnyCollisionDetector.h"

struct LOBufAngryBall;

struct LOChargeInfo
{
    short charged;
    float chargedTime;
    short chargedFrame;//also framNum for resp
    short chargedDelay;
    float chargedMaxDelay;
    SunnyVector2D chargedPos;
};

class LOAngryBall : public LOObject
{
    SunnyVector2D velocityAdd;
    short state;
    
    float turnOffTime;
    
    bool prediction;
    float predictionTime;
    float stopPrediction;
    float velocityScale;
    short ignoreTrace;
    
    short ignoreSnow;
    
    short type;
    
    LOChargeInfo chargeInfo;
public:
    short multiplayerTargetNum;
    SunnyVector2D jointVelocity;
    
    LOAngryBall();
    ~LOAngryBall();
    void attachMoveBody();
    void initWithParams(SunnyVector2D pos, short targetNum = 0);
    void initBufWithParams(LOBufAngryBall pos);
    void render(SunnyMatrix *MV);
    bool update();
    bool updatePosition();
    void exchange(LOAngryBall * object);
    void renderLight(SunnyMatrix *MV);
    
    void chargeTheBall();
    
    friend class LOMap;
    friend class LOMapInfo;
    
    void setAnomaly();
    
    void setLayers();
};

#endif
