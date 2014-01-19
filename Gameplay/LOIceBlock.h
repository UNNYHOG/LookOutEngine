//
//  LOIceBlock.h
//  LookOut
//
//  Created by Pavel Ignatov on 15.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifdef WIN32
#define SKIP_PHYSICS
#endif

#ifndef LookOut_LOIceBlock_h
#define LookOut_LOIceBlock_h

#include "SunnyVector2D.h"
#include "SunnyMatrix.h"
#include "LOObject.h"

#include "SunnyCollisionDetector.h"

class LOIceBlock
{
    SC_StaticBox * body;
    float movingDistance;
    short isMoving;//0 - no, 1-left,2-up,3-right,4-down
    bool hasContact;
    SunnyVector2D lastContactPosition;
    SunnyMatrix insideMatrix;
public:    
    SunnyVector2D position;
    SunnyVector2D curPosition;
    void createPhysics();
    void clearPhysics();
    void render(SunnyMatrix *MV);
    void update();
    void reset();
    void tryToMoveFromPoint(SunnyVector2D point);
    void removeContact();
    void renderShadow();
};

#endif
