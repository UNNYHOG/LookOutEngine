//
//  LOGoldCoin.h
//  LookOut
//
//  Created by Pavel Ignatov on 02.08.13.
//
//

#ifndef __LookOut__LOGoldCoin__
#define __LookOut__LOGoldCoin__

#include <iostream>
#include "SunnyMatrix.h"
#include "SunnyCollisionDetector.h"
const float minCoinHeight = 0.5;
const float coinsVerticalVelocity = 3;
const float coinsHeight = coinsVerticalVelocity*coinsVerticalVelocity/2/3;


class LOGoldCoin
{
protected:
    float verticalVelocity;
    float rotAngle;
    SunnyVector2D velocity;
    float getMove();
    SC_Circle *physics;
    bool physicsCreated;
    float physicsCreationTime;
public:
    LOGoldCoin();
    virtual ~LOGoldCoin();
    SunnyMatrix position;
    
    void initWithPosition(SunnyVector2D pos);
    virtual void renderShadow(SunnyMatrix *MV);
    virtual void render(SunnyMatrix *MV);
    void renderOverlay(SunnyMatrix *MV);
    static void renderForMenu(SunnyVector2D pos);
    static void renderModelForMenu(SunnyMatrix* mat,float alpha, float angle = -M_PI_2);
    void update();
    void collectCoin(bool grab = true);
    
    static void prepareForRender();
    static void prepareModel();
};

#endif /* defined(__LookOut__LOGoldCoin__) */
