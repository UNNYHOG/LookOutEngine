//
//  LOVioletSnowflake.h
//  LookOut
//
//  Created by Ignatov on 23.08.13.
//
//

#ifndef __LookOut__LOVioletSnowflake__
#define __LookOut__LOVioletSnowflake__

#include <iostream>
#include "SUnnyMatrix.h"
#include "SunnyCollisionDetector.h"

class LOVioletSnowflake
{
    SC_Circle *physics;
    bool physicsCreated;
    bool haveVelocity;
    float lifeTime;
    float backScale;
//    bool animated;
    float startToDissapear;
    short dissapearFrame;
public:
    LOVioletSnowflake();
    ~LOVioletSnowflake();
    
    SunnyVector2D getPosition();
    
    void initWithPosition(SunnyVector2D pos, SunnyVector2D velocity = SunnyVector2D(0,0));
    static void renderForMenu(SunnyVector2D pos, float scale = 1, float alpha = 1);
    void render(SunnyMatrix *MV);
    void renderOverlay(SunnyMatrix *MV);
    bool update();
    void prepareToDissapear();
    void collectSnowflake(bool grab = true);
    static void render(SunnyVector2D pos,float time, float scale, float alpha = 1);
    static void renderOverlay(SunnyVector2D pos, float scale, float alpha = 1);
};

#endif /* defined(__LookOut__LOVioletSnowflake__) */
