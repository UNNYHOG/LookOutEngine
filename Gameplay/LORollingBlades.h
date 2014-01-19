//
//  LORollingBlades.h
//  LookOut
//
//  Created by Pavel Ignatov on 08.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
#ifdef WIN32
#define SKIP_PHYSICS
#endif

#ifndef LookOut_LORollingBlades_h
#define LookOut_LORollingBlades_h
#include "LOObject.h"

#include "SunnyVector2D.h"
#include "SunnyVector4D.h"
#include "SunnyMatrix.h"
#include "SunnyCollisionDetector.h"
struct LOBufElectricField;

class LORollingBlades
{
    float velocityScale;
    short type;//1-rolling, 2-sliding , 3,4 - Electric
    bool isPhysicsAttached;
    SunnyVector4D resetParams;
public:
    float startAngle;//t0 for electric
    SunnyVector2D position;
    SC_StaticBox * body;
    SC_RollingBlade * blade;
    SunnyVector2D bounds;//for not rolling and 
    short charged;
    short attaking;
//    short justActivated;
    
    LORollingBlades();
    ~LORollingBlades();
    void initWithParam(SunnyVector4D params);
    void initWithParam(SunnyVector2D pos, SunnyVector3D params);
    void initElectricity(LOBufElectricField params);
    void render(SunnyMatrix *MV);
    void renderUnderPlayer();
    void renderPhysics();
    void renderElectricFieldShadow(SunnyMatrix *MV);
    void renderOverlay(SunnyMatrix *MV);
    void attachPhysics();
    void detachPhysics();
    
    void reset();
    
//    static void prepareRotatingBlades();
//    static void clearRotatingBlades();
//    static void prepareForRender();
    static void prepareElectricFieldForRender();
};

#endif
