//
//  LOSmoke.h
//  LookOut
//
//  Created by Pavel Ignatov on 02.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef LookOut_LOSmoke_h
#define LookOut_LOSmoke_h
#ifndef SKIP_PHYSICS
#include "SunnyOpengl.h"
#endif
#include "SunnyVector2D.h"

#include "LOObject.h"

const short maxSmokesCount = 400;
const short maxGroundSmokesCount = 150;
const short maxAnomalySmokesCount = 30*maxAnomalyBalls;

struct LOOneSmoke
{
    SunnyVector2D position;
    SunnyVector2D velocity;
    float lifeTime;
    short smokeNum;
};

class LOSmoke
{
//#ifndef SKIP_PHYSICS    
//    GLuint verticesVAO;
//    GLuint verticesVBO;
//#endif    
    
    LOOneSmoke smokes[maxSmokesCount];
    short smokesCount;
    
    LOOneSmoke groundSmokes[maxGroundSmokesCount];
    short groundSmokesCount;
    
    LOOneSmoke anomalySmokes[maxAnomalySmokesCount];
    short anomalySmokesCount;
    
    void init();
    void clear();
public:
    static LOSmoke* sharedSmoke();
    LOSmoke();
    ~LOSmoke();
    
    void addRandomSmoke(SunnyVector2D pos);
    void addGroundSmoke(SunnyVector2D pos);
    void addAnomalySmoke(SunnyVector2D pos);
    void update();
    void render();
    void reset();
};

#endif
