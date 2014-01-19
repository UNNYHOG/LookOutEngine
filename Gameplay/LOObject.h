//
//  LOObject.h
//  LookOut
//
//  Created by Pavel Ignatov on 07.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOObject_h
#define LookOut_LOObject_h

#include "SunnyVector2D.h"
#include "SUnnyMatrix.h"
#include "SunnyCollisionDetector.h"

const short stonesInGroundModelCount = 5;
const short stonesInFireModelCount = 9;
const short partsInSnowWall = 11;//5-big,6 small

const short maxFireBalls = 20;
const short maxWaterBalls = 12;
const short maxGroundBalls = 12;
const short maxAngryBalls = 10;
const short maxElectricBalls = 20;
const short maxAnomalyBalls = 12;

enum LOStatus
{
    LO_None = 0,
    LO_Respawn,
    LO_Active,
};

const float customRespawnTime = 0.5;
extern float currentRespawnTime;

const short customSmokeDelay = 2;

class LOObject
{
protected:
    short frameNum;
    bool skipFrame;
    SC_Circle * body;
    
    float respawnTime;
    LOStatus status;
private:
    void init();
    void clear();
public:
    SunnyMatrix position;
    SunnyVector2D velocity;
    
    LOObject();
    virtual ~LOObject();
    
//    static void prepareObjects();
//    static void clearObjects();
//    static void prepareForRender();
    
    virtual void initWithParams(SunnyVector2D pos, SunnyVector2D vel);
    void setPosition(SunnyVector2D pos);
    void setPosition(SunnyMatrix mat);
    virtual void exchange(LOObject * object);
    
    void attachPhysics();
    void detachPhysics();
    void continueAttaching();
    virtual void render();
    virtual void renderPhysics();
    void renderBottomLayer();
    void renderTopLayerWithRotation();
    void setVelocity(SunnyVector2D vel);
    SunnyVector2D getPosition();
    SunnyVector2D getVelocity();
    virtual bool update();
    virtual bool updatePosition();
    
    friend class LOMap;
    float distanceToPoint(SunnyVector2D p);
    float distanceSqToPoint(SunnyVector2D p);
    
    bool detach;
    
    friend class LOMap;
};

#endif