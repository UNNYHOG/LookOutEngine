//
//  LOGroundAnimation.h
//  LookOut
//
//  Created by Pavel Ignatov on 09.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOGroundAnimation_h
#define LookOut_LOGroundAnimation_h
#include "SunnyModelObj.h"

class LOGroundAnimation : public SunnyModelObj
{
    SunnyVector3D *positions;
public:
    void prepareLOGroundAnimation();
    LOGroundAnimation();
    ~LOGroundAnimation();
    
    void prepareForRender();
    void render(SunnyMatrix *MV, float centerAngle,SunnyVector2D *angles);
    void renderCenterBall();
    void renderShadowsByMatrices(SunnyMatrix * matrices);
    void renderByMatrices(SunnyMatrix *MV, SunnyMatrix * matrices, float * angles);
    void getMatricesForCrash(SunnyMatrix matrices[]);
};

#endif