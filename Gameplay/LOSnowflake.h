//
//  LOSnowflake.h
//  LookOut
//
//  Created by Pavel Ignatov on 09.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOSnowflake_h
#define LookOut_LOSnowflake_h

#include "SunnyVector2D.h"
#include "SUnnyMatrix.h"

extern float snowflakeBackScale;

class LOSnowflake
{
    short dissapearFrame;
    bool animated;
public:
    SunnyVector2D position;
    
    void initWithPosition(SunnyVector2D pos, bool animate = false);
    void render(SunnyMatrix *MV);
    void renderOverlay(SunnyMatrix *MV);
    void renderDissapear(SunnyMatrix *MV);
    static void render(SunnyVector2D pos,float time, float scale, float alpha = 1);
    static void renderOverlay(SunnyVector2D pos, float scale, float alpha = 1);
    static void renderDissapear(SunnyVector2D pos,short frame, float scale);

    static void update();
    
    friend class LOMapInfo;
};


#endif
